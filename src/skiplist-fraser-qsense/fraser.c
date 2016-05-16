/*
 * File:
 *   fraser.c
 * Author(s):
 *   Vincent Gramoli <vincent.gramoli@epfl.ch>
 * Description:
 *   Lock-based skip list implementation of the Fraser algorithm
 *   "Practical Lock Freedom", K. Fraser, 
 *   PhD dissertation, September 2003
 *   Cambridge University Technical Report UCAM-CL-TR-579 
 *
 * Copyright (c) 2009-2010.
 *
 * fraser.c is part of Synchrobench
 * 
 * Synchrobench is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "fraser.h"
#include "qsbr_smr_hybrid.h"

RETRY_STATS_VARS;

#include "latency.h"
#if LATENCY_PARSING == 1
__thread size_t lat_parsing_get = 0;
__thread size_t lat_parsing_put = 0;
__thread size_t lat_parsing_rem = 0;
#endif	/* LATENCY_PARSING == 1 */

extern ALIGNED(CACHE_LINE_SIZE) unsigned int levelmax;

#define FRASER_MAX_MAX_LEVEL 32 /* covers up to 2^64 elements */

void fraser_search(sl_intset_t *set, skey_t key, sl_node_t **left_list,
        sl_node_t **right_list) {
    int i;
    sl_node_t *left, *left_next, *right, *right_next;

    retry: PARSE_TRY();

    left = set->head;
    ///////////////////
    assign_hp(left, 0);
    ///////////////////

    for (i = levelmax - 1; i >= 0; i--) {
        left_next = left->next[i];
        //////////////////////////////////////
        assign_hp(GET_UNMARKED(left_next), 1);
        if (left_next != left->next[i]) {
            goto retry;
        }
        //////////////////////////////////////

        if (unlikely(IS_MARKED(left_next))) {
            goto retry;
        }

        /* Find unmarked node pair at this level */
        right = left_next;
        ////////////////////
        assign_hp(right, 2);
        ////////////////////

        for (;;) {
            /* Skip a sequence of marked nodes */
    
            right_next = right->next[i];
            //////////////////////////////////////
            assign_hp(GET_UNMARKED(right_next), 3);
            if (right_next != right->next[i]) {
                goto retry;
            }
            //////////////////////////////////////

            while (unlikely(IS_MARKED(right_next))) {
                right = GET_UNMARKED(right_next);
                ////////////////////
                assign_hp(right, 2);
                ////////////////////

                right_next = right->next[i];
                //////////////////////////////////////
                assign_hp(GET_UNMARKED(right_next), 3);
                if (right_next != right->next[i]) {
                    goto retry;
                }
                //////////////////////////////////////
                
            }

            if (right->key >= key) {
                break;
            }

            left = right;
            /////////////////////////////////
            assign_hp(GET_UNMARKED(left), 0);
            /////////////////////////////////

            left_next = right_next;
            //////////////////////////////////////
            assign_hp(GET_UNMARKED(left_next), 1);
            //////////////////////////////////////

            right = right_next;
            //////////////////////////////////
            assign_hp(GET_UNMARKED(right), 2);
            //////////////////////////////////
        }

        /* Ensure left and right nodes are adjacent */
        if ((left_next != right)) {
            if ((!ATOMIC_CAS_MB(&left->next[i], left_next, right))) {
                CLEANUP_TRY();
                goto retry;
            }
        }

        if (left_list != NULL) {
            left_list[i] = left;
            ///////////////////////////////////
            assign_hp(GET_UNMARKED(left), 4+i);
            ///////////////////////////////////
        }
        if (right_list != NULL) {
            right_list[i] = right;
            /////////////////////////////////////
            assign_hp(GET_UNMARKED(right), 36+i);
            /////////////////////////////////////
        }
    }
}

sval_t fraser_find(sl_intset_t *set, skey_t key) {
    sl_node_t* succs[FRASER_MAX_MAX_LEVEL];
    sval_t result = 0;

    PARSE_START_TS(0);
    fraser_search(set, key, NULL, succs);
    PARSE_END_TS(0, lat_parsing_get++);

    if (succs[0]->key == key && !succs[0]->deleted) {
        result = succs[0]->val;
    }
    return result;
}

inline void mark_node_ptrs(sl_node_t *n) {
    int i;
    sl_node_t *n_next;

    for (i = n->toplevel - 1; i >= 0; i--) {
        do {
            n_next = n->next[i];
            if (IS_MARKED(n_next)) {
                break;
            }
        } while (!ATOMIC_CAS_MB(&n->next[i], n_next,
                GET_MARKED(n_next)));
    }
}

sval_t fraser_remove(sl_intset_t *set, skey_t key) {
    /* sl_node_t **succs; */
    sl_node_t* succs[FRASER_MAX_MAX_LEVEL];
    sval_t result = 0;

    UPDATE_TRY();

    PARSE_START_TS(2);
    fraser_search(set, key, NULL, succs);
    PARSE_END_TS(2, lat_parsing_rem++);

    if (succs[0]->key != key) {
        goto end;
    }
    /* 1. Node is logically deleted when the deleted field is not 0 */
    if (succs[0]->deleted) {
        goto end;
    }

    if (ATOMIC_FETCH_AND_INC_FULL(&succs[0]->deleted) == 0) {
        /* 2. Mark forward pointers, then search will remove the node */
        mark_node_ptrs(succs[0]);

        result = succs[0]->val;
        sl_delete_node(succs[0]);

        /* MEM_BARRIER; */
        fraser_search(set, key, NULL, NULL);
    }

    end: return result;
}

int fraser_insert(sl_intset_t *set, skey_t key, sval_t val) {
    sl_node_t *new, *new_next, *pred, *succ;
    sl_node_t *succs[FRASER_MAX_MAX_LEVEL], *preds[FRASER_MAX_MAX_LEVEL];
    int i;
    int result = 0;
    uint32_t top_level = get_rand_level();

    new = sl_new_simple_node(key, val, top_level, 0);

    int trials = 0;
    while (new == NULL) {
        
        allocate_fail(trials);
        trials++;

        new = sl_new_simple_node(key, val, top_level, 0);
    }


    PARSE_START_TS(1);
    retry: UPDATE_TRY();

    fraser_search(set, key, preds, succs);
    PARSE_END_TS(1, lat_parsing_put);

    /* Update the value field of an existing node */
    if (succs[0]->key == key) { /* Value already in list */
        if (succs[0]->deleted) { /* Value is deleted: remove it and retry */
            mark_node_ptrs(succs[0]);
            goto retry;
        }
        result = 0;
        sl_delete_node(new);
        goto end;
    }

    for (i = 0; i < new->toplevel; i++) {
        new->next[i] = succs[i];
    }

#if defined(__tile__)
    MEM_BARRIER;
#endif

    /* Node is visible once inserted at lowest level */
    if (!ATOMIC_CAS_MB(&preds[0]->next[0], succs[0], new)) {
        goto retry;
    }

    for (i = 1; i < new->toplevel; i++) {
        while (1) {
            pred = preds[i];
            succ = succs[i];
            /* Update the forward pointer if it is stale */
            new_next = new->next[i];
            if (IS_MARKED(new_next)) {
                goto success;
            }
            if ((new_next != succ)
                    && (!ATOMIC_CAS_MB(&new->next[i],
                            GET_UNMARKED(new_next), succ)))
                break; /* Give up if pointer is marked */
            /* Check for old reference to a k node */
            if (succ->key == key) {
                succ = GET_UNMARKED(succ->next);
            }
            /* We retry the search if the CAS fails */
            if (ATOMIC_CAS_MB(&pred->next[i], succ, new))
                break;

            fraser_search(set, key, preds, succs);
        }
    }

    success: result = 1;
    end: PARSE_END_INC(lat_parsing_put);
    return result;
}


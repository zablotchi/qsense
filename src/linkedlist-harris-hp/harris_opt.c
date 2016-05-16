/*
 *  linkedlist.c
 *
 *  Description:
 *   Lock-free linkedlist implementation of Harris' algorithm
 *   "A Pragmatic Implementation of Non-Blocking Linked Lists" 
 *   T. Harris, p. 300-314, DISC 2001.
 */

#include "linkedlist.h"
#include "smr.h"

extern __thread smr_data_t sd;


RETRY_STATS_VARS;

/*
 * The five following functions handle the low-order mark bit that indicates
 * whether a node is logically deleted (1) or not (0).
 *  - is_marked_ref returns whether it is marked, 
 *  - (un)set_marked changes the mark,
 *  - get_(un)marked_ref sets the mark before returning the node.
 */
inline int is_marked_ref(node_t* i) {
    return ((uintptr_t) i & 0x1L);
}

inline node_t*
get_unmarked_ref(node_t* w) {
    return (node_t*) ((uintptr_t) w & ~0x1L);
}

inline node_t*
get_marked_ref(node_t* w) {
    return (node_t*) ((uintptr_t) w | 0x1L);
}

static inline int physical_delete_right(node_t* left_node, node_t* right_node) {
    node_t* new_next = get_unmarked_ref(right_node->next);
    node_t* res = CAS_PTR(&left_node->next, right_node, new_next);
    int removed = (res == right_node);
    if (likely(removed)){
        free_node_later((void*) res);
    }
    return removed;
}

/*
 * list_search looks for value val, it
 *  - returns right_node owning val (if present) or its immediately higher 
 *    value present in the list (otherwise) and 
 *  - sets the left_node to the node owning the value immediately lower than val. 
 * Encountered nodes that are marked as logically deleted are physically removed
 * from the list, yet not garbage collected.
 */
static inline node_t*
list_search(intset_t* set, skey_t key, node_t** left_node_ptr) {
    PARSE_TRY();
    node_t *left_node, *right_node;
    int base = K * sd.thread_index;
    int offset = 0;

try_again_search:    
    left_node = set->head;
    right_node = (set->head->next);
    
    while (1) {

        HP[base + offset].p = right_node;
        MEM_BARRIER;
        if (right_node != (left_node->next)) {
            goto try_again_search;
        }
        offset = 1-offset;

        if (right_node->key == 600000) {
            fprintf(stderr, "Touched illegal node in search\n");
        }
        
        if (likely(!is_marked_ref(right_node->next))) {
            if (unlikely(right_node->key >= key)) {
                break;
            }
            left_node = right_node;
        } else {
            CLEANUP_TRY();
            physical_delete_right(left_node, right_node);
        }
        right_node = get_unmarked_ref(right_node->next);
    }
    *left_node_ptr = left_node;
    return right_node;
}

/*
 * returns a value different from 0 if there is a node in the list owning value val.
 */
sval_t harris_find(intset_t* the_list, skey_t key) {

    node_t *left_node, *right_node;
    int base = K * sd.thread_index;
    int offset = 0;

try_again_search:    
    left_node = the_list->head;
    right_node = the_list->head->next;
    
    while (1) {

        HP[base + offset].p = right_node;
        MEM_BARRIER;
        if (right_node != left_node->next) {
            goto try_again_search;
        }
        offset = 1-offset;

        if (right_node->key == 600000) {
            fprintf(stderr, "Touched illegal node in find\n");
        }
        
        
        if (unlikely(right_node->key >= key)) {
            break;
        }

        left_node = right_node;

        right_node = get_unmarked_ref(right_node->next);
    }

    if (right_node->key == key && !is_marked_ref(right_node->next)) {
        return right_node->val;
    }
    return 0;
}

/*
 * inserts a new node with the given value val in the list
 * (if the value was absent) or does nothing (if the value is already present).
 */
int harris_insert(intset_t *the_list, skey_t key, sval_t val) {
    do {
        UPDATE_TRY();
        node_t* left_node;
        node_t* right_node = list_search(the_list, key, &left_node);
        if (right_node->key == key) {
            return 0;
        }

        node_t* node_to_add = new_node(key, val, right_node, 0);
        if (node_to_add == NULL){
            scan();
            node_to_add = new_node(key, val, right_node, 0);
        }

        // Try to swing left_node's unmarked next pointer to a new node

        if (CAS_PTR(&left_node->next, right_node, node_to_add) == right_node) {
            return 1;
        }
        ssfree_alloc(0, (void*) node_to_add);
    } while (1);
}

/*
 * deletes a node with the given value val (if the value is present) 
 * or does nothing (if the value is already present).
 * The deletion is logical and consists of setting the node mark bit to 1.
 */
sval_t harris_delete(intset_t *the_list, skey_t key) {
    node_t* cas_result;
    node_t* unmarked_ref;
    node_t* left_node;
    node_t* right_node;

    do {
        UPDATE_TRY();
        right_node = list_search(the_list, key, &left_node);

        if (right_node->key != key) {
            return 0;
        }

        // Try to mark right_node as logically deleted
        unmarked_ref = get_unmarked_ref(right_node->next);
        node_t* marked_ref = get_marked_ref(unmarked_ref);
        cas_result = CAS_PTR(&right_node->next, unmarked_ref, marked_ref);
    } while (cas_result != unmarked_ref);

    sval_t ret = right_node->val;

    if (!physical_delete_right(left_node, right_node)) {
        list_search(the_list, key, &left_node);
    }

    return ret;
}

int set_size(intset_t *set) {
    size_t size = 0;
    node_t* node;

    /* We have at least 2 elements */
    node = get_unmarked_ref(set->head->next);
    while (get_unmarked_ref(node->next) != NULL) {
        if (!is_marked_ref(node->next))
            size++;
        node = get_unmarked_ref(node->next);
    }

    return size;
}

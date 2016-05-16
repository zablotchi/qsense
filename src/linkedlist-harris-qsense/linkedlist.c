/*
 *  linkedlist.c
 *  
 *  Linked list data structure
 *
 *  Created by Vincent Gramoli on 1/12/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "linkedlist.h"


node_t*
new_node(skey_t key, sval_t val, node_t *next, int initializing) {
    volatile node_t *node;


    node = (volatile node_t *) ssalloc_alloc(0, sizeof(node_t));

    if (node == NULL) {
        return NULL;
        //perror("malloc @ new_node");
        //exit(1);
    }

    node->key = key;
    node->val = val;
    node->next = next;
    return (node_t*) node;
}

intset_t*
set_new() {
    intset_t *set;
    node_t *min, *max;

    // trying memalign instead of ssalloc_aligned
    if ((set = (intset_t*) memalign(CACHE_LINE_SIZE, sizeof(intset_t)))
            == NULL) {
        perror("malloc");
        exit(1);
    }

    max = new_node(KEY_MAX, 0, NULL, 1);
    min = new_node(KEY_MIN, 0, max, 1);
    set->head = min;

    return set;
}

void set_delete(intset_t *set) {
    node_t *node, *next;

    node = set->head;
    while (node != NULL) {
        next = node->next;
        free((void*) node);
        node = next;
    }
    free(set);
}


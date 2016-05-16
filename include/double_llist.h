#ifndef _DLLIST_
#define _DLLIST_

#include <mr.h>

struct double_llist {
    mr_node_t *head, *tail;
    uint32_t size; 
};

typedef struct double_llist double_llist_t;

void add_to_head(double_llist_t* list, mr_node_t* node);

mr_node_t* remove_from_tail(double_llist_t* list);

mr_node_t* remove_node(double_llist_t* list, mr_node_t* node);

void init(double_llist_t* list);

#endif
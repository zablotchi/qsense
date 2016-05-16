#include "double_llist.h"

void add_to_head(double_llist_t* list, mr_node_t* node) {
    node->mr_next = list->head;
    node->mr_prev = NULL;
    
    if (list->head == NULL) {
        list->tail = node;           
    } else {
        list->head->mr_prev = node;
    }

    list->head = node;
    list->size++;
}

mr_node_t* remove_from_tail(double_llist_t* list) {
    if (list->tail == NULL) {
        return NULL;
    }

    mr_node_t* res = list->tail;
    list->tail = res->mr_prev;

    if (list->tail != NULL) {
        list->tail->mr_next = NULL;
    } else {
        list->head = NULL;
    }

    res->mr_prev = NULL;
    list->size --;
    return res;
}

void init(double_llist_t* list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

mr_node_t* remove_node(double_llist_t* list, mr_node_t* node){
    if (node->mr_prev != NULL){
        (node->mr_prev)->mr_next = node->mr_next;
    } else{
        //update list head
        list->head = node->mr_next;
    }

    if(node->mr_next != NULL){
        (node->mr_next)->mr_prev = node->mr_prev;
    } else{
        //update list tail
        list->tail = node->mr_prev;
    }

    //update list size
    list->size --;

    return node;
}

/******************************************************************************
 *
 * Functions which all memory reclamation schemes must provide.
 *
 *****************************************************************************/

#ifndef MR_H
#define MR_H

 #include "utils.h"

void mr_init_global(uint8_t nthreads);
void mr_init_local(uint8_t thread_index, uint8_t nthreads);
void mr_thread_exit();
void mr_exit_global();
void mr_reinitialize();

struct mr_node {
    void *actual_node;
    struct timeval created; 
    struct mr_node *mr_prev;
    struct mr_node *mr_next;
    char padding[CACHE_LINE_SIZE - sizeof(void *) - sizeof(struct timeval) - 2*sizeof(struct mr_node*)];
};

typedef ALIGNED(CACHE_LINE_SIZE) struct mr_node mr_node_t;

#endif

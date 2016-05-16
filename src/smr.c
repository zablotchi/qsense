/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Copyright (c) Thomas E. Hart.
 */

#include "smr.h"
#include "ssalloc.h"
#include <stdio.h>
#include <stdlib.h>


// IGOR: SSALLOC allocator convention:
// 0 is for actual nodes
// 1 is for m_nodes

__thread smr_data_t sd;

void mr_init_global(uint8_t nthreads){
  /* Allocate HP array. Over-allocate since the parent has pid 32. */
  HP = (hazard_pointer_t *)malloc(sizeof(hazard_pointer_t) * K*nthreads);
  if (HP == NULL) {
    fprintf(stderr, "SMR mr_init: out of memory\n");
    exit(-1);
  }

  /* Initialize the hazard pointers. */
  int i;
  for (i = 0; i < K*(nthreads); i++)
    HP[i].p = NULL;
}

void mr_init_local(uint8_t thread_index, uint8_t nthreads){

  sd.rlist = (double_llist_t*) malloc(sizeof(double_llist_t));
  init(sd.rlist);

  sd.rcount = 0;
  sd.thread_index = thread_index;
  sd.nthreads = nthreads;
  sd.hp_index_base = thread_index * K;
  sd.plist = (void **) malloc(sizeof(void *) * K * sd.nthreads);
}


void mr_thread_exit()
{
    int i;
    
    for (i = 0; i < K; i++)
        HP[K * sd.thread_index + i].p = NULL;
    
    while (sd.rcount > 0) {
        scan();
        sched_yield();
    }
}

void mr_reinitialize()
{
}

/* 
 * Comparison function for qsort.
 *
 * We just need any total order, so we'll use the arithmetic order 
 * of pointers on the machine.
 *
 * Output (see "man qsort"):
 *  < 0 : a < b
 *    0 : a == b
 *  > 0 : a > b
 */

int compare (const void *a, const void *b)
{
  return ( *(uint64_t*)a - *(uint64_t*)b );
}

inline int ssearch(void **list, size_t size, void *key) {
    int i;
    for (i = 0; i < size; i++) {
      if (list[i] == key) {
        return 1;
      }
    }
    return 0;
}

void scan()
{
    /* Iteratation variables. */
    mr_node_t *cur;
    int i;

    /* List of SMR callbacks. */
    double_llist_t tmplist;
    init(&tmplist);

    /* List of hazard pointers, and its size. */
    void **plist = sd.plist;
    size_t psize;

    /*
     * Make sure that the most recent node to be deleted has been unlinked
     * in all processors' views.
     *
     * Good:
     *   A -> B -> C ---> A -> C ---> A -> C
     *                    B -> C      B -> POISON
     *
     * Illegal:
     *   A -> B -> C ---> A -> B      ---> A -> C
     *                    B -> POISON      B -> POISON
     */
    //write_barrier();
    MEM_BARRIER;

    /* Stage 1: Scan HP list and insert non-null values in plist. */
    psize = 0;
    for (i = 0; i < H; i++) {
      if (HP[i].p != NULL){
        plist[psize] = HP[i].p;
        psize++;
      }
    }

    
    /* Stage 2: Sort the plist. */
    /* For now, just do linear search*/
    //qsort(plist, psize, sizeof(void *), compare);

    /* Stage 3: Free non-harzardous nodes. */
    sd.rcount = 0;

    tmplist.head = sd.rlist->head;
    tmplist.tail = sd.rlist->tail;
    tmplist.size = sd.rlist->size;

    init(sd.rlist);

    // Setting rcount to 0 here would ignore nodes from rlist
    while (tmplist.size > 0) {
        /* Pop cur off top of tmplist. */
        cur = remove_from_tail(&tmplist);
        if (ssearch(plist, psize, cur->actual_node)) {
            add_to_head(sd.rlist, cur);
            sd.rcount++;
        } else {
            ssfree_alloc(0, cur->actual_node);
            ssfree_alloc(1, cur);
        }
    }
}

void free_node_later(void *n)
{
    mr_node_t* wrapper_node = ssalloc_alloc(1, sizeof(mr_node_t));
    wrapper_node->actual_node = n;

    add_to_head(sd.rlist, wrapper_node);
    sd.rcount++;

    if (sd.rcount >= R) {
        scan();
    }
}

inline void assign_hp(volatile void* target, int hp_index) {
  HP[sd.hp_index_base + hp_index].p = target;
  MEM_BARRIER;
}


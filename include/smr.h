/*
 * Functionality for hazard pointers, aka safe memory reclamation (SMR).
 *
 * Uses a linked list for the hazard poitners. Using an array can be a 
 * related experiment.
 *
 * Follows the pseudocode given in :
 *  M. M. Michael. Hazard Pointers: Safe Memory Reclamation for Lock-Free 
 *  Objects. IEEE TPDS (2004) IEEE Transactions on Parallel and Distributed 
 *  Systems 15(8), August 2004.
 *
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
 
#ifndef __SMR_H
#define __SMR_H

#include "mr.h"
#include "double_llist.h"

/* Parameters to the algorithm:
 *  K: Number of hazard pointers per CPU.
 *  H: Number of hazard pointers required.
 *  R: Chosen such that R = H + Omega(H).
 */
#define K 69 // 3 + 2 * max_level (skiplist)
// #define K 6 // BST
// #define K 2 // linked list

#define H (K * sd.nthreads)
#define R (100 + 2*H)
#define SCAN_THRESHOLD 50

#define MAX_EXIT_RETRIES 100 * sd.nthreads

 
struct hazard_pointer {
    void *p;
    char padding[CACHE_LINE_SIZE - sizeof(void *)];
};

typedef ALIGNED(CACHE_LINE_SIZE) struct hazard_pointer hazard_pointer_t;

/* Must be dynamically initialized to be an array of size H. */
hazard_pointer_t *HP;

struct smr_data {
  double_llist_t *rlist;
  /* Nodes that are old enough to be deleted are added here from the rlist*/
  double_llist_t *vlist;
  void **plist;

  /* rcount = rlist_size + vlist_size*/
  uint32_t rcount;
  uint32_t nthreads;
  uint32_t thread_index;
  uint32_t hp_index_base;
  uint8_t free_calls;
  char padding[CACHE_LINE_SIZE - 4*sizeof(uint32_t) - 2*sizeof(double_llist_t *) - sizeof(void **) - sizeof(uint8_t)];
};

typedef ALIGNED(CACHE_LINE_SIZE) struct smr_data smr_data_t;

void scan();

void free_node_later(void *);

inline void assign_hp(volatile void* target, int hp_index);
 
#endif

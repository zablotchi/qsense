#ifndef __QSBR_SMR_HYBRID_H
#define __QSBR_SMR_HYBRID_H

#include "mr.h"
#include "lock_if.h"
#include "sleeper_threads.h"
#include "double_llist.h"

// QSBR 
#define FUZZY 0
#define NOT_FUZZY 1

#define N_EPOCHS 3
#define QUIESCENCE_THRESHOLD 1000
#define SWITCH_THRESHOLD 10

// SMR 

/* Parameters to the algorithm:
 *  K: Number of hazard pointers per CPU.
 *  H: Number of hazard pointers required.
 *  R: Chosen such that R = H + Omega(H).
 */
#define K 69
#define H (K * ltd.nthreads)
#define R (100 + 2*H)

#define MAX_EXIT_RETRIES 100 * ltd.nthreads

typedef ALIGNED(CACHE_LINE_SIZE) struct fallback_flag {
  volatile uint8_t flag;
  char padding[CACHE_LINE_SIZE - sizeof(uint8_t)];
} fallback_flag_t;

fallback_flag_t fallback;

struct hazard_pointer {
    void *p;
    char padding[CACHE_LINE_SIZE - sizeof(void *)];
};

typedef ALIGNED(CACHE_LINE_SIZE) struct hazard_pointer hazard_pointer_t;

/* Must be dynamically initialized to be an array of size H. */
hazard_pointer_t *HP;

struct qsbr_globals {
    ptlock_t update_lock ALIGNED(CACHE_LINE_SIZE);
    uint8_t global_epoch ALIGNED(CACHE_LINE_SIZE);

};

struct shared_thread_data {
    /* EBR per-thread data:
     *  limbo_list: three lists of nodes awaiting physical deletion, one
     *              for each epoch
     *  in_critical: flag telling us whether we're in a critical section
     *               with respect to memory reclamation
     *  epoch: the local epoch
     */
    uint8_t epoch;
    uint8_t in_critical;
    uint8_t is_present;
    uint64_t process_callbacks_count;
    uint64_t scan_count;
    uint64_t allocate_fail_count;


    char padding[CACHE_LINE_SIZE - 3 * sizeof(uint8_t)  - 3 * sizeof(uint64_t)];
};

struct local_thread_data {
  double_llist_t *limbo_list [N_EPOCHS];
  double_llist_t *vlist;
  void **plist;
  uint64_t rcount;
  uint8_t nthreads;
  uint8_t thread_index;
  uint8_t last_flag;
  uint32_t free_calls;
  uint32_t hp_index_base;

  char padding[CACHE_LINE_SIZE - N_EPOCHS * sizeof(double_llist_t *) - sizeof(double_llist_t *) - sizeof(uint64_t) - sizeof(void **) - 3 * sizeof(uint8_t) - 2* sizeof(uint32_t)];
};

typedef ALIGNED(CACHE_LINE_SIZE) struct shared_thread_data shared_thread_data_t;
typedef ALIGNED(CACHE_LINE_SIZE) struct local_thread_data local_thread_data_t; 

void scan();

void quiescent_state (int flag);

void manage_hybrid_state();

void free_node_later(void *);

//Called when impossible to allocate node; triggers switch to SMR.
void allocate_fail(int trials);

inline void assign_hp(volatile void* target, int hp_index);

inline void assign_hp_fake(volatile void* target, int hp_index);


#endif

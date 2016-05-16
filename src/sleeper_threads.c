#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sched.h>
#include <inttypes.h>
#include <sys/time.h>
#include <unistd.h>
#include <malloc.h>

#include "utils.h"
#include "atomic_ops.h"
#include "rapl_read.h"
#include "sleeper_threads.h"

static volatile int wakeup_stop;
int num_sleeper_threads = 0;
sleeper_thread_data_t* slthds = NULL;
pthread_t* sleeper_threads = NULL;


void (*maintenance_func)(void);

void* wakeup(void * arg) {

    sleeper_thread_data_t* sthd = (sleeper_thread_data_t*) arg;
    uint32_t core = sthd->target_core;
    int phys_id = the_cores[core];
    set_cpu(phys_id);

    struct timespec timeout;
    timeout.tv_sec = sthd->sleep_millis / 1000;
    timeout.tv_nsec = (sthd->sleep_millis % 1000) * 1000000;

    while (wakeup_stop == 0) {
        // One of the sleeper threads periodically resets the presence vector
            
        if (core == 0){
        	maintenance_func();
        } 
        nanosleep(&timeout, NULL);
    }

    pthread_exit(NULL);
}

//called from global mr_init
void init_sleeper_threads(int worker_threads, void (*func)(void)){

	// Create sleeper threads, one per core
	maintenance_func = func;    

    size_t num_cores = CORES_PER_SOCKET * NUMBER_OF_SOCKETS;

    if (worker_threads < num_cores){
    	num_sleeper_threads = worker_threads;
    } else {
    	num_sleeper_threads = num_cores;
    }
    
    printf("num_sleeper_threads %d\n", num_sleeper_threads);

    sleeper_threads = (pthread_t *) malloc(num_sleeper_threads * sizeof(pthread_t));
    slthds = (sleeper_thread_data_t *) malloc(
            num_sleeper_threads * sizeof(sleeper_thread_data_t));

    wakeup_stop = 0;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


    int t, rc;

    for (t = 0; t < num_sleeper_threads; t++) {

		slthds[t].target_core = t;
        slthds[t].sleep_millis = SLEEP_AMOUNT;
        rc = pthread_create(&sleeper_threads[t], &attr, wakeup,
                slthds + t); 
        if (rc) {
            printf("ERROR SLEEPER THREADS; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);

}

//called from global mr_exit
void join_sleeper_threads(){

    void *status;
    wakeup_stop = 1;
    int t, rc;

    for (t = 0; t < num_sleeper_threads; t++) {
    	rc = pthread_join(sleeper_threads[t], &status);
        if (rc) {

            printf("ERROR SLEEPER THREADS; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }

    free(slthds);
    free(sleeper_threads);

}
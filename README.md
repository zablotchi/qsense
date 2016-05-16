# How to run the tests

0. This code was developed and tested on a 48 core AMD Opteron with four 12-core 2.1 GHz Processors and 128 GB of RAM, running Ubuntu 14.04. Our code was compiled with GCC 4.8.4.

# Linked list
### Preliminaries

1. go to `include/smr.h` and uncomment the line `#define K 2` (commenting out all other definitions of K from that file)

2. In `ssalloc.h` uncomment 
`#define SSALLOC_SIZE_SMALL 2 * 1024  * 1024 // ll i2000 r4000 u50`.

### Throughput test (scalability)

3. From the root folder, run `make linkedlist_mr`. This will produce the binaries in the `bin` folder.
4. Either run the binaries one by one: `./bin/${binary} -u{0,50,100} -i2000 -r4000 -n{1,2,4,8,16,32}` or run scalability test with all of them: `./scripts/scalability${x}.sh "1 2 4 8 16 32" [the x binaries from `bin`] -u{0,50,100} -i2000 -r4000`  

The arguments stand for the following:
* `-i` specifies the initial size of the data structure (it will be rounded up to the next power of two )
* `-r` specifies the size of the key range (it will be rounded up to the next power of two )
* `-n` specifies the number of worker threads
* `-u` specifies the percentage of updates
* `-d` specifies the duration of the experiment in milliseconds

### Robustness test (`test time`)

3. From the root folder, run `make ll_harris_qsbr ll_harris_hp ll_harris_qsense TEST=time`
4. Run the binaries one by one: 
    * `./bin/linkedlist-harris-qsbr -x100 -u50 -n8 -i2000 -r4000`. Note that normally the qsbr version blocks after 10 seconds, so the test needs to be stopped manually (e.g. using Ctrl-C).
    * `./bin/linkedlist-harris-hp -x100 -u50 -n8 -i2000 -r4000`
    * `./bin/linkedlist-harris-qsense -x100 -u50 -n8 -i2000 -r4000`

Here the `-x100` means that the test will last for 100 seconds and one thread will be delayed for ten seconds every 20 seconds (starting with t=10 seconds). Changing the value of x changes the length of the test but not the delay behaviour/intervals. 

# Skip list

### Preliminaries

1. go to `include/smr.h` and uncomment the line `#define K 69` (commenting out all other definitions of K from that file). Note that this value of 69 covers the FRASER_MAX_MAX_LEVEL (the maximum posible value of levelmax --- the maximum node height for a given skiplist size; levelmax is computed as log2(key_range) for each instance of a skiplist but it cannot go above FRASER_MAX_MAX_LEVEL) of 32. For a skiplist of range 40000, like we use in the paper, levelmax is going to be 16, so it is correct when we say in the paper that the skiplist uses up to 35 hazard pointers.

2. In `ssalloc.h` uncomment 
`#define SSALLOC_SIZE_SMALL 200 * 1024  * 1024 // sl i20000 r40000 u50`

For the throughput and robustness tests, everything is the same as for the linked list, except that the binary names change (check the big Makefile as well as the small makefiles in each data structure folder for the names) and the sizes change: `-i20000 -r40000`

# BST

### Preliminaries

1. go to `include/smr.h` and uncomment the line `#define K 6` (commenting out all other definitions of K from that file)

2. In `ssalloc.h` uncomment 
#define SSALLOC_SIZE_SMALL 320 * 1024  * 1024 // bst i2000000 u50


For the throughput and robustness tests, everything is the same as for the linked list, except that the binary names change (check the big Makefile as well as the small makefiles in each data structure folder) and the sizes change: `-i2000000 -r4000000`

# Compilation Flags

### Ours

* TEST=time : do the time test with intermittent delays (default is throughtput test)
* OPT_P=1 : enable scan periodicity optimization
* OPT_R=1 : enable removed list ordering optimization
* OPT_B=1 : enable Bloom filters optimization
* INIT=one : initialize the structure from one thread (default is from all)

### Inherited from ASCYLIB

* VERSION =   DEBUG : compile with O0 and debug flags
              SYMBOL :  compile with O3 and -g
     O0 : compile with -O0
     O1 : compile with -O1
     O2 : compile with -O2
     O3 : compile with -O3 (default)
* GC=0 or GC=1 : disable/enable GC (default is 1)
* TEST=old : use the old test.c file (disables GC)
* TEST=correct : use the test_correct.c file (enables GC)
* LATENCY= 1 : enable per operation latency measurements with getticks
           2 : enable per operation latency measurements with sspfd (only id==0 prints results)
       3 : enable per operation latency measurements with sspfd (all cores print results)
* GRANULARITY=GLOBAL_LOCK (or G=GL) for global lock
* INIT=all : initialize the structure from all threads (default is from one)
* LFSL= fraser  : compile with Fraser's lock-free skip list algo
        herlihy : compile with Herlihy's lock-free skip list algo (default)
* LBSL= herlihy : compile with Herlihy's lock-bazed lazy skip list algo (default)
        pugh    : compile with Pugh's lock-based lazy  skip list algo
* SET_CPU=0 : does not pin threads to cores (does this by default) 
* POWER=1 : enable power measurements with rapl_read library (default is 0)             
* RO_FAIL=0 : disables read-only unsuccessful updates in linked lists (pugh, lazy, copy) 
          and hash tables (pug, lazy, copy, lea:java)
* STATS=1 : enable extra stats (about retries)
* PAD=1 : enabled node padding. NB. not all data structures support this properly for now
* SEQ_NO_FREE=1 : make the SEQ implementations NOT use the ssmem_free (but uses SSMEM)

ROOT = ../..

include $(ROOT)/common/Makefile.common


BIN_NAME := skiplist-fraser-qsense

BINS = $(BINDIR)/$(BIN_NAME)

PROF = $(ROOT)/src

.PHONY:	all clean

all:	main

ALGO_O = fraser.o
ALGO_C = fraser.c
measurements.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/measurements.o $(PROF)/measurements.c

ssalloc.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/ssalloc.o $(PROF)/ssalloc.c

skiplist.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/skiplist.o skiplist.c

sleeper_threads.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/sleeper_threads.o $(PROF)/sleeper_threads.c

murmurhash.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/MurmurHash2.o $(PROF)/MurmurHash2.c

bloom.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/bloom.o $(PROF)/bloom.c

double_llist.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/double_llist.o $(PROF)/double_llist.c

hybrid_optimized.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/hybrid_optimized.o $(PROF)/hybrid_optimized.c

$(ALGO_O): skiplist.h 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/$(ALGO_O) $(ALGO_C)

intset.o: skiplist.h fraser.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/intset.o intset.c

test.o: skiplist.h fraser.h intset.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test.o $(TEST_FILE)

main: measurements.o ssalloc.o skiplist.o $(ALGO_O) intset.o test.o double_llist.o hybrid_optimized.o sleeper_threads.o murmurhash.o bloom.o
	$(CC) $(CFLAGS) $(BUILDIR)/measurements.o $(BUILDIR)/ssalloc.o $(BUILDIR)/skiplist.o $(BUILDIR)/$(ALGO_O) $(BUILDIR)/intset.o $(BUILDIR)/test.o $(BUILDIR)/hybrid_optimized.o $(BUILDIR)/sleeper_threads.o $(BUILDIR)/MurmurHash2.o $(BUILDIR)/bloom.o $(BUILDIR)/double_llist.o -o $(BINS) $(LDFLAGS)

clean:
	-rm -f $(BINS)

ROOT = ../..

include $(ROOT)/common/Makefile.common

PROF = $(ROOT)/src

BIN_NAME := bst-aravind-qsense

BINS = $(BINDIR)/$(BIN_NAME)


.PHONY:	all clean

all:	main

measurements.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/measurements.o $(PROF)/measurements.c

ssalloc.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/ssalloc.o $(PROF)/ssalloc.c

bst-aravind.o: 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/bst-aravind.o bst-aravind.c

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

test.o: bst-aravind.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test.o $(TEST_FILE)

main: measurements.o ssalloc.o  bst-aravind.o test.o double_llist.o hybrid_optimized.o sleeper_threads.o murmurhash.o bloom.o
	$(CC) $(CFLAGS) $(BUILDIR)/measurements.o $(BUILDIR)/ssalloc.o $(BUILDIR)/bst-aravind.o  $(BUILDIR)/test.o $(BUILDIR)/hybrid_optimized.o $(BUILDIR)/sleeper_threads.o $(BUILDIR)/MurmurHash2.o $(BUILDIR)/bloom.o $(BUILDIR)/double_llist.o -o $(BINS) $(LDFLAGS)

clean:
	-rm -f $(BINS)

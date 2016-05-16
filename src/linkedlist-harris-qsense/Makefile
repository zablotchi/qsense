ROOT = ../..

include $(ROOT)/common/Makefile.common

BINS = $(BINDIR)/linkedlist-harris-qsense
PROF = $(ROOT)/src

.PHONY:	all clean

all:	main

LIST_IMPL = harris_opt

measurements.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/measurements.o $(PROF)/measurements.c

ssalloc.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/ssalloc.o $(PROF)/ssalloc.c

sleeper_threads.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/sleeper_threads.o $(PROF)/sleeper_threads.c

double_llist.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/double_llist.o $(PROF)/double_llist.c

hybrid_optimized.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/hybrid_optimized.o $(PROF)/hybrid_optimized.c

linkedlist.o: 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/linkedlist.o linkedlist.c

harris.o: linkedlist.h linkedlist.o
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/harris.o $(LIST_IMPL).c

intset.o: linkedlist.h harris.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/intset.o intset.c

test.o: linkedlist.h harris.h intset.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test.o $(TEST_FILE)

main: measurements.o ssalloc.o linkedlist.o harris.o harris.o intset.o test.o hybrid_optimized.o sleeper_threads.o double_llist.o $(TMILB)
	$(CC) $(CFLAGS) $(BUILDIR)/measurements.o $(BUILDIR)/ssalloc.o $(BUILDIR)/linkedlist.o $(BUILDIR)/harris.o $(BUILDIR)/intset.o $(BUILDIR)/test.o $(BUILDIR)/hybrid_optimized.o $(BUILDIR)/sleeper_threads.o $(BUILDIR)/double_llist.o -o $(BINS) $(LDFLAGS)

clean:
	-rm -f $(BINS)

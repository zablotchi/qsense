ROOT = ../..

include $(ROOT)/common/Makefile.common

BINS = $(BINDIR)/skiplist-fraser-qsbr
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

qsbr.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/qsbr.o $(PROF)/qsbr.c

$(ALGO_O): skiplist.h 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/$(ALGO_O) $(ALGO_C)

intset.o: skiplist.h fraser.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/intset.o intset.c

test.o: skiplist.h fraser.h intset.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test.o $(TEST_FILE)

main: measurements.o ssalloc.o skiplist.o $(ALGO_O) intset.o test.o qsbr.o
	$(CC) $(CFLAGS) $(BUILDIR)/measurements.o $(BUILDIR)/ssalloc.o $(BUILDIR)/skiplist.o $(BUILDIR)/$(ALGO_O) $(BUILDIR)/intset.o $(BUILDIR)/test.o $(BUILDIR)/qsbr.o -o $(BINS) $(LDFLAGS)

clean:
	-rm -f $(BINS)

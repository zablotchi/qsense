ROOT = ../..

include $(ROOT)/common/Makefile.common

BINS = $(BINDIR)/bst-aravind-hp
PROF = $(ROOT)/src

.PHONY:	all clean

all:	main

measurements.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/measurements.o $(PROF)/measurements.c

ssalloc.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/ssalloc.o $(PROF)/ssalloc.c

bst-aravind.o: 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/bst-aravind.o bst-aravind.c

double_llist.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/double_llist.o $(PROF)/double_llist.c

smr.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/smr.o $(PROF)/smr.c

test.o: bst-aravind.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test.o $(TEST_FILE)

main: measurements.o ssalloc.o  bst-aravind.o test.o double_llist.o smr.o
	$(CC) $(CFLAGS) $(BUILDIR)/measurements.o $(BUILDIR)/ssalloc.o $(BUILDIR)/bst-aravind.o  $(BUILDIR)/test.o $(BUILDIR)/smr.o $(BUILDIR)/double_llist.o -o $(BINS) $(LDFLAGS)

clean:
	-rm -f $(BINS)

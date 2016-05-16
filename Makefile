.PHONY:	all

BENCHS = src/bst-aravind src/bst-aravind-hp src/bst-aravind-qsbr src/bst-aravind-qsense src/linkedlist-harris src/linkedlist-harris-qsense src/linkedlist-harris-qsbr src/linkedlist-harris-hp src/skiplist-fraser src/skiplist-fraser-hp src/skiplist-fraser-qsbr src/skiplist-fraser-qsense

.PHONY: clean $(BENCHS)

all: linkedlist_mr bst_mr skiplist_mr

## BST 

bst_aravind_none:
	$(MAKE) "STM=LOCKFREE" "GC=0" src/bst-aravind

bst_aravind_qsbr:
	$(MAKE) "LOCK=TICKET" "GC=0" "INIT=one" src/bst-aravind-qsbr

bst_aravind_hp:
	$(MAKE) "STM=LOCKFREE" "GC=0" "INIT=one" src/bst-aravind-hp

bst_aravind_qsense:
	$(MAKE) "LOCK=TICKET" "GC=0" "INIT=one" src/bst-aravind-qsense


bst_mr: bst_aravind_none bst_aravind_qsbr bst_aravind_hp bst_aravind_qsense

## Skip list

sl_fraser_none:
	$(MAKE) "STM=LOCKFREE" "GC=0" src/skiplist-fraser

sl_fraser_qsbr:
	$(MAKE) "LOCK=TICKET" "GC=0" "INIT=one" src/skiplist-fraser-qsbr

sl_fraser_hp:
	$(MAKE) "STM=LOCKFREE" "GC=0" "INIT=one" src/skiplist-fraser-hp

sl_fraser_qsense:
	$(MAKE) "LOCK=TICKET" "GC=0" "INIT=one" src/skiplist-fraser-qsense	

skiplist_mr: sl_fraser_none sl_fraser_qsbr sl_fraser_hp  sl_fraser_qsense

## Linked list

ll_harris_none:
	$(MAKE) "GC=0" "INIT=one" src/linkedlist-harris

ll_harris_qsbr:
	$(MAKE) "LOCK=TICKET" "GC=0" "INIT=one" src/linkedlist-harris-qsbr

ll_harris_hp:
	$(MAKE) "STM=LOCKFREE" "GC=0" "INIT=one" src/linkedlist-harris-hp	

ll_harris_qsense:
	$(MAKE) "GC=0" "LOCK=TICKET" "INIT=one" src/linkedlist-harris-qsense

linkedlist_mr: ll_harris_none ll_harris_qsbr ll_harris_hp ll_harris_qsense


clean:
	rm -rf build
	rm -rf bin/*

$(BENCHS):
	$(MAKE) -C $@ $(TARGET)




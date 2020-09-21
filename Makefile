CC=gcc
CFLAGS= -std=c99 -Wall -Wextra -Wpedantic -m32
PUBL=publications
HASH=HashtableRBT
HASHS=Hashtable
Q=Queue
LL=LinkedList
RBT=rb_tree
HEAP=heap

.PHONY: build clean

build: $(PUBL)_unlinked.o $(HASH).o $(LL).o $(Q).o $(HEAP).o $(RBT).o $(HASHS).o
	ld -m elf_i386 -r $(PUBL)_unlinked.o $(HASH).o $(LL).o $(Q).o  $(HEAP).o $(RBT).o  $(HASHS).o -o $(PUBL).o

$(PUBL)_unlinked.o: $(PUBL).c $(PUBL).h
	$(CC) $(CFLAGS) $(PUBL).c -c -o $(PUBL)_unlinked.o

$(HASH).o: $(HASH).c $(HASH).h
	$(CC) $(CFLAGS) $(HASH).c  -c -o $(HASH).o
$(HASHS).o: $(HASHS).c $(HASHS).h
	$(CC) $(CFLAGS) $(HASHS).c  -c -o $(HASHS).o
$(Q).o: $(Q).c $(Q).h
	$(CC) $(CFLAGS) $(Q).c -c -o $(Q).o
$(LL).o: $(LL).c $(LL).h
	$(CC) $(CFLAGS) $(LL).c -c -o $(LL).o
$(HEAP).o: $(HEAP).c $(HEAP).h
	$(CC) $(CFLAGS) $(HEAP).c -c -o $(HEAP).o

$(RBT).o: $(RBT).c $(RBT).h 
	$(CC) $(CFLAGS) $(RBT).c -c -o $(RBT).o
clean:
	rm -f *.o *.h.gch

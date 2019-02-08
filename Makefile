all: multiThreadSorter

multiThreadSorter: multiThreadSorter_thread.c multiThreadSorter_thread.h mergesort.o
	gcc -pthread multiThreadSorter_thread.c mergesort.o -o multiThreadSorter
mergesort.o: mergesort.c
	gcc -c mergesort.c
clean:
	rm -rf multiThreadSorter mergesort.o

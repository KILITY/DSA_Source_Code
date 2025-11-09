#ifndef DSA_SOURCE_CODE_HEAP_H
#define DSA_SOURCE_CODE_HEAP_H

struct Heap {
    int data[200001]; //we have k <= 2*10^5 which means that at most we can have 200000 elements in the heap
    int size;
};

void heap_init(Heap *h);

void heap_push(Heap *h, int value);

int heap_top(Heap *h);

void heap_pop(Heap *h);

int heap_size(Heap *h);

void heapify(Heap *h, int i); //helper to restore heap property on given index

void build_heap(Heap *h, int arr[], int n); //to build a heap from an existing array

#endif
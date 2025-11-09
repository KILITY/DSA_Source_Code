#include "/Users/killity/Documents/DSA_Source_Code/Heaps/Powers-Of-Two-C-implementation/heap.h"

// Restore heap property for subtree rooted at index i
void heapify(Heap *h, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < h->size && h->data[left] > h->data[largest])
        largest = left;

    if (right < h->size && h->data[right] > h->data[largest])
        largest = right;

    if (largest != i) {
        int temp = h->data[i];
        h->data[i] = h->data[largest];
        h->data[largest] = temp;
        heapify(h, largest);
    }
}

void heap_init(Heap *h) {
    h->size = 0;
}

void heap_push(Heap *h, int value) {
    int i = h->size++;
    h->data[i] = value;

    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->data[parent] >= h->data[i]) break;

        /*  we use an array to represent the heap (binary tree)
            so
            parent(i) = (i - 1) / 2
            left(i)   = 2 * i + 1
            right(i)  = 2 * i + 2
        */

        int temp = h->data[parent];
        h->data[parent] = h->data[i];
        h->data[i] = temp;

        i = parent;
    }
}

int heap_top(Heap *h) {
    return (h->size > 0) ? h->data[0] : -1;
}

void heap_pop(Heap *h) {
    if (h->size == 0) return;

    h->data[0] = h->data[--h->size];
    heapify(h, 0);
}

int heap_size(Heap *h) {
    return h->size;
}

void build_heap(Heap *h, int arr[], int n) {
    for (int i = 0; i < n; i++)
        h->data[i] = arr[i];
    h->size = n;

    for (int i = (n / 2) - 1; i >= 0; i--)
        heapify(h, i);
}
#include <iostream>
#include "/Users/killity/Documents/DSA_Source_Code/Heaps/Powers-Of-Two-C-implementation/heap.h"
using namespace std;

// Problem link: https://codeforces.com/problemset/problem/1095/C
// Idea: represent n as sum of powers of two; if we need more parts,
// split the largest power until we reach k parts or cannot split further.
// literally the same code as C++ STL priority queue implementation
// works on example cases can't test because it is made using multiple files

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long n;
    int k;
    cin >> n >> k;

    Heap pq;
    heap_init(&pq);

    for (int i = 0; i < 63; ++i) {
        if (n & (1LL << i))
            heap_push(&pq, 1 << i);
    }

    if (heap_size(&pq) > k) {
        cout << "NO\n";
        return 0;
    }

    while (heap_size(&pq) < k) {
        int largest = heap_top(&pq);
        heap_pop(&pq);

        if (largest == 1) {
            cout << "NO\n";
            return 0;
        }

        heap_push(&pq, largest / 2);
        heap_push(&pq, largest / 2);
    }

    cout << "YES\n";
    while (heap_size(&pq) > 0) {
        cout << heap_top(&pq) << " ";
        heap_pop(&pq);
    }
    cout << "\n";

    return 0;
}
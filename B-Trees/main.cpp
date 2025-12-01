// Credits to this video for explanations: https://www.youtube.com/watch?v=K1a2Bk8NrYQ&t=9s

#include <bits/stdc++.h>

#include "Implementation.h"

int main() {
    // Constraints:
    // Minimum degree t
    // Node keys range: [t-1 .. 2t-1]
    int t = 3;
    BTree tree(t);

    int array[] = {10, 20, 5, 6, 12, 30, 7, 17};
    int n = sizeof(array) / sizeof(array[0]); //number of elements in array

    for (int i = 0; i < n; i++)
        tree.insert(array[i]);

    std::cout << "B-Tree traversal (sorted): ";
    tree.traverse();
    std::cout << "\n";

    // Search test - we can search for whatever we want
    int k1 = 6;
    std::cout << "Search " << k1 << ": "
              << (tree.search(k1) ? "FOUND" : "NOT FOUND") << "\n";

    int k2 = 15;
    std::cout << "Search " << k2 << ": "
              << (tree.search(k2) ? "FOUND" : "NOT FOUND") << "\n";

    return 0;
}
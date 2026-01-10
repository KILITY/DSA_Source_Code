#include <string>
#include <sstream>
#include <limits>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "Implementation.h"

static std::string validateNode(
    BTreeNode* node,
    bool isRoot,
    int t,
    long long minExclusive,
    long long maxExclusive,
    int depth,
    int& leafDepth
) {
    if (!node) {
        return "INVALID: null node pointer";
    }

    int numKeys = (int)node->keys.size();
    int numChildren = (int)node->children.size();

    // 1. node degree consistency
    if (node->t != t) {
        return "INVALID: node->t differs from tree->t";
    }

    // 2. key count bounds
    if (numKeys > 2 * t - 1) {
        return "INVALID: node has more than 2t-1 keys";
    }

    if (!isRoot && numKeys < t - 1) {
        return "INVALID: non-root node has fewer than t-1 keys";
    }

    if (isRoot && !node->leaf && numKeys == 0) {
        return "INVALID: root internal node has 0 keys";
    }

    // 3. leaf / children rules
    if (node->leaf) {
        if (numChildren != 0) {
            return "INVALID: leaf node has children";
        }

        if (leafDepth == -1)
            leafDepth = depth;
        else if (leafDepth != depth)
            return "INVALID: leaves are not all at same depth";

    } else {
        if (numChildren != numKeys + 1) {
            return "INVALID: internal node children != keys + 1";
        }

        for (BTreeNode* c : node->children) {
            if (!c) {
                return "INVALID: internal node has null child";
            }
        }
    }

    // 4. keys strictly increasing + interval check
    for (int i = 0; i < numKeys; i++) {
        if (i > 0 && node->keys[i - 1] >= node->keys[i]) {
            return "INVALID: keys not strictly increasing";
        }

        long long key = node->keys[i];
        if (!(key > minExclusive && key < maxExclusive)) {
            return "INVALID: key violates parent interval constraint";
        }
    }

    // 5. recurse children with correct intervals
    if (!node->leaf) {
        // first child
        {
            std::string r = validateNode(
                node->children[0],
                false,
                t,
                minExclusive,
                node->keys[0],
                depth + 1,
                leafDepth
            );
            if (r != "VALID") return r;
        }

        // middle children
        for (int i = 1; i < numKeys; i++) {
            std::string r = validateNode(
                node->children[i],
                false,
                t,
                node->keys[i - 1],
                node->keys[i],
                depth + 1,
                leafDepth
            );
            if (r != "VALID") return r;
        }

        // last child
        {
            std::string r = validateNode(
                node->children[numKeys],
                false,
                t,
                node->keys[numKeys - 1],
                maxExclusive,
                depth + 1,
                leafDepth
            );
            if (r != "VALID") return r;
        }
    }

    return "VALID";
}

std::string validateBTree(BTree& tree) {
    if (tree.t < 2) {
        return "INVALID: t must be >= 2";
    }

    if (!tree.root) {
        return "VALID"; // empty tree
    }

    int leafDepth = -1;

    return validateNode(
        tree.root,
        true,
        tree.t,
        std::numeric_limits<long long>::min(),
        std::numeric_limits<long long>::max(),
        0,
        leafDepth
    );
}

static void DBG_LINE(const char* phase, int i, int key, int t, int n, unsigned seed) {
    std::cout
        << "[BTREE-TEST] "
        << "phase=" << phase
        << " i=" << i
        << " key=" << key
        << " t=" << t
        << " n=" << n
        << " seed=" << seed
        << std::endl;
} //Printing to see each operation

//I am using a seed randomized test to generate random numbers such that the test mimics real world scenarios better, the tests are reproducible since using the same seed gives the same test
std::string runBTreeGeneratedTest(int t, int n, unsigned seed) {
    if (t < 2) return "FAIL: t must be >= 2";
    if (n < 0) return "FAIL: n must be >= 0";

    std::cout << "[BTREE-TEST] START t=" << t << " n=" << n << " seed=" << seed << std::endl;

    BTree tree(t);
    std::mt19937 rng(seed);

    int poolSize = std::max(1, 4 * n);
    std::vector<int> pool(poolSize);
    for (int i = 0; i < poolSize; i++) pool[i] = i + 1;
    std::shuffle(pool.begin(), pool.end(), rng);

    std::vector<int> inserted;
    inserted.reserve(n);

    // INSERT phase
    for (int i = 0; i < n; i++) {
        int key = pool[i];

        DBG_LINE("INSERT:before", i, key, t, n, seed);
        tree.insert(key);
        inserted.push_back(key);
        DBG_LINE("INSERT:after", i, key, t, n, seed);

        DBG_LINE("VALIDATE:afterInsert:before", i, key, t, n, seed);
        std::string v = validateBTree(tree);
        DBG_LINE("VALIDATE:afterInsert:after", i, key, t, n, seed);

        if (v != "VALID") {
            std::ostringstream oss;
            oss << "FAIL: validator failed after INSERT"
                << " | i=" << i
                << " | key=" << key
                << " | t=" << t
                << " | n=" << n
                << " | seed=" << seed
                << " | validator=\"" << v << "\"";
            return oss.str();
        }
    }

    std::shuffle(inserted.begin(), inserted.end(), rng);

    int delCount = n / 2;

    // DELETE half phase
    for (int i = 0; i < delCount; i++) {
        int key = inserted[i];

        DBG_LINE("DELETE_HALF:before", i, key, t, n, seed);
        tree.remove(key);
        DBG_LINE("DELETE_HALF:after", i, key, t, n, seed);

        DBG_LINE("VALIDATE:afterDeleteHalf:before", i, key, t, n, seed);
        std::string v = validateBTree(tree);
        DBG_LINE("VALIDATE:afterDeleteHalf:after", i, key, t, n, seed);

        if (v != "VALID") {
            std::ostringstream oss;
            oss << "FAIL: validator failed after DELETE (half phase)"
                << " | i=" << i
                << " | key=" << key
                << " | deleted=" << (i + 1) << "/" << delCount
                << " | t=" << t
                << " | n=" << n
                << " | seed=" << seed
                << " | validator=\"" << v << "\"";
            return oss.str();
        }
    }

    // CLEAR phase (delete remaining)
    for (int i = delCount; i < n; i++) {
        int clearIndex = i - delCount;

        std::cout << "[BTREE-TEST] CLEAR:iter-start i=" << i
                  << " clearIndex=" << clearIndex
                  << " insertedSize=" << inserted.size()
                  << std::endl;

        if (i < 0 || i >= (int)inserted.size()) {
            std::ostringstream oss;
            oss << "FAIL: inserted index out of bounds"
                << " | i=" << i
                << " | insertedSize=" << inserted.size()
                << " | t=" << t
                << " | n=" << n
                << " | seed=" << seed;
            return oss.str();
        }

        int key = inserted[i];

        DBG_LINE("CLEAR:before", clearIndex, key, t, n, seed);
        tree.remove(key);
        DBG_LINE("CLEAR:after", clearIndex, key, t, n, seed);

        DBG_LINE("VALIDATE:afterClearDelete:before", clearIndex, key, t, n, seed);
        std::string v = validateBTree(tree);
        DBG_LINE("VALIDATE:afterClearDelete:after", clearIndex, key, t, n, seed);

        if (v != "VALID") {
            std::ostringstream oss;
            oss << "FAIL: validator failed during CLEAR phase"
                << " | i=" << clearIndex
                << " | key=" << key
                << " | cleared=" << (clearIndex + 1) << "/" << (n - delCount)
                << " | t=" << t
                << " | n=" << n
                << " | seed=" << seed
                << " | validator=\"" << v << "\"";
            return oss.str();
        }
    }

    std::cout << "[BTREE-TEST] PASS t=" << t << " n=" << n << " seed=" << seed << std::endl;
    return "PASS";
}

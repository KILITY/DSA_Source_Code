#ifndef B_TREESS___UNIT_TEST_VALIDATOR_H
#define B_TREESS___UNIT_TEST_VALIDATOR_H

#include <string>

struct BTree;
struct BTreeNode;

std::string validateBTree(BTree& tree);

std::string runBTreeGeneratedTest(int t, int n, unsigned seed = 123456789u);

#endif
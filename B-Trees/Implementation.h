#ifndef DSA_SOURCE_CODE_IMPLEMENTATION_H
#define DSA_SOURCE_CODE_IMPLEMENTATION_H

#include <vector>

struct BTreeNode {
    bool leaf;
    int t;                     // minimum degree (defines node capacity)
    std::vector<int> keys;     // can have multiple keys unlike bst
    std::vector<BTreeNode*> children;

    BTreeNode(int _t, bool _leaf);
    ~BTreeNode();

    BTreeNode* search(int k);
    void traverse();               // output in sorted order
    void insertNonFull(int k);     // insert when node is NON-full
    void splitChild(int i, BTreeNode* y); // split full child - the last two functions specific to B-Trees

    //All of these Just for Delete VVV
    void remove(int k);
    int findKey(int k);

    int getPred(int idx);
    int getSucc(int idx);

    void removeFromLeaf(int idx);
    void removeFromNonLeaf(int idx);

    void fill(int idx);
    void borrowFromPrev(int idx);
    void borrowFromNext(int idx);
    void merge(int idx);
};

struct BTree {
    BTreeNode *root;
    int t;

    BTree(int _t);
    ~BTree();

    void traverse();
    BTreeNode* search(int k);
    void insert(int k);
    void remove(int k);
};

#endif
#include "Implementation.h"
#include <bits/stdc++.h> //I have windows NOW :)

BTreeNode::BTreeNode(int _t, bool _leaf) {    //constructor
    t = _t;
    leaf = _leaf;
    keys.reserve(2 * t - 1);       // max keys
    children.reserve(2 * t);       // max children
}

BTreeNode::~BTreeNode() {       //destructor
    for (auto c : children)
        delete c;
}

BTreeNode* BTreeNode::search(int k) {
    int i = 0;

    // Find first key >= k
    while (i < (int)keys.size() && k > keys[i])
        i++;

    // Key found in this node
    if (i < (int)keys.size() && keys[i] == k)
        return this;

    if (leaf) //didn't find and is leaf so we stop
        return nullptr;

    return children[i]->search(k); //go to the childrean of the correct key
}

void BTreeNode::insertNonFull(int k) {
    int i = (int)keys.size() - 1;

    if (leaf) {
        // Insert key in sorted order inside leaf
        keys.push_back(0);  // temporary space
        while (i >= 0 && keys[i] > k) {
            keys[i + 1] = keys[i];
            i--;
        }
        keys[i + 1] = k;
    } else {
        // Find child to descend into
        while (i >= 0 && keys[i] > k)
            i--;
        i++; //we go 1 after the child we need so we take a step back

        // If child is full -> split it
        if ((int)children[i]->keys.size() == 2 * t - 1) {
            splitChild(i, children[i]);
            if (keys[i] < k)
                i++;
        }
        children[i]->insertNonFull(k);
    }
}

void BTreeNode::splitChild(int i, BTreeNode* y) {  //This is the core of the BTree it basically splits a node when it is full and moves up the middle key
    // y is full so it has 2t-1 keys. Create new node z.
    BTreeNode* z = new BTreeNode(y->t, y->leaf);

    // Move last (t-1) keys of y to z
    for (int j = 0; j < t - 1; j++)
        z->keys.push_back(y->keys[j + t]);

    // If y is not leaf, move t children to z
    if (!y->leaf) {
        for (int j = 0; j < t; j++)
            z->children.push_back(y->children[j + t]);
    }

    // Memory management
    y->keys.resize(t - 1);
    if (!y->leaf)
        y->children.resize(t);

    // Insert new child into parent node
    children.insert(children.begin() + i + 1, z);

    // Insert midle key of y into this node
    keys.insert(keys.begin() + i, y->keys[t - 1]);
}

BTree::BTree(int _t) {
    root = nullptr;
    t = _t;
}

BTree::~BTree() {
    delete root;
}

BTreeNode* BTree::search(int k) {
    return (root == nullptr) ? nullptr : root->search(k);
}

void BTree::insert(int k) {
    if (!root) { //no root -> we create tree can't have tree without root
        root = new BTreeNode(t, true);
        root->keys.push_back(k);
        return;
    }

    // If root is full, it must be split, btrees can have max 2t-1 keys
    if ((int)root->keys.size() == 2 * t - 1) {
        BTreeNode* newRoot = new BTreeNode(t, false);

        newRoot->children.push_back(root);

        // Split old root
        newRoot->splitChild(0, root);

        // Insert into correct subtreee
        int i = 0;
        if (newRoot->keys[0] < k)
            i++;
        newRoot->children[i]->insertNonFull(k);

        root = newRoot;
    } else {
        root->insertNonFull(k);
    }
}

void BTreeNode::traverse() {
    int i;
    for (i = 0; i < (int)keys.size(); i++) {
        if (!leaf)
            children[i]->traverse();
        std::cout << keys[i] << " ";
    }

    if (!leaf)
        children[i]->traverse();
}

void BTree::traverse() {
    if (root)
        root->traverse();
}
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

    int midKey = y->keys[t - 1];

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

    keys.insert(keys.begin() + i, midKey);
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

void BTree::traverse() {
    if (root)
        root->traverse();
}

//143 lines of code for everything else vs 130 just for deletion :)

int BTreeNode::findKey(int k) {
    int idx = 0;
    while (idx < (int)keys.size() && keys[idx] < k)
        ++idx;
    return idx;
}

void BTree::remove(int k) {
    if (!root) return;

    root->remove(k);

    if ((int)root->keys.size() == 0) {
        BTreeNode* tmp = root;
        if (root->leaf)
            root = nullptr;
        else
            root = root->children[0];
        tmp->children.clear();
        delete tmp;
    }
}

void BTreeNode::remove(int k) {
    int idx = findKey(k);

    // Case 1: key found in this node
    if (idx < (int)keys.size() && keys[idx] == k) {
        if (leaf)
            removeFromLeaf(idx);
        else
            removeFromNonLeaf(idx);
    }
    else {
        if (leaf) return; // Key not found

        bool flag = (idx == (int)keys.size());
        if ((int)children[idx]->keys.size() < t)
            fill(idx);

        if (flag && idx > (int)keys.size())
            children[idx - 1]->remove(k);
        else
            children[idx]->remove(k);
    }
}

void BTreeNode::removeFromLeaf(int idx) {  // Simplest
    keys.erase(keys.begin() + idx);
}

void BTreeNode::removeFromNonLeaf(int idx) {
    int k = keys[idx];

    // Case 2A: predecessor child >= t keys
    if ((int)children[idx]->keys.size() >= t) {
        int pred = getPred(idx);
        keys[idx] = pred;
        children[idx]->remove(pred);
    }

    // Case 2B: successor child >= t keys
    else if ((int)children[idx + 1]->keys.size() >= t) {
        int succ = getSucc(idx);
        keys[idx] = succ;
        children[idx + 1]->remove(succ);
    }

    // Case 2C: both children have t-1 keys -> merge
    else {
        merge(idx);
        children[idx]->remove(k);
    }
}

int BTreeNode::getPred(int idx) {
    BTreeNode* cur = children[idx];
    while (!cur->leaf)
        cur = cur->children[(int)cur->keys.size()];
    return cur->keys[(int)cur->keys.size() - 1];
}

int BTreeNode::getSucc(int idx) {
    BTreeNode* cur = children[idx + 1];
    while (!cur->leaf)
        cur = cur->children[0];
    return cur->keys[0];
}

void BTreeNode::fill(int idx) {
    if (idx != 0 && (int)children[idx - 1]->keys.size() >= t)
        borrowFromPrev(idx);
    else if (idx != (int)keys.size() && (int)children[idx + 1]->keys.size() >= t)
        borrowFromNext(idx);
    else {
        if (idx != (int)keys.size())
            merge(idx);
        else
            merge(idx - 1);
    }
}

void BTreeNode::borrowFromPrev(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx - 1];

    child->keys.insert(child->keys.begin(), keys[idx - 1]);
    keys[idx - 1] = sibling->keys.back();
    sibling->keys.pop_back();

    if (!child->leaf) {
        child->children.insert(child->children.begin(), sibling->children.back());
        sibling->children.pop_back();
    }
}

void BTreeNode::borrowFromNext(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];

    child->keys.push_back(keys[idx]);
    keys[idx] = sibling->keys[0];
    sibling->keys.erase(sibling->keys.begin());

    if (!child->leaf) {
        child->children.push_back(sibling->children[0]);
        sibling->children.erase(sibling->children.begin());
    }
}

void BTreeNode::merge(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];

    child->keys.push_back(keys[idx]);

    for (int i = 0; i < (int)sibling->keys.size(); i++)
        child->keys.push_back(sibling->keys[i]);

    if (!child->leaf) {
        for (int i = 0; i < (int)sibling->children.size(); i++)
            child->children.push_back(sibling->children[i]);
        sibling->children.clear();
    }

    keys.erase(keys.begin() + idx);
    children.erase(children.begin() + idx + 1);

    delete sibling;
}
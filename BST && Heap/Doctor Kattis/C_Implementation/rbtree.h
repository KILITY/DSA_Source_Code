#ifndef C_IMPLEMENTATION_RBTREE_H
#define C_IMPLEMENTATION_RBTREE_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct Patient Patient; // forward declaration for comparison

    typedef struct RBNode {
        Patient* data;
        struct RBNode* parent;
        struct RBNode* left;
        struct RBNode* right;
        int color; // 0 = black, 1 = red
    } RBNode;

    typedef struct RBTree {
        RBNode* root;
        int (*compare)(const Patient*, const Patient*);
    } RBTree;

    RBTree* rbtree_create(int (*cmp)(const Patient*, const Patient*));
    void rbtree_insert(RBTree* tree, Patient* data);
    void rbtree_delete(RBTree* tree, Patient* data);
    Patient* rbtree_min(RBTree* tree);
    int rbtree_empty(RBTree* tree);
    void rbtree_free(RBTree* tree);

#ifdef __cplusplus
}
#endif

#endif
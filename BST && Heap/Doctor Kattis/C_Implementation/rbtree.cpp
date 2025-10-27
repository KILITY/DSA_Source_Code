/* =====================================================================
 * RED-BLACK TREE IMPLEMENTATION
 * =====================================================================

 * KEY PROPERTIES OF RED-BLACK TREES:
 * 1. Every node is either RED or BLACK
 * 2. The root is always BLACK
 * 3. Every leaf (NULL) is considered BLACK
 * 4. If a node is RED, both its children must be BLACK (no two RED nodes in a row)
 * 5. Every path from a node to any of its descendant leaves has the same
 *    number of BLACK nodes (black height property)
 *
 * These properties ensure the tree height is at most 2*log(n), guaranteeing
 * O(log n) performance for search, insert, and delete operations.
 * ===================================================================== */

#include "rbtree.h"
#include <stdlib.h>

#define RED   1
#define BLACK 0


static RBNode* new_node(Patient* data, int color, RBNode* parent) {
    RBNode* n = (RBNode*)malloc(sizeof(RBNode));
    n->data = data;
    n->color = color;
    n->parent = parent;
    n->left = n->right = NULL;
    return n;
}

/* =====================================================================
 * rotate_left: Performs a left rotation around node x
 * =====================================================================
 * A rotation is a local restructuring operation that maintains the
 * binary search tree property while changing the tree structure.
 *
 * LEFT ROTATION visualization:
 *        x                   y
 *       / \                 / \
 *      A   y      =>       x   C
 *         / \             / \
 *        B   C           A   B
 *
 * After left rotation:
 * - y moves up to replace x's position
 * - x becomes y's left child
 * - y's left subtree (B) becomes x's right subtree
 * - All parent pointers are updated
 * ===================================================================== */

static void rotate_left(RBTree* tree, RBNode* x) {
    RBNode* y = x->right;  // y is x's right child (y > x)
    x->right = y->left;    // y's left child becomes x's right child
    if (y->left)
        y->left->parent = x;
    y->parent = x->parent;  //inherit the parent of x
    if (!x->parent)
        tree->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

/* =====================================================================
 * rotate_right: Performs a right rotation around node x
 * =====================================================================
 * RIGHT ROTATION visualization (opposite of left rotation):
 *        x                 y
 *       / \               / \
 *      y   C    =>       A   x
 *     / \                   / \
 *    A   B                 B   C
 *
 * After right rotation:
 * - y moves up to replace x's position
 * - x becomes y's right child
 * - y's right subtree (B) becomes x's left subtree
 * - All parent pointers are updated
 * ===================================================================== */

static void rotate_right(RBTree* tree, RBNode* x) {
    RBNode* y = x->left;   // y is x's left child (y < x)
    x->left = y->right;    // y's right child becomes x's left child
    if (y->right)
        y->right->parent = x;
    y->parent = x->parent;
    if (!x->parent)
        tree->root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

/* =====================================================================
 * fix_insert: Restores red-black tree properties after insertion
 * =====================================================================
 * When a new node is inserted, it's colored RED. This might violate the
 * RBT property: "No two RED nodes in a row"
 *
 * This function walks up the tree from the newly inserted node (z),
 * fixing any violations it encounters.
 *
 * TWO MAIN CASES:
 *
 * CASE 1: Node z's uncle is RED
 *   - Recolor z's parent, uncle, and grandparent
 *   - Move the violation up the tree (z = grandparent)
 *
 * CASE 2: Node z's uncle is BLACK
 *   - Perform rotations to restructure the tree
 *   - May need to do two rotations if z is the "wrong" child
 *   - After rotation, recolor nodes to restore properties
 * ===================================================================== */
static void fix_insert(RBTree* tree, RBNode* z) {

    while (z->parent && z->parent->color == RED) {
        RBNode* g = z->parent->parent;
        if (!g) break;

        // CASE: z's parent is the LEFT child of the grandparent
        if (z->parent == g->left) {
            RBNode* y = g->right;

            // SUBCASE 1: Uncle is RED - Just recolor
            if (y && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                g->color = RED;
                z = g;
            }
            // SUBCASE 2: Uncle is BLACK - Need rotations
            else {
                // If z is the RIGHT child of its parent, need left rotation first
                if (z == z->parent->right) {
                    z = z->parent;
                    rotate_left(tree, z);
                }
                // Now z is the LEFT child of its parent
                z->parent->color = BLACK;
                g->color = RED;
                rotate_right(tree, g);
            }
        }
        // CASE: z's parent is the RIGHT child of the grandparent - the same but backwards
        else {
            RBNode* y = g->left;

            // SUBCASE 1: Uncle is RED - Just recolor
            if (y && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                g->color = RED;
                z = g;
            }
            // SUBCASE 2: Uncle is BLACK - Need rotations
            else {
                // If z is the LEFT child of its parent, need right rotation first
                if (z == z->parent->left) {
                    z = z->parent;
                    rotate_right(tree, z);
                }
                // Now z is the RIGHT child of its parent
                z->parent->color = BLACK;
                g->color = RED;
                rotate_left(tree, g);
            }
        }
    }
    tree->root->color = BLACK;  // ROOT IS ALWAYS BLACK
}

/* =====================================================================
 * rbtree_create: Creates and initializes an empty red-black tree
 * =====================================================================
 * Parameters:
 *   cmp - Function pointer to a comparison function for ordering Patient data
 *         Should return: negative if first < second
 *                        zero if first == second
 *                        positive if first > second
 * ===================================================================== */
RBTree* rbtree_create(int (*cmp)(const Patient*, const Patient*)) {
    RBTree* t = (RBTree*)malloc(sizeof(RBTree));
    t->root = NULL;
    t->compare = cmp;
    return t;
}

/* =====================================================================
 * rbtree_insert: Inserts a new Patient node into the red-black tree
 * =====================================================================
 * Time Complexity: O(log n) on average due to tree balancing
 *
 * Note: New nodes are always colored RED to minimize the number of
 *       property violations that need fixing.
 * ===================================================================== */
void rbtree_insert(RBTree* tree, Patient* data) {
    RBNode* y = NULL;  // y will track the parent of the new node
    RBNode* x = tree->root;  // Start from the root

    while (x) {
        y = x;
        int c = tree->compare(data, x->data);
        if (c < 0)
            x = x->left;
        else
            x = x->right;
    }

    RBNode* z = new_node(data, RED, y);
    if (!y)
        tree->root = z;
    else if (tree->compare(data, y->data) < 0)
        y->left = z;
    else
        y->right = z;

    fix_insert(tree, z);
}

static RBNode* tree_min(RBNode* node) {
    while (node && node->left)
        node = node->left;
    return node;
}


Patient* rbtree_min(RBTree* tree) {
    RBNode* m = tree_min(tree->root);
    return m ? m->data : NULL; //Returns Data instead of Node
}

int rbtree_empty(RBTree* tree) {
    return tree->root == NULL;
}

static void transplant(RBTree* tree, RBNode* u, RBNode* v) {
    if (!u->parent)
        tree->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    if (v)
        v->parent = u->parent;
}

/* =====================================================================
 * fix_delete: Restores red-black tree properties after deletion
 * =====================================================================
 * Deletion is more complex than insertion in RBTs because removing a
 * BLACK node can violate the "black height" property.
 *
 * CONCEPT - DOUBLE BLACK NODE:
 * When we remove a BLACK node and replace it with another node (or NULL),
 * we conceptually mark that position as having two BLACK colors (a "double
 * black").
 *
 * This function walks up the tree from position x, fixing violations by:
 * 1. If x's sibling (w) is RED: Convert to a case where sibling is BLACK
 * 2. If w and w's children are BLACK: Move the double-black up to parent
 * 3. If w is BLACK but has at least one RED child: Rotate and recolor
 * ===================================================================== */
static void fix_delete(RBTree* tree, RBNode* x) {
    while (x != tree->root && (!x || x->color == BLACK)) {
        RBNode* p = x ? x->parent : NULL;
        if (!p) break;

        // CASE: x is the LEFT child of its parent
        if (x == p->left) {
            RBNode* w = p->right; // w is x's sibling

            // CASE 1: Sibling w is RED
            if (w && w->color == RED) {
                w->color = BLACK;
                p->color = RED;
                rotate_left(tree, p);
                w = p->right;
            }

            // CASE 2: Sibling w is BLACK, and both w's children are BLACK
            if ((!w->left || w->left->color == BLACK) &&
                (!w->right || w->right->color == BLACK)) {
                if (w) w->color = RED;
                x = p;
            }
            // CASE 3: Sibling w is BLACK, w has at least one RED child
            else {
                // SUBCASE 3a: w's right child is BLACK (only left is RED)
                if (!w->right || w->right->color == BLACK) {
                    if (w->left) w->left->color = BLACK;
                    w->color = RED;
                    rotate_right(tree, w);
                    w = p->right;
                }
                // SUBCASE 3b: w's right child is RED
                if (w) w->color = p->color;
                p->color = BLACK;
                if (w && w->right) w->right->color = BLACK;
                rotate_left(tree, p);
                x = tree->root;
            }
        }
        // CASE: x is the RIGHT child of its parent - as above but backwards
        else {
            RBNode* w = p->left;

            // CASE 1: Sibling w is RED
            if (w && w->color == RED) {
                w->color = BLACK;
                p->color = RED;
                rotate_right(tree, p);
                w = p->left;
            }

            // CASE 2: Sibling w is BLACK, and both w's children are BLACK
            if ((!w->right || w->right->color == BLACK) &&
                (!w->left || w->left->color == BLACK)) {
                if (w) w->color = RED;
                x = p;
            }
            // CASE 3: Sibling w is BLACK, w has at least one RED child
            else {
                // SUBCASE 3a: w's left child is BLACK (only right is RED)
                if (!w->left || w->left->color == BLACK) {
                    if (w->right) w->right->color = BLACK;
                    w->color = RED;
                    rotate_left(tree, w);
                    w = p->left;
                }
                // SUBCASE 3b: w's left child is RED
                if (w) w->color = p->color;
                p->color = BLACK;
                if (w && w->left) w->left->color = BLACK;
                rotate_right(tree, p);
                x = tree->root;
            }
        }
    }
    if (x) x->color = BLACK;
}

/* =====================================================================
 * rbtree_delete: Removes a node from the red-black tree
 * =====================================================================
 * This is the most complex RBT operation. The deletion process:
 *
 * 1. FIND: Locate the node to delete by exact match using comparison function
 * 2. REMOVE: Use standard BST deletion logic
 *    - If node has no left child: replace with right child
 *    - If node has no right child: replace with left child
 *    - If node has both children: replace with in-order successor
 *      (smallest node in right subtree) and delete that successor
 * 3. FIX: If a BLACK node was removed, fix the tree properties
 *
 * Time Complexity: O(log n)
 * ===================================================================== */
void rbtree_delete(RBTree* tree, Patient* data) {
    RBNode* z = tree->root;
    while (z) {
        if (z->data == data) break;
        int c = tree->compare(data, z->data);
        if (c < 0) z = z->left; else z = z->right;
    }
    if (!z) return;

    RBNode* y = z;  //actual node we remove
    int y_original_color = y->color;
    RBNode* x = NULL; // node that replaces y

    if (!z->left) {
        x = z->right;
        transplant(tree, z, z->right);
    }
    else if (!z->right) {
        x = z->left;
        transplant(tree, z, z->left);
    }
    else {
        y = tree_min(z->right);
        y_original_color = y->color;
        x = y->right;

        if (y->parent == z) {
            // Special case: y is z's direct right child
            if (x) x->parent = y;
        } else {
            // General case: y is deeper in z's right subtree
            transplant(tree, y, y->right);
            y->right = z->right;
            if (y->right) y->right->parent = y;
        }

        transplant(tree, z, y);
        y->left = z->left;
        if (y->left) y->left->parent = y;
        y->color = z->color;
    }

    if (y_original_color == BLACK)
        fix_delete(tree, x);

    free(z);
}

static void free_nodes(RBNode* n) {
    if (!n) return;
    free_nodes(n->left);
    free_nodes(n->right);
    free(n);
}

void rbtree_free(RBTree* tree) {
    if (tree) {
        free_nodes(tree->root);  // Free all nodes
        free(tree);              // Free the tree structure itself
    }
}
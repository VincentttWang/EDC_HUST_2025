#include "mode_tree.h"
#include "stdio.h"
#include "AllHeader.h"

// Static memory pool for tree nodes
static ModeTree tree_pool[MAX_TREE_NODES];
static bool pool_initialized = false;

void initModeTreePool(void) {
    for (int i = 0; i < MAX_TREE_NODES; i++) {
        tree_pool[i].is_used = false;
        tree_pool[i].firstChild = NULL;
        tree_pool[i].nextSibling = NULL;
        tree_pool[i].parent = NULL;
    }
    pool_initialized = true;
}

ModeTree *createModeTree(ModeNode node) {
    if (!pool_initialized) {
        initModeTreePool();
    }
    
    // Find an unused node in the pool
    for (int i = 0; i < MAX_TREE_NODES; i++) {
        if (!tree_pool[i].is_used) {
            tree_pool[i].nodes = node;
            tree_pool[i].firstChild = NULL;
            tree_pool[i].nextSibling = NULL;
            tree_pool[i].parent = NULL;
            tree_pool[i].is_used = true;
            return &tree_pool[i];
        }
    }
    
    // No free nodes available
    #ifdef INITIALIZE_H
    sprintf(error_message, "No free tree nodes available");
    #endif
    error_handler();
    return NULL;
}

void addChild(ModeTree *parent, ModeTree *child) {
    if (parent == NULL || child == NULL) {
        error_handler(); // Handle error for null pointers
        return; // Invalid input
    }
    child->parent = parent; // Set the parent pointer
    if (parent->firstChild == NULL) {
        parent->firstChild = child; // Set as first child if none exists
    } else {
        ModeTree *sibling = parent->firstChild;
        while (sibling->nextSibling != NULL) {
            sibling = sibling->nextSibling; // Traverse to the last sibling
        }
        sibling->nextSibling = child; // Add as next sibling
    }
}

ModeTree *getFirstChild(ModeTree *node) {
    if (node == NULL) {
        return NULL; // Return NULL if the node is NULL
    }
    return node->firstChild; // Return the first child node
}
ModeTree *getNextSibling(ModeTree *node) {
    if (node == NULL) {
        return NULL; // Return NULL if the node is NULL
    }
    return node->nextSibling; // Return the next sibling node
}

#include "mode_tree.h"
#include "stdio.h"
#include "main.h"
#include "stdlib.h"

ModeTree *createModeTree(ModeNode node) {
    ModeTree *newTree = (ModeTree *)malloc(sizeof(ModeTree));
    if (newTree == NULL) {
        return NULL; // Memory allocation failed
    }
    newTree->nodes = node;
    newTree->firstChild = NULL;
    newTree->nextSibling = NULL;
    return newTree;
}

void addChild(ModeTree *parent, ModeTree *child) {
    if (parent == NULL || child == NULL) {
        Error_Handler(); // Handle error for null pointers
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

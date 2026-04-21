/*
 * Author: Qinghan Yang
 * Date: 2025-07-29
 * Mode Tree Header
 * This header file defines the data structures and functions for managing the mode tree.
 */

#ifndef MODE_TREE_H
#define MODE_TREE_H

#include <stddef.h>
#include <stdbool.h>

#define MAX_TREE_NODES 32  // Maximum number of tree nodes

typedef struct{
    void (*mode_function)(void); // Function pointer for the mode function
    char *mode_name; // Name of the mode
} ModeNode;

typedef struct ModeTree {
    ModeNode nodes; 
    struct ModeTree *firstChild; // Pointer to the first child node
    struct ModeTree *nextSibling; // Pointer to the next sibling node
    struct ModeTree *parent; // Pointer to the parent node, if needed
    bool is_used; // Flag to indicate if this node is in use
} ModeTree;

// Static memory management
void initModeTreePool(void); // Initialize the static pool
ModeTree *createModeTree(ModeNode node); // Function to create a new mode tree
void addChild(ModeTree *parent, ModeTree *child); // Function to add a child to a parent node

ModeTree *getFirstChild(ModeTree *node); // Function to get the first child of a node
ModeTree *getNextSibling(ModeTree *node); // Function to get the next sibling of a node

#endif // MODE_TREE_H
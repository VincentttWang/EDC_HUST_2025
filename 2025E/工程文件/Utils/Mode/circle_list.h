#ifndef CIRCLE_LIST_H
#define CIRCLE_LIST_H

#include "mode_tree.h"
#include <stdbool.h>

#define MAX_CIRCLE_LIST_NODES 16  // Maximum number of circular list nodes

typedef struct CircleListNode {
    ModeTree *data; // Data stored in the node
    struct CircleListNode *next; // Pointer to the next node in the list
    bool is_used; // Flag to indicate if this node is in use
} CircleListNode;

typedef struct {
    CircleListNode *head; // Pointer to the head of the list
    CircleListNode *tail; // Pointer to the tail of the list
} CircleList;

void initCircleListPool(void); // Initialize the static pool
void CircleList_Init(CircleList *list);
int CircleList_Insert(CircleList *list, ModeTree *data); // Returns 0 on success, -1 on failure
void CircleList_Delete(CircleList *list, ModeTree *data);
void CircleList_Display(CircleList *list);
void CircleList_Clear(CircleList *list); // Clear all nodes from the list

#endif // CIRCLE_LIST_H
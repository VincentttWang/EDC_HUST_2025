#ifndef CIRCLE_LIST_H
#define CIRCLE_LIST_H

#include "mode_tree.h"

typedef struct CircleListNode {
    ModeTree *data; // Data stored in the node
    struct CircleListNode *next; // Pointer to the next node in the list
} CircleListNode;

typedef struct {
    CircleListNode *head; // Pointer to the head of the list
    CircleListNode *tail; // Pointer to the tail of the list
} CircleList;

void CircleList_Init(CircleList *list);
void CircleList_Insert(CircleList *list, ModeTree *data);
void CircleList_Delete(CircleList *list, ModeTree *data);
void CircleList_Display(CircleList *list);
void CircleList_Free(CircleList *list);

#endif // CIRCLE_LIST_H
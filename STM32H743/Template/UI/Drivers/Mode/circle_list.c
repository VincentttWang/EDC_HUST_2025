#include "circle_list.h"
#include "stdlib.h"

void CircleList_Init(CircleList *list) {
    list->head = NULL; // Initialize head to NULL
    list->tail = NULL; // Initialize tail to NULL
}

void CircleList_Insert(CircleList *list, ModeTree *data) {
    CircleListNode *new_node = (CircleListNode *)malloc(sizeof(CircleListNode));
    if (new_node == NULL) return; // Handle memory allocation failure
    new_node->data = data;
    if (list->head == NULL) {
        // If the list is empty, initialize head and tail
        list->head = new_node;
        list->tail = new_node;
        new_node->next = new_node; // Point to itself
    } else {
        // Insert the new node at the end and update tail
        list->tail->next = new_node;
        list->tail = new_node;
        new_node->next = list->head; // Make it circular
    }
}
void CircleList_Delete(CircleList *list, ModeTree *data) {
    if (list->head == NULL) return; // List is empty
    CircleListNode *current = list->head;
    CircleListNode *previous = list->tail;

    do {
        if (current->data == data) {
            if (current == list->head) {
                // If the node to delete is the head
                if (list->head == list->tail) {
                    // Only one node in the list
                    free(current);
                    list->head = NULL;
                    list->tail = NULL;
                } else {
                    // More than one node in the list
                    list->tail->next = current->next; // Update tail's next
                    list->head = current->next; // Move head to next node
                    free(current);
                }
            } else {
                // If the node to delete is not the head
                previous->next = current->next; // Bypass the current node
                if (current == list->tail) {
                    list->tail = previous; // Update tail if necessary
                }
                free(current);
            }
            return; // Node found and deleted
        }
        previous = current;
        current = current->next;
    } while (current != list->head); // Loop until we come back to head
}
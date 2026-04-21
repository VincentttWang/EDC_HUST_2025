#include "circle_list.h"
#include "AllHeader.h"

// Static memory pool for circular list nodes
static CircleListNode circle_pool[MAX_CIRCLE_LIST_NODES];
static bool circle_pool_initialized = false;

void initCircleListPool(void) {
    for (int i = 0; i < MAX_CIRCLE_LIST_NODES; i++) {
        circle_pool[i].is_used = false;
        circle_pool[i].data = NULL;
        circle_pool[i].next = NULL;
    }
    circle_pool_initialized = true;
}

static CircleListNode *allocateCircleListNode(void) {
    if (!circle_pool_initialized) {
        initCircleListPool();
    }
    
    for (int i = 0; i < MAX_CIRCLE_LIST_NODES; i++) {
        if (!circle_pool[i].is_used) {
            circle_pool[i].is_used = true;
            circle_pool[i].data = NULL;
            circle_pool[i].next = NULL;
            return &circle_pool[i];
        }
    }
    return NULL; // No free nodes available
}

static void freeCircleListNode(CircleListNode *node) {
    if (node >= circle_pool && node < circle_pool + MAX_CIRCLE_LIST_NODES) {
        node->is_used = false;
        node->data = NULL;
        node->next = NULL;
    }
}

void CircleList_Init(CircleList *list) {
    list->head = NULL; // Initialize head to NULL
    list->tail = NULL; // Initialize tail to NULL
}

int CircleList_Insert(CircleList *list, ModeTree *data) {
    CircleListNode *new_node = allocateCircleListNode();
    if (new_node == NULL) {
        #ifdef INITIALIZE_H
        sprintf(error_message, "No free circle list nodes available");
        #endif
        error_handler();
        return -1; // No free nodes available
    }
    
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
    return 0; // Success
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
                    freeCircleListNode(current);
                    list->head = NULL;
                    list->tail = NULL;
                } else {
                    // More than one node in the list
                    list->tail->next = current->next; // Update tail's next
                    list->head = current->next; // Move head to next node
                    freeCircleListNode(current);
                }
            } else {
                // If the node to delete is not the head
                previous->next = current->next; // Bypass the current node
                if (current == list->tail) {
                    list->tail = previous; // Update tail if necessary
                }
                freeCircleListNode(current);
            }
            return; // Node found and deleted
        }
        previous = current;
        current = current->next;
    } while (current != list->head); // Loop until we come back to head
}

void CircleList_Display(CircleList *list) {
    // Implementation for display if needed
}

void CircleList_Clear(CircleList *list) {
    if (list->head == NULL) return; // List is already empty
    
    CircleListNode *current = list->head;
    CircleListNode *next;
    
    do {
        next = current->next;
        freeCircleListNode(current);
        current = next;
    } while (current != list->head);
    
    list->head = NULL;
    list->tail = NULL;
}
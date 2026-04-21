#ifndef MENU_H
#define MENU_H

#include "mode_tree.h"
#include "oled.h"
#include "stdbool.h"

#define END_X 128 // End X coordinate for the OLED display
#define TEST_MENU

// Function prototypes for menu operations
void menu_init(void);
void menu_display(ModeTree *modeTree);
void menu_function(void);
void select_menu(ModeTree *modeTree);

void menu_begin(void);

bool is_menu_node(ModeTree *node); // Check if the node is a menu node


#endif // MENU_H
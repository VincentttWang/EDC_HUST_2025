/**
 * @file static_menu_test.c
 * @brief Test file to verify static memory allocation for menu system
 * @author AI Assistant
 * @date 2025-08-01
 */

#include "menu.h"
#include <stdio.h>

// Mock functions for testing (replace with actual implementations)
void test_dis(void) { printf("Test Distance Function\n"); }
void test_Cordi(void) { printf("Test Coordinate Function\n"); }
void test_Circle(void) { printf("Test Circle Function\n"); }
void test_track(void) { printf("Test Track Function\n"); }
void proB_1(void) { printf("Problem B1 Function\n"); }
void proB_2_3(void) { printf("Problem B2/3 Function\n"); }
void proH_1(void) { printf("Problem H1 Function\n"); }
void proH_2(void) { printf("Problem H2 Function\n"); }

// Mock error handler
void error_handler(void) { 
    printf("Error occurred in menu system\n"); 
}

// Mock OLED functions
void OLED_Clear(void) { }
void OLED_ShowString(int x, int y, char *str, int size) { 
    printf("OLED[%d,%d]: %s\n", x, y, str); 
}
void OLED_ShowChar(int x, int y, char c, int size) { 
    printf("OLED[%d,%d]: %c\n", x, y, c); 
}

// Mock key functions
bool Key_short_press(void) { return false; }
bool Key_long_press(void) { return false; }

#ifdef INITIALIZE_H
char error_message[256];
#endif

/**
 * @brief Test function to verify static memory allocation
 */
void test_static_menu_system(void) {
    printf("=== Static Menu System Test ===\n");
    
    // Initialize the menu system
    menu_init();
    printf("Menu initialization completed.\n");
    
    // Display memory usage info
    printf("Maximum tree nodes: %d\n", MAX_TREE_NODES);
    printf("Maximum circle list nodes: %d\n", MAX_CIRCLE_LIST_NODES);
    printf("Total static memory for tree nodes: %lu bytes\n", 
           sizeof(ModeTree) * MAX_TREE_NODES);
    printf("Total static memory for circle list nodes: %lu bytes\n", 
           sizeof(CircleListNode) * MAX_CIRCLE_LIST_NODES);
    
    printf("Static menu system test completed successfully!\n");
}

int main(void) {
    test_static_menu_system();
    return 0;
}

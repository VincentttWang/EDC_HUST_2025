#include "menu.h"
#include "AllHeader.h"
/**
  ******************************************************************************
  * @author  Qinghan Yang
  * @date    2025-07-29
  * @file    menu.c
  * @brief   This file contains the implementation of the menu system for the
  *          peripheral devices UI drivers.
  ******************************************************************************
  * @attention
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

static ModeTree *now_mode_tree = NULL; // Pointer to the current mode tree
char CircleNum = '0'; // Variable to hold the current circle number

// Static string buffers for menu node names to avoid using string literals
static char circle_name_1[] = "1";
static char circle_name_2[] = "2";
static char circle_name_3[] = "3";
static char circle_name_4[] = "4";
static char circle_name_5[] = "5";

// Static menu list for menu navigation
static CircleList static_menu_list;


void menu_init(void)
{// Initialize the menu system
    // Initialize static memory pools
    initModeTreePool();
    initCircleListPool();
    
    ModeNode rootNode = {NULL, "Root"}; // Create the root node
    now_mode_tree = createModeTree(rootNode); // Create the mode tree with the root node
    if (now_mode_tree == NULL) {
        #ifdef INITIALIZE_H
        sprintf(error_message, "Failed to create mode tree");
        #endif
        error_handler(); // Handle error if tree creation fails
    }
    now_mode_tree->parent = NULL; // Set the parent of the root node to NULL 
    OLED_Clear(); // Clear the OLED display
    now_mode_tree->nodes.mode_function = menu_function; // Set the function pointer for the root node
    now_mode_tree->nodes.mode_name = "Main Menu"; // Set the name for the root node
    // Add more nodes to the mode tree as needed
    now_mode_tree->parent = NULL; // Set the parent of the root node to NULL 
    OLED_Clear(); // Clear the OLED display
    
    // 构建测试菜单树结构
    // 创建主菜单的子菜单项
    ModeNode TestNode = {menu_function, "Test Menu"}; // Create a test menu node
    ModeNode ProBNode = {menu_function, "Problem B"}; // Create a problem menu node
    ModeNode ProHNode = {menu_function, "Problem H"}; // Create a custom menu node

    ModeTree *testMenu = createModeTree(TestNode); // Create the test menu tree
    ModeTree *problemBMenu = createModeTree(ProBNode); // Create the problem menu tree
    ModeTree *proHMenu = createModeTree(ProHNode); // Create the custom menu tree

    addChild(now_mode_tree, testMenu);
    addChild(now_mode_tree, problemBMenu);
    addChild(now_mode_tree, proHMenu); // Add the problem menu to the main menu

    // Add child nodes to the test menu
    ModeNode test_dis_mode = {test_dis, "Test Distance"}; // Create a test distance node
    ModeNode test_cordi_mode = {test_Cordi, "Test Coordinate"}; // Create a test coordinate node
    ModeNode test_circle_mode = {test_Circle, "Test Circle"}; // Create a test circle node
    ModeNode test_track_mode = {test_track, "Test Track"}; // Create a test track node
    ModeNode test_connect_mode = {test_Connect, "Test Connect"}; // Create a test connect node

    // Add the test nodes to the test menu
    addChild(testMenu, createModeTree(test_connect_mode)); // Add the connect test node

    addChild(testMenu, createModeTree(test_dis_mode));
    addChild(testMenu, createModeTree(test_cordi_mode));
    addChild(testMenu, createModeTree(test_circle_mode));
    addChild(testMenu, createModeTree(test_track_mode));

    ModeNode ProB1menu = {menu_function, "ProB1"}; // Create a sub-menu node for Problem B
    ModeNode ProB2_3menu = {proB_2_3, "ProB2/3"}; // Create a sub-menu node for Problem B

    ModeNode ProH1menu = {menu_function, "ProH1"}; // Create a sub-menu node for Problem H
    ModeNode ProH2menu = {proH_2, "ProH2"};

    ModeTree *ProBMenu1 = createModeTree(ProB1menu);
    ModeTree *ProBMenu2_3 = createModeTree(ProB2_3menu);

    ModeTree *ProHMenu1 = createModeTree(ProH1menu);
    ModeTree *ProHMenu2 = createModeTree(ProH2menu);

    addChild(problemBMenu, ProBMenu1);//有下一级菜单
    addChild(problemBMenu, ProBMenu2_3);//无下一级菜单
    addChild(proHMenu, ProHMenu1);//有下一级菜单
    addChild(proHMenu, ProHMenu2);//无下一级菜单

    // Use static string buffers instead of dynamic allocation
    char *circle_names[] = {circle_name_1, circle_name_2, circle_name_3, circle_name_4, circle_name_5};
    
    for(int i = 0; i < 5; i++)
    {
        ModeNode CircleNode = {proB_1, circle_names[i]};
        ModeTree *circleMenu = createModeTree(CircleNode); // Create a circle menu node
        addChild(ProBMenu1, circleMenu); // Add the circle menu to the Problem B menu
        if (circleMenu == NULL) {
            #ifdef INITIALIZE_H
            sprintf(error_message, "Failed to create circle menu %d", i + 1);
            #endif
            error_handler(); // Handle error if circle menu creation fails
        }
    }

    for(int i = 0; i < 2; i++)
    {
        ModeNode CircleNode = {proH_1, circle_names[i]};
        ModeTree *circleMenu = createModeTree(CircleNode); // Create a circle menu node
        addChild(ProHMenu1, circleMenu); // Add the circle menu to the Problem H menu
        if (circleMenu == NULL) {
            #ifdef INITIALIZE_H
            sprintf(error_message, "Failed to create circle menu %d", i + 1);
            #endif
            error_handler(); // Handle error if circle menu creation fails
        }
    }
    return;
}

void menu_begin(void)
{
    now_mode_tree->nodes.mode_function(); // Set the function pointer for the current mode tree
}
void menu_function(void)
{
    // Implement the menu function here
    OLED_Clear(); // Clear the OLED display
    CircleList_Clear(&static_menu_list); // Clear the static menu list
    CircleList_Init(&static_menu_list); // Initialize the static menu list

    if(now_mode_tree == NULL)
    {
        OLED_ShowString(0, 0, "Menu not initialized", 8); // Show error message if menu is not initialized
        return;
    }
    if(now_mode_tree->parent == NULL && now_mode_tree->firstChild == NULL)
    {
        OLED_ShowString(0, 0, "Empty Menu", 8); // Show message if the menu is empty
        return;
    }
    if(now_mode_tree->parent != NULL)
    {
        if (CircleList_Insert(&static_menu_list, now_mode_tree->parent) != 0) {
            OLED_ShowString(0, 0, "Menu list full", 8);
            return;
        }
    }
    ModeTree *child = getFirstChild(now_mode_tree);
    while (child != NULL) {
        if (CircleList_Insert(&static_menu_list, child) != 0) {
            OLED_ShowString(0, 0, "Menu list full", 8);
            return;
        }
        child = getNextSibling(child); // Move to the next sibling
    }
    CircleListNode *current = static_menu_list.head; // Start from the head of the circular list
    int index = 0; // Index for displaying menu items
    do{
        if (current->data != NULL && current->data->nodes.mode_name != NULL) {
            if(current->data == now_mode_tree->parent)
            {
                OLED_ShowString(0, index, "..", 8); // Show parent node with ".."
            }
            else
            {
                OLED_ShowString(0, index, current->data->nodes.mode_name, 8); // Display the mode name on the OLED
            }
            index++;
        }
        current = current->next; // Move to the next node in the circular list
    } while (current != static_menu_list.head); // Loop until we come back to the head
    int total = index; // Total number of menu items
    index = 0; // Reset index for displaying the select arrow
    OLED_ShowChar(END_X - 8, index, '<', 8); // Show a select arrow at the end of the menu
    while(1)
    {
        // Implement menu navigation and selection logic here
        if(Key_long_press()) 
        {
            // If Key1 is pressed, select the current menu item
            CircleNum = current->data->nodes.mode_name[0]; // Store the selected menu item name
            select_menu(current->data);
            break; // Exit the menu function after selection
        }
        if(Key_short_press())
        {
            // If Key2 is pressed, navigate to the next menu item
            current = current->next;
            OLED_ShowChar(END_X - 8, index, ' ', 8); // Clear the previous select arrow
            index = (index + 1) % total; // Move to the next item
            OLED_ShowChar(END_X - 8, index, '<', 8); // Show the select arrow at the new position
        }
    }
}

void select_menu(ModeTree *menu)
{
    if (menu == NULL) return;
    OLED_Clear(); // Clear the OLED display
    now_mode_tree = menu; // Set the current mode tree to the selected menu
    now_mode_tree->nodes.mode_function(); // Call the function associated with the selected menu
}

bool is_menu_node(ModeTree *node)
{
    // Check if the node is a menu node by checking if it has a function pointer
    return (node != NULL && node->nodes.mode_function != NULL) && 
           (node->nodes.mode_function == menu_function);
}
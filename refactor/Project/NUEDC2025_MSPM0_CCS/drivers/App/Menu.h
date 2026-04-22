/**
 * @file  Menu.h
 * @brief OLED 菜单系统接口，提供菜单显示与按键交互
 */
#ifndef MENU_H
#define MENU_H

#include "ModeTree.h"
#include "CircleList.h"
#include "Oled.h"
#include <stdbool.h>
#include "project_build_config.h"

#define END_X 128

void menu_init(void);
void menu_function(void);
void select_menu(ModeTree *modeTree);
void menu_begin(void);
bool is_menu_node(ModeTree *node);

#endif /* MENU_H */

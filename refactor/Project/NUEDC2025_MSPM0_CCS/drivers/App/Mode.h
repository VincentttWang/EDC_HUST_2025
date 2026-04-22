/**
 * @file  Mode.h
 * @brief 应用模式接口，定义各题目模式与测试模式的入口函数
 */
#ifndef MODE_H
#define MODE_H

#include <stdbool.h>
#include "project_build_config.h"

#if PROJECT_ENABLE_TEST_MODES
void mode_test_distance(void);
void mode_test_coordinate(void);
void mode_test_circle(void);
void mode_test_connection(void);
void mode_test_tracking(void);
#endif

void mode_problem_b_1(void);
void mode_problem_b_2_3(void);
void mode_problem_h_1(void);
void mode_problem_h_2(void);

int mode_set_circle_num(char num);
bool mode_turn_step(void);
bool mode_init_guard(void);

#endif /* MODE_H */

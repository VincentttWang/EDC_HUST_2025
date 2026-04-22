#ifndef MODE_H
#define MODE_H

#include <stdbool.h>
#include "project_build_config.h"

#if PROJECT_ENABLE_TEST_MODES
void test_dis(void);
void test_Cordi(void);
void test_Circle(void);
void test_Connect(void);
void test_track(void);
#endif

void proB_1(void);
void proB_2_3(void);
void proH_1(void);
void proH_2(void);

int SetCircleNum(char num);
bool turn_func(void);

bool Init(void);

#endif
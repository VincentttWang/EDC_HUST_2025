/**
 * @file  Rotation.h
 * @brief 旋转矩阵运算接口，提供矩阵构建、乘法、转置及欧拉角提取
 */
#ifndef ROTATION_H
#define ROTATION_H

#include "Kinematics.h"

void rotation_matrix(RotationAngles angles, float matrix[3][3]);
void matrix_multiplication(float mat1[3][3], float mat2[3][3], float result[3][3]);
void matrix_transpose(float matrix[3][3], float result[3][3]);
void matrix_to_angles(float matrix[3][3], RotationAngles *angles);

#endif /* ROTATION_H */
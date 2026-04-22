#ifndef ROTATION_H
#define ROTATION_H

#include "AllHeader.h"

void rotation_matrix(RotationAngles angles, float matrix[3][3]);
void matrix_multiplication(float mat1[3][3], float mat2[3][3], float result[3][3]);
void matrix_transpose(float matrix[3][3], float result[3][3]);
void matrix_to_angles(float matrix[3][3], RotationAngles *angles);

#endif /* ROTATION_H */
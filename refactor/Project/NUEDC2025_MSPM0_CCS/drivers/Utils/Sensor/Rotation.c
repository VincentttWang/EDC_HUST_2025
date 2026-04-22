#include "Rotation.h"

/*
 * Rotation.c
 * Description: Source file for rotation matrix functions
 */

void rotation_matrix(RotationAngles angles, float matrix[3][3])
{
    // Calculate the rotation matrix based on yaw, pitch, and roll angles
    float cy = cosf(DEG_TO_RAD(angles.yaw));
    float sy = sinf(DEG_TO_RAD(angles.yaw));
    float cp = cosf(DEG_TO_RAD(angles.pitch));
    float sp = sinf(DEG_TO_RAD(angles.pitch));
    float cr = cosf(DEG_TO_RAD(angles.roll));
    float sr = sinf(DEG_TO_RAD(angles.roll));

    matrix[0][0] = cy * cp;
    matrix[0][1] = cy * sp * sr - sy * cr;
    matrix[0][2] = cy * sp * cr + sy * sr;

    matrix[1][0] = sy * cp;
    matrix[1][1] = sy * sp * sr + cy * cr;
    matrix[1][2] = sy * sp * cr - cy * sr;

    matrix[2][0] = -sp;
    matrix[2][1] = cp * sr;
    matrix[2][2] = cp * cr;
}

void matrix_multiplication(float mat1[3][3], float mat2[3][3], float result[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            result[i][j] = 0;
            for (int k = 0; k < 3; k++)
            {
                result[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
}

void matrix_transpose(float matrix[3][3], float result[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            result[j][i] = matrix[i][j];
        }
    }
}

void matrix_to_angles(float matrix[3][3], RotationAngles *angles)
{
    angles->yaw = RAD_TO_DEG(atan2f(matrix[1][0], matrix[0][0]));
    angles->pitch = RAD_TO_DEG(atan2f(-matrix[2][0], sqrtf(matrix[2][1] * matrix[2][1] + matrix[2][2] * matrix[2][2])));
    angles->roll = RAD_TO_DEG(atan2f(matrix[2][1], matrix[2][2]));
}

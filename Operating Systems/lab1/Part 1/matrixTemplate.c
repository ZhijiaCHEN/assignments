#define idx1 IDX1
#define idx2 IDX2
#define idx3 IDX3
#include "matrixTemplate.h"

void matrix_multiplication__CNT__(double **A, double **B, double **C, unsigned int N)
{
    //print_matrix(C);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < N; k++)
            {
                C[idx1][idx2] += A[idx1][idx3] * B[idx3][idx2];
                //print_matrix(C);
            }
        }
    }
    return;
}
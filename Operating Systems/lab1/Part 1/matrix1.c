#define idx1 i
#define idx2 k
#define idx3 j
#include "matrix1.h"

void matrix_multiplication1(double **A, double **B, double **C, unsigned int N)
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
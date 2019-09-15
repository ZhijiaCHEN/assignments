#define idx1 k
#define idx2 i
#define idx3 j
#include "matrix4.h"

void matrix_multiplication4(double **A, double **B, double **C, unsigned int N)
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
#define idx1 k
#define idx2 j
#define idx3 i
#include "matrix5.h"

void matrix_multiplication5(double A[N][N], double B[N][N], double C[N][N])
{
    //print_matrix(C);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < N; k++)
            {
                C[idx1][idx2] += A[idx1][idx3]*B[idx3][idx2];
                //print_matrix(C);
            }
        }
    }
    return;
}
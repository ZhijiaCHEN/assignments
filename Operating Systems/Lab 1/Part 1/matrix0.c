#define idx1 i
#define idx2 j
#define idx3 k
#include <string.h>
#include "matrix0.h"

void matrix_multiplication0(double A[N][N], double B[N][N], double C[N][N])
{
    memset(C, 0, sizeof(double[N][N]));
    for (int idx1 = 0; idx1 < N; idx1++)
        for (int idx2 = 0; idx2 < N; idx2++)
            for (int idx3 = 0; idx3 < N; idx3++)
                C[idx1][idx2] += A[idx1][idx3]*B[idx3][idx2];
    return;
}
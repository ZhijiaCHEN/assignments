#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "main.h"
#include "matrix.h"

const unsigned int N = 1000;
const unsigned int M = 10000;

void insert_sort(int array[], unsigned int size, int new);

int main(int argc, char const *argv[])
{
    //part 1
    double A[N][N];
    double B[N][N];
    double C[N][N];
    char order[6][3] = {{'i', 'j', 'k'}, {'i', 'k', 'j'}, {'j', 'i', 'k'}, {'j', 'k', 'i'}, {'k', 'i', 'j'}, {'k', 'j', 'i'}};
    void (*funArray[6])(double A[N][N], double B[N][N], double C[N][N]) = {matrix_multiplication0, matrix_multiplication1, matrix_multiplication2, matrix_multiplication3, matrix_multiplication4, matrix_multiplication5};
    clock_t start, end;
    unsigned long cpu_time_used;
    srand(time(0));
    for (int r = 0; r < N; r++)
    {
        for (int c = 0; c < N; ++c)
        {
            A[r][c] = (double)rand();
            B[r][c] = (double)rand();
        }
    }
    FILE *fp = fopen("matrix.log", "a");
    fprintf(fp, "#matrix_size ijk_order elapsed_time\n");
    for (int i = 0; i < 6; ++i)
    {
        start = clock();
        funArray[i](A, B, C);
        end = clock();
        cpu_time_used = (end - start) * 1000 / CLOCKS_PER_SEC;
        fprintf(fp, "%d %s,%s,%s %d\n", N, order[i][0], order[i][1], order[i][2], cpu_time_used);
    }

    //part 2
    int unsorted[M] ={0};
    int sorted1[M], sorted2[M];
    for (unsigned int i = 0; i < M; ++i)
        unsorted[i] = rand();
    return 0;
}

void insert_sort(int array[], unsigned int size, int new)
{
    for (unsigned int i = size; i > 0; --i)
    {
        if (new < array[i-1])
        {
            array[i] = array[i-1];
        }
        else
        {
            array[i] = new;
            break;
        }
    }
    if (new < array[0])
        array[0] = new;
}


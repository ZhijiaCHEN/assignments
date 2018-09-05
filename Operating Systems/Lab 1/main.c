#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "main.h"
#include "matrix.h"

const unsigned int N = 1000;
const unsigned int M = 10000;

void insert_sort(int array[], size_t size);

int main(int argc, char const *argv[])
{
    int runPart1 = 1, runPart2 = 0;
    int part1Repeat, part2Repeat = 3;
    int checkMatrix = 0;
    int checkSort = 0;
    
    //part 1
    if (runPart1 == 1)
    {
        /*
        double a[N][N] = {{1,2,3},{4,5,6},{7,8,9}};
        double b[N][N] = {{1,2,3},{4,5,6},{7,8,9}};
        double (*A)[N][N] = &a, (*B)[N][N] = &b;
        double (*C)[N][N] = calloc(N*N, sizeof(double));
        matrix_multiplication0(*A, *B, *C);
        */

        double (*A)[N][N] = calloc(N*N, sizeof(double));
        double (*B)[N][N] = calloc(N*N, sizeof(double));
        double (*C)[N][N] = calloc(N*N, sizeof(double));
        char order[6][3] = {{'i', 'j', 'k'}, {'i', 'k', 'j'}, {'j', 'i', 'k'}, {'j', 'k', 'i'}, {'k', 'i', 'j'}, {'k', 'j', 'i'}};
        void (*funArray[6])(double [N][N], double [N][N], double [N][N]) = {matrix_multiplication0, matrix_multiplication1, matrix_multiplication2, matrix_multiplication3, matrix_multiplication4, matrix_multiplication5};
        clock_t start, end;
        double cpu_time_used;
        srand(time(0));
        for (int r = 0; r < N; r++)
        {
            for (int c = 0; c < N; ++c)
            {
                (*A)[r][c] = (double)rand();
                (*B)[r][c] = (double)rand();
            }
        }

        if (checkMatrix == 1)
        {
            double (*D)[N][N] = calloc(N*N, sizeof(double));
            funArray[0](*A, *B, *D);
                for (int i = 1; i < 6; ++i)
            {
                funArray[i](*A, *B, *C);
                for(size_t r = 0; r < N; ++r)
                {
                    for(size_t c = 0; c < N; ++c)
                        assert((*C)[r][c] == (*D)[r][c]);
                }
            }
            free(D);
        }
        else
        {
            FILE *fp = fopen("matrix.log", "a");
            for (int i = 0; i < 6; ++i)
            {
                start = clock();
                funArray[i](*A, *B, *C);
                end = clock();
                cpu_time_used = (double)((end - start) * 1000) / CLOCKS_PER_SEC;
                fprintf(fp, "%d %c,%c,%c %f\n", N, order[i][0], order[i][1], order[i][2], cpu_time_used);
                memset(*C, 0, sizeof(double [N][N]));
            }
            fclose(fp);
        }

        free(A);
        free(B);
        free(C);
    }
    

    //part 2
    if (runPart2 == 1)
    {
        FILE *sortLog = fopen("sort.log", "a");
        unsigned long insertSortTime = 0, mergeSortTime = 0;
        clock_t start, end;
        for (int r = 0; r < part2Repeat; ++r)
        {
            int (*unsorted)[M] = calloc(sizeof(int), M);
            int (*unsortedHalf1)[M / 2] = calloc(sizeof(int), M / 2);
            int (*unsortedHalf2)[M - M / 2] = calloc(sizeof(int), M - M / 2);
            for (unsigned int i = 0; i < M; ++i)
                (*unsorted)[i] = rand();
            memcpy(*unsortedHalf1, *unsorted, (M / 2)*sizeof(int));
            memcpy(*unsortedHalf2, &((*unsorted)[M / 2]), (M - M / 2)*sizeof(int));
            start = clock();
            insert_sort(*unsorted, M);
            end = clock();
            insertSortTime += end - start;
            start = clock();
            insert_sort(*unsortedHalf1, M / 2);
            insert_sort(*unsortedHalf2, M - M / 2);
            end = clock();
            mergeSortTime += end - start;
            int *ptr1 = *unsortedHalf1, *ptr2 = *unsortedHalf2;
            if (checkSort == 1)
            {
                for (size_t i = 0; i < M; ++i)
                {
                    if ((*ptr1 < *ptr2) && (ptr1 < (*unsortedHalf1) + M / 2))
                    {
                        assert(*ptr1 == (*unsorted)[i]);
                        ++ptr1;
                    }
                    else
                    {
                        assert(*ptr2 == (*unsorted)[i]);
                        ++ptr2;
                    }
                }
            }
            else
            {
                FILE *fp;
                start = clock();
                fp = fopen("insertSort.log", "w");
                for (size_t i = 0; i < M; ++i)
                {
                    fprintf(fp, "%d\n", (*unsorted)[i]);
                }
                fclose(fp);
                end = clock();
                insertSortTime += end - start;

                start = clock();
                fp = fopen("mergeSort.log", "w");
                for (size_t i = 0; i < M; ++i)
                {
                    if((*ptr1 < *ptr2) && (ptr1 < (*unsortedHalf1) + M / 2))
                    {
                        fprintf(fp, "%d\n", *ptr1);
                        ++ptr1;
                    }
                    else
                    {
                        fprintf(fp, "%d\n", *ptr2);
                        ++ptr2;
                    }
                }
                fclose(fp);
                end = clock();
                mergeSortTime += end - start;
            }
        }
        fprintf(sortLog, "%d %f %f\n", M, (double)(insertSortTime * 1000) / (CLOCKS_PER_SEC*part2Repeat), (double)(mergeSortTime * 1000) / (CLOCKS_PER_SEC*part2Repeat));
        fclose(sortLog);
    }
    

    return 0;
}

void insert_sort(int array[], size_t size)
{
    if (size <= 1)
        return;
    for (size_t i = 1; i < size; ++i)
    {
        for (size_t j = 0; j < i; ++j)
        {
            if (array[i] < array[j])
            {
                int tmp = array[i];
                for (size_t k = i; k > j; --k)
                    array[k] = array[k - 1];
                array[j] = tmp;
                break;
            }
        }
    }
}

void print_matrix(double X[N][N])
{
    for (size_t r = 0; r < N; ++r)
    {
        for (size_t c = 0; c < N; ++c)
        {
            printf("%f ", X[r][c]);
        }
        printf("\n");
    }
    printf("\n");
}
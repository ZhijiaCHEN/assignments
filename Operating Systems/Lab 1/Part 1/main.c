#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "main.h"
#include "matrix.h"

const unsigned int N = 1000;

int main(int argc, char const *argv[])
{
    int check = 0, uid;
    double(*A)[N][N] = calloc(N * N, sizeof(double));
    double(*B)[N][N] = calloc(N * N, sizeof(double));
    double(*C)[N][N] = calloc(N * N, sizeof(double));
    char *hostname = calloc(100, sizeof(char));
    gethostname(hostname, 100);
    uid = geteuid();
    char order[6][3] = {{'i', 'j', 'k'}, {'i', 'k', 'j'}, {'j', 'i', 'k'}, {'j', 'k', 'i'}, {'k', 'i', 'j'}, {'k', 'j', 'i'}};
    char timeBuff[100];
    void (*funArray[6])(double[N][N], double[N][N], double[N][N]) = {matrix_multiplication0, matrix_multiplication1, matrix_multiplication2, matrix_multiplication3, matrix_multiplication4, matrix_multiplication5};
    clock_t start, end;
    time_t rawtime;
    struct tm *timeinfo;
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

    if (check == 1) //check result
    {
        double(*D)[N][N] = calloc(N * N, sizeof(double));
        funArray[0](*A, *B, *D);
        for (int i = 1; i < 6; ++i)
        {
            funArray[i](*A, *B, *C);
            for (size_t r = 0; r < N; ++r)
            {
                for (size_t c = 0; c < N; ++c)
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
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            strftime(timeBuff, 100, "%I:%M", timeinfo);
            fputs(timeBuff, fp);
            fprintf(fp, " %d %c,%c,%c %f %s %d\n", N, order[i][0], order[i][1], order[i][2], cpu_time_used, hostname, uid);
            memset(*C, 0, sizeof(double[N][N]));
        }
        fclose(fp);
    }

    free(A);
    free(B);
    free(C);
    free(hostname);
    return 0;
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
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "main.h"
#include "matrix.h"

int main(int argc, char const *argv[])
{
    int check = 0, uid;
    char *hostname = calloc(100, sizeof(char));
    gethostname(hostname, 100);
    uid = geteuid();
    struct passwd *pw;
    pw = getpwuid(getuid());
    char order[6][3] = {{'i', 'j', 'k'}, {'i', 'k', 'j'}, {'j', 'i', 'k'}, {'j', 'k', 'i'}, {'k', 'i', 'j'}, {'k', 'j', 'i'}};
    char timeBuff[100];
    clock_t start, end;
    time_t rawtime;
    struct tm *timeinfo;
    double cpu_time_used;
    void (*funArray[6])(double **, double **, double **, unsigned int N) = {matrix_multiplication0, matrix_multiplication1, matrix_multiplication2, matrix_multiplication3, matrix_multiplication4, matrix_multiplication5};

    for (unsigned int N = 100; N <= 1000; N += 100)
    {

        double **A = calloc(N, sizeof(double *));
        double **B = calloc(N, sizeof(double *));
        double **C = calloc(N, sizeof(double *));
        double **D = calloc(N, sizeof(double *));
        for (unsigned int i = 0; i < N; ++i)
        {
            A[i] = calloc(N, sizeof(double));
            B[i] = calloc(N, sizeof(double));
            C[i] = calloc(N, sizeof(double));
            D[i] = calloc(N, sizeof(double));
        }

        srand(time(0));
        for (int r = 0; r < N; r++)
        {
            for (int c = 0; c < N; ++c)
            {
                A[r][c] = (double)rand();
                B[r][c] = (double)rand();
            }
        }

        if (check == 1) //check result
        {
            funArray[0](A, B, D, N);
            for (int i = 1; i < 6; ++i)
            {
                funArray[i](A, B, C, N);
                for (size_t r = 0; r < N; ++r)
                {
                    for (size_t c = 0; c < N; ++c)
                    {
                        if (C[r][c] != D[r][c])
                        {
                            printf("error!\n");
                            getchar();
                        }
                    }
                }
                for (unsigned int i = 0; i < N; ++i)
                {
                    memset(C[i], 0, N * sizeof(double));
                }
            }
            printf("pass check.\n");
        }
        else
        {
            FILE *fp = fopen("matrix.log", "a");
            for (int i = 0; i < 6; ++i)
            {
                start = clock();
                funArray[i](A, B, C, N);
                end = clock();
                cpu_time_used = (double)((end - start) * 1000) / CLOCKS_PER_SEC;
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                strftime(timeBuff, 100, "%I:%M", timeinfo);
                fputs(timeBuff, fp);
                fprintf(fp, " %d %c,%c,%c %f %s %s\n", N, order[i][0], order[i][1], order[i][2], cpu_time_used, hostname, pw->pw_name);
                for (unsigned int i = 0; i < N; ++i)
                {
                    memset(C[i], 0, N * sizeof(double));
                }
            }
            fclose(fp);
        }
        for (unsigned int i = 0; i < N; ++i)
        {
            free(A[i]);
            free(B[i]);
            free(C[i]);
            free(D[i]);
        }
        free(A);
        free(B);
        free(C);
        free(D);
    }

    free(hostname);
    getchar();
    return 0;
}

void print_matrix(double **X, unsigned int N)
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
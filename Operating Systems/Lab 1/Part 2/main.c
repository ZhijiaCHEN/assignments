#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

const unsigned int N = 10000;

void insert_sort(int array[], size_t size);

int main(int argc, char const *argv[])
{
    int repeat = 3;
    int check = 0;

    FILE *sortLog = fopen("sort.log", "w");
    unsigned long insertSortTime = 0, mergeSortTime = 0;
    clock_t start, end;
    for (int r = 0; r < repeat; ++r)
    {
        int (*unsorted)[N] = calloc(sizeof(int), N);
        int (*unsortedHalf1)[N / 2] = calloc(sizeof(int), N / 2);
        int (*unsortedHalf2)[N - N / 2] = calloc(sizeof(int), N - N / 2);
        for (unsigned int i = 0; i < N; ++i)
            (*unsorted)[i] = rand();
        memcpy(*unsortedHalf1, *unsorted, (N / 2)*sizeof(int));
        memcpy(*unsortedHalf2, &((*unsorted)[N / 2]), (N - N / 2)*sizeof(int));
        start = clock();
        insert_sort(*unsorted, N);
        end = clock();
        insertSortTime += end - start;
        start = clock();
        insert_sort(*unsortedHalf1, N / 2);
        insert_sort(*unsortedHalf2, N - N / 2);
        end = clock();
        mergeSortTime += end - start;
        int *ptr1 = *unsortedHalf1, *ptr2 = *unsortedHalf2;
        if (check == 1)//check result
        {
            for (size_t i = 0; i < N; ++i)
            {
                if ((*ptr1 < *ptr2) && (ptr1 < (*unsortedHalf1) + N / 2))
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
            for (size_t i = 0; i < N; ++i)
            {
                fprintf(fp, "%d\n", (*unsorted)[i]);
            }
            fclose(fp);
            end = clock();
            insertSortTime += end - start;

            start = clock();
            fp = fopen("mergeSort.log", "w");
            for (size_t i = 0; i < N; ++i)
            {
                if((*ptr1 < *ptr2) && (ptr1 < (*unsortedHalf1) + N / 2))
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
    fprintf(sortLog, "%d %f %f\n", N, (double)(insertSortTime * 1000) / (CLOCKS_PER_SEC*repeat), (double)(mergeSortTime * 1000) / (CLOCKS_PER_SEC*repeat));
    fclose(sortLog);

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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "myshell/myshell.h"

typedef struct
{
    int* taskCnt;
    int *stopFlag;
    pthread_mutex_t *m;
    pthread_mutex_t *s;
    pthread_cond_t *c;
}dispatcherArgs;

void task_dispatcher(dispatcherArgs *arg);

int main()
{
    // ftok to generate unique key
    key_t key1 = ftok("shared memory for tuple space", 65), key2 = ftok("shared memory for output space", 65);
    pid_t pid, status;
    int taskSigPipe[2], resultSigPipe[2], taskCnt = 0, stopFlag = 0;
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t s = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t c = PTHREAD_COND_INITIALIZER;

    if (pipe(taskSigPipe) < 0)
    {
        printf("Pipe could not be initialized\n");
        exit(EXIT_FAILURE);
    }
    if (pipe(resultSigPipe) < 0)
    {
        close(taskSigPipe[0]);
        close(taskSigPipe[1]);
        printf("Pipe could not be initialized\n");
        exit(EXIT_FAILURE);
    }

    // shmget returns an identifier in shmid
    int shmid1 = shmget(key1, 1024, 0666 | IPC_CREAT);
    int shmid2 = shmget(key2, 1024, 0666 | IPC_CREAT);

    //start shell process
    pid = fork();
    if (pid == 0)
    {
        // shmat to attach to shared memory
        char *tupleSpace = (char *)shmat(shmid1, (void *)0, 0);
        char *outputSpace = (char *)shmat(shmid2, (void *)0, 0);

        close(taskSigPipe[1]);
        close(resultSigPipe[0]);
        printf("child process forked.\n");
        dup2(taskSigPipe[0], STDIN_FILENO);
        close(taskSigPipe[0]);
        dup2(resultSigPipe[1], STDOUT_FILENO);
        close(resultSigPipe[1]);
        shell_entry(0, NULL, resultSigPipe[1]);
    }
    else
    {
        dispatcherArgs arg1;
        arg1.c = &c;
        arg1.s = &s;
        arg1.m = &m;
        arg1.stopFlag = &stopFlag;
        arg1.taskCnt = &taskCnt;
        pthread_t p;
        pthread_create(&p, NULL, &task_dispatcher, (void *)(&arg1));
        // shmat to attach to shared memory
        char *tupleSpace = (char *)shmat(shmid1, (void *)0, 0);
        char *outputSpace = (char *)shmat(shmid2, (void *)0, 0);

        close(taskSigPipe[0]);
        close(resultSigPipe[1]);
        char *line = readline("test> ");
        char *goOn = "go on", *stop = "stop";
        char resultSignal[100];
        /*while (strcmp(line, stop) != 0)
        {
            printf("parent get the input line: %s\n", line);
            //strcpy(tupleSpace, line);
            write(taskSigPipe[1], line, strlen(line) + 1);
            read(resultSigPipe[0], resultSignal, 100);
            printf("Output from result space: %s\n", resultSignal);
            free(line);
            line = readline("test> ");
        }*/
        while (strcmp(line, stop) != 0)
        {   
            /*
            strcpy(tupleSpace, line); //put the task in share memory
            strcat(tupleSpace, " parent touch");
            write(taskSigPipe[1], line, strlen(line) + 1); //notify the child
            read(resultSigPipe[0], resultSignal, 100);     //wait for signal from child
            printf("parent get the line from child: %s\n", outputSpace);*/
            pthread_mutex_lock(&m);
            taskCnt++;
            //printf("task count address: %p, passed address:%p\n", (void *)&taskCnt, (void *)arg1.taskCnt);
            pthread_cond_signal(&c);
            pthread_mutex_unlock(&m);
            free(line);
            line = readline("test> ");
        }
        write(taskSigPipe[1], "stop", strlen("stop") + 1);
        //printf("main is acquiring stop lock\n");
        pthread_mutex_lock(&s);
        //printf("main locked stop\n");
        stopFlag = 1;
        pthread_mutex_unlock(&s);
        //printf("main unlocked stop\n");
        pthread_mutex_lock(&m);
        if (taskCnt == 0)
        {
            taskCnt = -1;//the dispatcher may be waiting for new task
            pthread_cond_signal(&c);
        }
        pthread_mutex_unlock(&m);
        printf("main thread is waiting for dispatcher to return.\n");
        pthread_join(p, NULL);
        //printf("stop command received in parent.");
        //write(taskSigPipe[1], stop, strlen(stop) + 1);
        close(taskSigPipe[1]);
        close(resultSigPipe[0]);
        free(line);
        //detach from shared memory
        shmdt(tupleSpace);
        shmdt(outputSpace);

        //wait for shell to finish
        pid = wait(&status);
        //remove shared memory
        shmctl(shmid1, IPC_RMID, NULL);
        shmctl(shmid1, IPC_RMID, NULL);
    }

    return 0;
}

void task_dispatcher(dispatcherArgs *arg)
{
    //printf("task count address in dispatcher: %p\n", (void *)arg->taskCnt);
    while(1)
    {
        //printf("dispatcher is acquiring stop lock.\n");
        pthread_mutex_lock(arg->s);
        //printf("dispatcher locked stop\n");
        if ((*(arg->stopFlag) == 1) && (*(arg->taskCnt) == 0))
        {
            pthread_mutex_unlock(arg->s);
            //printf("dispatcher unlocked stop\n");
            return;
        }
        pthread_mutex_unlock(arg->s);
        //printf("dispatcher unlocked stop\n");
        pthread_mutex_lock(arg->m);
        while(*(arg->taskCnt) == 0)
        {
            pthread_cond_wait(arg->c, arg->m);
        }
        if (*(arg->taskCnt) == -1)
        {
            printf("stop signal received in child thread");
            return;
        }
        printf("current task number: %d\n", *(arg->taskCnt));
        (*(arg->taskCnt))--;
        pthread_mutex_unlock(arg->m);
        sleep(3);
    }
}

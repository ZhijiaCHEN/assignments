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
#include "myshell/myshell.h"

int main()
{
    // ftok to generate unique key
    key_t key1 = ftok("shared memory for tuple space", 65), key2 = ftok("shared memory for output space", 65);
    pid_t pid, status;
    int taskSigPipe[2], resultSigPipe[2];

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

        //dup2(resultSigPipe[1], STDOUT_FILENO);
        //shell_entry(0, NULL, resultSigPipe[1]);

        char l1[100];
        read(STDIN_FILENO, l1, 100);
        while (strcmp(l1, "stop") != 0)
        {
            printf("child get the line from parent: %s\n", tupleSpace);
            strcpy(outputSpace, tupleSpace);
            strcat(outputSpace, " child touch");
            write(resultSigPipe[1], "done", strlen("done") + 1);
            read(STDIN_FILENO, l1, 100);
        }

        //detach from shared memory
        shmdt(tupleSpace);
        shmdt(outputSpace);

        close(resultSigPipe[1]);
    }
    else
    {
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
            strcpy(tupleSpace, line); //put the task in share memory
            strcat(tupleSpace, " parent touch");
            write(taskSigPipe[1], line, strlen(line) + 1); //notify the child
            read(resultSigPipe[0], resultSignal, 100);     //wait for signal from child
            printf("parent get the line from child: %s\n", outputSpace);
            free(line);
            line = readline("test> ");
        }
        write(taskSigPipe[1], "stop", strlen("stop") + 1);
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
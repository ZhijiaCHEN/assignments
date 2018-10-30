#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include<sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "myshell/myshell.h"

int main() 
{ 
    // ftok to generate unique key 
    key_t key1 = ftok("shared memory for tuple space",65), key2 = ftok("shared memory for output space",65);
    pid_t pid, status;
    int taskSigPipe[2], resultSigPipe[2];

    // shmget returns an identifier in shmid 
    int shmid1 = shmget(key1,1024,0666|IPC_CREAT); 
    int shmid2 = shmget(key2,1024,0666|IPC_CREAT); 

    // shmat to attach to shared memory 
    char *tupleSpace = (char*) shmat(shmid1,(void*)0,0); 
    char *outputSpace = (char*) shmat(shmid2,(void*)0,0); 

    //start shell process
    pid = fork();
    if (pid == 0)
    {
        printf("child process forked.");
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
        dup2(taskSigPipe[0], STDIN_FILENO);
        //dup2(resultSigPipe[1], STDOUT_FILENO);
        shell_entry(0, NULL, resultSigPipe[1]);
        close(taskSigPipe[0]);
        close(taskSigPipe[1]);
        close(resultSigPipe[0]);
        close(resultSigPipe[1]);
    }
    else
    {
        char *line = readline( "test> " );
        char *goOn = "go on", *stop = "stop";
        char resultSignal[100];
        while (strcmp(line, stop) != 0)
        {
            printf("parent get the input line: %s\n", line);
            strcpy(tupleSpace, line);
            write(taskSigPipe[1], goOn, strlen(goOn)+1);
            read(resultSigPipe[0], resultSignal, 100);
            printf("Output from result space: %s\n", outputSpace);
            free(line);
            line = readline( "test> " );
        }
        printf("stop command received in parent.");
        write(taskSigPipe[1], stop, strlen(stop)+1);
        write(taskSigPipe[1], stop, strlen(stop)+1);
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
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

int main() 
{ 
    // ftok to generate unique key 
    key_t key = ftok("shmfile",65); 

    // shmget returns an identifier in shmid 
    int shmid = shmget(key,1024,0666|IPC_CREAT); 

    // shmat to attach to shared memory 
    char *str = (char*) shmat(shmid,(void*)0,0); 

    char *line = readline( "test> " );
    strcpy(str, line);
    free(line);
    //detach from shared memory 
    shmdt(str); 

    return 0; 
}
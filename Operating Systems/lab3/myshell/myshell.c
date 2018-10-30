#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

char *read_line();
/*
    List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
    "cd",
    "help",
    "exit"};

int num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

/*
    Builtin function implementations.
*/

/**
     @brief Bultin command: change directory.
     @param args List of args.    args[0] is "cd".    args[1] is the directory.
     @return Always returns 1, to continue executing.
 */
int sh_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "myshell: expected argument to \"cd\"\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("myshell");
        }
    }
    return 1;
}

/**
     @brief Builtin command: print help.
     @param args List of args.    Not examined.
     @return Always returns 1, to continue executing.
 */
int sh_help(char **args)
{
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (int i = 0; i < num_builtins(); i++)
    {
        printf("    %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

/**
     @brief Builtin command: exit.
     @param args List of args.    Not examined.
     @return Always returns 0, to terminate execution.
 */
int sh_exit(char **args)
{
    return 0;
}

int (*builtin_func[])(char **) = {
    &sh_cd,
    &sh_help,
    &sh_exit};

/**
    @brief Launch a program and wait for it to terminate.
    @param args Null terminated list of arguments (including program).
    @return Always returns 1, to continue execution.
 */
int launch(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        // Child process
        if (execvp(args[0], args) == -1)
        {
            perror("myshell");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        // Error forking
        perror("myshell");
    }
    else
    {
        // Parent process
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED); // waitpid can wait multiple process, wpid returns the pid of the signaled process
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

/**
    @print out the parsed commands
 */
void print_cmds(char ***cmds)
{
    int i = 0, j = 0;
    while (cmds[i])
    {
        j = 0;
        while (cmds[i][j])
        {
            printf("%s ", cmds[i][j]);
            ++j;
        }
        printf("\n");
        ++i;
    }
}

/**
    @brief Launch one or multiple piped programs and wait for it/them to terminate.
    @param cmds Null terminated list of commands.
    @return Always returns 1, to continue execution.
 */
int pipe_launch(char ***cmds)
{
    pid_t pid, wpid;
    int status, cmdCnt = 0, *pipeFds, redirectFd = -1, argCnt;
    char *buf, *sPos, *ePos, redirect;

    while (cmds[cmdCnt])
        ++cmdCnt;
    if (cmdCnt == 0)
        return 1;

    pipeFds = malloc((cmdCnt - 1) * 2 * sizeof(int));
    for (int i = 0; i < cmdCnt - 1; ++i)
    {
        if (pipe(&pipeFds[2 * i]) < 0)
        {
            printf("Pipe could not be initialized\n");
            for (int j = 0; j < i; ++j)
            {
                close(pipeFds[2 * j]);
                close(pipeFds[2 * j + 1]);
            }
            free(pipeFds);
            return 1;
        }
    }

    for (int i = 0; i < cmdCnt; ++i)
    {
        pid = fork();
        if (pid == 0)
        {
            if (i != 0)
            {
                if (dup2(pipeFds[2 * (i - 1)], STDIN_FILENO) < 0) //duplicate the file descriptor for the read end of the pipe to file descriptor STDIN_FILENO
                {
                    //printf("I am child %d, something went wrong with fds[%d]\n", i, 2 * (i - 1));
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                /*Check redirection for input. 
                We assume that input redirection can only appear in the first command, otherwise it's a syntax error.
                The shell does not handle syntax error, nor does it check syntax error
                */
                argCnt = 0;
                redirect = '<';
                while (cmds[i][argCnt])
                    ++argCnt;
                for (int j = 0; j < argCnt; ++j)
                {
                    sPos = strchr(cmds[i][j], redirect);
                    if (sPos != NULL) //we get '<'
                    {
                        if (sPos[1] == '\0') // cmds[i][j] is '?<'
                        {
                            redirectFd = open(cmds[i][j + 1], O_RDONLY | O_CREAT, S_IRWXU);
                            if (redirectFd < 0)
                            {
                                printf("unable to open file: %s", cmds[i][j + 1]);
                                exit(EXIT_FAILURE);
                            }
                            if (sPos == cmds[i][j]) // cmds[i][j] is '<'
                            {
                                for (int k = j + 2;; ++k)
                                {
                                    cmds[i][k - 2] = cmds[i][k];
                                    if (cmds[i][k] == NULL)
                                        break;
                                }
                            }
                            else // cmds[i][j] is '*<'
                            {
                                sPos[0] = '\0';
                                for (int k = j + 2;; ++k)
                                {
                                    cmds[i][k - 1] = cmds[i][k];
                                    if (cmds[i][k] == NULL)
                                        break;
                                }
                            }
                        }
                        else // cmds[i][j] is '?<*'
                        {
                            redirectFd = open(sPos + 1, O_RDONLY | O_CREAT, S_IRWXU);
                            if (redirectFd < 0)
                            {
                                printf("unable to open file: %s", sPos + 1);
                                exit(EXIT_FAILURE);
                            }
                            if (sPos == cmds[i][j]) // cmds[i][j] is '<*'
                            {
                                for (int k = j + 1;; ++k)
                                {
                                    cmds[i][k - 1] = cmds[i][k];
                                    if (cmds[i][k] == NULL)
                                        break;
                                }
                            }
                            else // cmds[i][j] is '*<*'
                            {
                                sPos[0] = '\0';
                            }
                        }
                        if (dup2(redirectFd, STDIN_FILENO) < 0) //duplicate the file descriptor for the write end of the pipe to file descriptor STDIN_FILENO
                        {
                            perror("dup2");
                            exit(EXIT_FAILURE);
                        }
                        close(redirectFd);
                        redirectFd = -1;
                        break;
                    }
                }
            }

            if (i != cmdCnt - 1)
            {
                if (dup2(pipeFds[2 * i + 1], STDOUT_FILENO) < 0) //duplicate the file descriptor for the write end of the pipe to file descriptor STDOUT_FILENO
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                /*check redirection for output. 
                We assume that ouput redirection can only appears in the last command, otherwise it's a syntax error.
                The shell do not handle syntax error, nor do it check syntax error
                */
                argCnt = 0;
                redirect = '>';
                while (cmds[i][argCnt])
                    ++argCnt;
                for (int j = 0; j < argCnt; ++j)
                {
                    sPos = strchr(cmds[i][j], redirect);
                    if (sPos != NULL) //we get '>', need to further check if there is '>>'
                    {
                        if (sPos[1] == '>') //we get >>
                        {
                            ePos = sPos + 1;
                        }
                        else
                        {
                            ePos = sPos;
                        }
                        if (ePos[1] == '\0') // cmds[i][j] is '?>'
                        {
                            if (sPos == ePos) // >
                            {
                                redirectFd = open(cmds[i][j + 1], O_WRONLY | O_CREAT, S_IRWXU);
                            }
                            else // >>
                            {
                                redirectFd = open(cmds[i][j + 1], O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
                            }
                            if (redirectFd < 0)
                            {
                                printf("unable to open file: %s", cmds[i][j + 1]);
                                exit(EXIT_FAILURE);
                            }
                            if (sPos == cmds[i][j]) // cmds[i][j] is '>'
                            {

                                for (int k = j + 2;; ++k)
                                {
                                    cmds[i][k - 2] = cmds[i][k];
                                    if (cmds[i][k] == NULL)
                                        break;
                                }
                            }
                            else // cmds[i][j] is '*>'
                            {
                                sPos[0] = '\0';
                                for (int k = j + 2;; ++k)
                                {
                                    cmds[i][k - 1] = cmds[i][k];
                                    if (cmds[i][k] == NULL)
                                        break;
                                }
                            }
                        }
                        else // cmds[i][j] is '?>*'
                        {
                            if (sPos == ePos) // >
                            {
                                redirectFd = open(ePos + 1, O_WRONLY | O_CREAT, S_IRWXU);
                            }
                            else // >>
                            {
                                redirectFd = open(ePos + 1, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
                            }
                            if (redirectFd < 0)
                            {
                                printf("unable to open file: %s", ePos + 1);
                                exit(EXIT_FAILURE);
                            }
                            if (sPos == cmds[i][j]) // cmds[i][j] is '>*'
                            {
                                for (int k = j + 1;; ++k)
                                {
                                    cmds[i][k - 1] = cmds[i][k];
                                    if (cmds[i][k] == NULL)
                                        break;
                                }
                            }
                            else // cmds[i][j] is '*>*'
                            {
                                sPos[0] = '\0';
                            }
                        }
                        if (dup2(redirectFd, STDOUT_FILENO) < 0) //duplicate the file descriptor for the write end of the pipe to file descriptor STDOUT_FILENO
                        {
                            perror("dup2");
                            exit(EXIT_FAILURE);
                        }

                        close(redirectFd);
                        redirectFd = -1;
                        break;
                    }
                }
            }
            for (int j = 0; j < cmdCnt - 1; ++j)
            {

                close(pipeFds[2 * j]);
                close(pipeFds[2 * j + 1]);
            }

            if (execvp(cmds[i][0], cmds[i]) < 0)
            {
                printf("Failed to execute command \"%s\"...", cmds[i][0]);
                exit(EXIT_FAILURE);
            }

            exit(0);
        }
        else if (pid < 0)
        {
            printf("Could not fork\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            //printf("child %d has pid: %d\n", i, pid);
        }
    }

    for (int j = 0; j < cmdCnt - 1; ++j)
    {
        close(pipeFds[2 * j]);
        close(pipeFds[2 * j + 1]);
    }

    for (int i = 0; i < cmdCnt; ++i)
    {
        pid = wait(&status);
        if (pid < 0)
        {
            fprintf(stderr, "one child exits with error status: %d\n", status);
            exit(EXIT_FAILURE);
        }
        //printf("child process %d exit with status %d\n", pid, status);
    }

    return 1;
}

/**
     @brief Execute shell built-in or launch program.
     @param cmds Null terminated array of commands, each command is a Null terminated array of arguments.
     @return 1 if the shell should continue running, 0 if it should terminate
 */
int execute(char ***cmds)
{
    int i, j;

    if (cmds[0] == NULL)
    {
        // An empty command was entered.
        return 1;
    }

    i = 0;
    while (cmds[i] != NULL)
    {
        for (j = 0; j < num_builtins(); ++j)
        {
            if (strcmp(cmds[i][0], builtin_str[j]) == 0)
            {
                if (i > 0 || cmds[i + 1] != NULL)
                {
                    printf("I do not handle pipes with build-in functions...");
                    return 1;
                }
                else
                {
                    return (*builtin_func[j])(cmds[i]);
                }
            }
        }
        ++i;
    }

    return pipe_launch(cmds);
}

#define INPUT_BUFSIZE 1024
/**
     @brief Read a line of input from stdin.
     @return The line from stdin.
 */
char *read_line(void)
{
    int bufsize = INPUT_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer)
    {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // Read a character
        c = getchar();

        // If we hit EOF, replace it with a null character and return.
        if (c == EOF || c == '\n')
        {
            buffer[position] = '\0';
            return buffer;
        }
        else
        {
            buffer[position] = c;
        }
        position++;

        // If we have exceeded the buffer, reallocate.
        if (position >= bufsize)
        {
            bufsize += INPUT_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer)
            {
                fprintf(stderr, "myshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

#define ARG_BUFSIZE 64
#define ARG_DELIM " \t\r\n\a"
/**
     @brief Split a command into arguments (very naively).
     @param cmd The command.
     @return Null-terminated array of arguments.
 */
char **split_arg(char *cmd)
{
    int bufsize = ARG_BUFSIZE, position = 0;
    char **args = malloc(bufsize * sizeof(char *));
    char *argToken;

    if (!args)
    {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    argToken = strtok(cmd, ARG_DELIM);
    while (argToken != NULL)
    {
        args[position] = argToken;
        position++;

        if (position >= bufsize)
        {
            bufsize += ARG_BUFSIZE;
            args = realloc(args, bufsize * sizeof(char *));
            if (!args)
            {
                fprintf(stderr, "myshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        argToken = strtok(NULL, ARG_DELIM);
    }
    args[position] = NULL;
    return args;
}

#define CMD_BUFSIZE 8
#define CMD_DELIM "|"
/**
     @brief Split a line into an array of commands and further split every command into an array of arguments (very naively).
     @param line The line.
     @return Null-terminated array of commands, each command is a pointer to a Null-terminated array of arguments.
 */
char ***split_cmds(char *line)
{
    int bufsize = CMD_BUFSIZE, position = 0;
    char ***cmds = malloc(CMD_BUFSIZE * sizeof(void *));
    char *cmdToken;

    if (!cmds)
    {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    cmdToken = strtok(line, CMD_DELIM);
    while (cmdToken != NULL)
    {
        cmds[position] = (void *)cmdToken;
        position++;

        if (position >= bufsize)
        {
            bufsize += CMD_BUFSIZE;
            cmds = realloc(cmds, bufsize * sizeof(char **));
            if (!cmds)
            {
                fprintf(stderr, "myshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        cmdToken = strtok(NULL, CMD_DELIM);
    }
    cmds[position] = NULL;
    for (int i = 0; i < position; ++i)
    {
        cmds[i] = split_arg((void *)cmds[i]);
    }

    return cmds;
}

/**
     @brief Loop getting input and executing it.
 */
void loop(void)
{
    char *line;
    char ***cmds;
    int status, cmdIdx;

    do
    {
        printf("> ");
        line = read_line();
        cmds = split_cmds(line);
        status = execute(cmds);

        free(line);
        cmdIdx = 0;
        while (cmds[cmdIdx] != NULL)
        {
            free(cmds[cmdIdx]);
            cmdIdx++;
        }
        free(cmds);

    } while (status);
}

/**
     @brief Main entry point.
     @param argc Argument count.
     @param argv Argument vector.
     @return status code
 */
int shell_entry(int argc, char **argv, int outFd)
{
    printf("shell launched, shell is going to read first line");
    char *line = read_line();
    char *goOn = "go on", *stop = "stop";
    // ftok to generate unique key 
    key_t key1 = ftok("shared memory for tuple space",65), key2 = ftok("shared memory for output space",65);

    // shmget returns an identifier in shmid 
    int shmid1 = shmget(key1,1024,0666|IPC_CREAT); 
    int shmid2 = shmget(key2,1024,0666|IPC_CREAT); 

    // shmat to attach to shared memory 
    char *tupleSpace = (char*) shmat(shmid1,(void*)0,0); 
    char *outputSpace = (char*) shmat(shmid2,(void*)0,0); 
    while (strcmp(line, stop) != 0)
    {
        strcpy(outputSpace, tupleSpace);
        write(outFd, goOn, strlen(goOn)+1);
        printf("task finished, shell is going to read new line");
        free(line);
        line = read_line();
    }
    free(line);
    //loop();

    // Perform any shutdown/cleanup.

    return EXIT_SUCCESS;
}

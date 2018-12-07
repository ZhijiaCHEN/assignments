/***************************************************************************/ /**

  @file         main.c

  @author       Stephen Brennan

  @date         Thursday,  8 January 2015

  @brief        LSH (Libstephen SHell). Revised by Justin Y. Shi for remote shell demo. Nov. 2018

*******************************************************************************/

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <memory.h>
#include "synergy.h"

/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
    "cd",
    "help",
    "exit"};

int (*builtin_func[])(char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit};

int lsh_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int lsh_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("lsh");
        }
    }
    return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_help(char **args)
{
    int i;
    printf("Stephen Brennan's LSH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < lsh_num_builtins(); i++)
    {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_exit(char **args)
{
    return 0;
}

/**
    @wait for a child process to return and copy its output to the given shell_out data structure
    @param pid the child pid to wait
    @return void
**/
void wait_child(shell_out_list *shellOutElmPtr)
{
    int *status = &shellOutElmPtr->shellOutPtr->status;
    do
    {
        waitpid(shellOutElmPtr->shellOutPtr->pid, status, WUNTRACED);
    } while (!WIFEXITED(*status) && !WIFSIGNALED(*status));

    /* Read from child’s stdout, read after child process has finished */
    int count = read(shellOutElmPtr->fd, shellOutElmPtr->shellOutPtr->stdout, sizeof(shellOutElmPtr->shellOutPtr->stdout));
    if (count == -1)
    {
        sprintf(shellOutElmPtr->shellOutPtr->stdout, "Failed to read output from process #%d.\n", shellOutElmPtr->shellOutPtr->pid);
    }

    close(shellOutElmPtr->fd);
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{
    pid_t pid, wpid;
    int status, count;
    int filedes[2]; // For capture fork output

    // Create the pipe for capture stdout of child process
    if (pipe(filedes) == -1)
    {
        perror("pipe");
        exit(1);
    }
    pid = fork();
    if (pid == 0)
    {
        // Child process
        while ((dup2(filedes[1], STDOUT_FILENO) == -1) && (errno == EINTR))
        {
        };
        close(filedes[1]); // the child does not need these
        close(filedes[0]);
        if (execvp(args[0], args) == -1)
        {
            perror("lsh");
            strcpy(shell_out.stdout, "execvp failure");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        // Error forking
        perror("lsh");
        strcpy(shell_out.stdout, "fork failure");
    }
    else
    {
        // Parent process
        printf("launched child process id %d\n", pid);
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        bzero(shell_out.stdout, sizeof(shell_out.stdout)); // Remove all contents
        shell_out.status = 111;
        shell_out.pid = 12345;
        close(filedes[1]);
        /* Read from child’s stdout */
        while (1)
        {
            count = read(filedes[0], shell_out.stdout, sizeof(shell_out.stdout));
            if (count == -1)
            {
                if (errno == EINTR)
                {
                    perror("Interrupted?");
                    continue;
                }
                else
                {
                    perror("Pipe read error");
                    exit(1);
                }
            }
            else if (count == 0)
            {
                printf("Nothing to read. count=0 ??? \n");
                break;
            }
            else
            {
                printf("%s", shell_out.stdout);
                break;
            }
        }
        close(filedes[0]);
    }

    return 1;
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

    if (cmdCnt == 1) // we do not fork child if there is only one cmd
    {
        if (execvp(cmds[0][0], cmds[0]) < 0)
        {
            printf("Failed to execute command \"%s\"...", cmds[0][0]);
            exit(EXIT_FAILURE);
        }
        else
        {
            return 1;
        }
    }

    pipeFds = malloc((cmdCnt)*2 * sizeof(int)); // The output of the last cmd should be sent to the launch process
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
            else // This is the first cmd
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
            else // This is the last cmd
            {

                /*check redirection for output. 
                We assume that ouput redirection can only appears in the last command, otherwise it's a syntax error.
                The shell does not handle syntax error, nor does it check syntax error
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
            fprintf(stderr, "cmd execution failed.");
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
int lsh_execute(char ***cmds)
{
    int i, j, status, count;
    pid_t pid, wpid;
    int filedes[2]; // For capture output of the launch process
    shell_out_list *shellOutListElmPtr;

    if (cmds[0] == NULL)
    {
        // An empty command was entered.
        return 1;
    }

    // Check for built-in cmds
    i = 0;
    while (cmds[i] != NULL)
    {
        for (j = 0; j < lsh_num_builtins(); ++j)
        {
            if (strcmp(cmds[i][0], builtin_str[j]) == 0)
            {
                if (i > 0 || cmds[i + 1] != NULL)
                {
                    strcpy(shell_out.stdout, "I do not handle pipes with build-in functions...");
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

    // Create the pipe for capture stdout of the launch process
    if (pipe(filedes) == -1)
    {
        perror("pipe");
        strcpy(shell_out.stdout, "pipe failure, abort launch.");
        shell_out.status = -1;
        return 0;
    }
    pid = fork();
    if (pid == 0)
    {
        // launch process
        while ((dup2(filedes[1], STDOUT_FILENO) == -1) && (errno == EINTR))
        {
        };
        close(filedes[1]); // the child does not need these
        close(filedes[0]);
        pipe_launch(cmds);
        exit(0); // the launch process must exist after the cmd is executed
    }
    else if (pid < 0)
    {
        // Error forking
        perror("fork");
        strcpy(shell_out.stdout, "fork failure, abort launch.");
        shell_out.status = -1;
        return 0;
    }
    else
    {
        // Parent process
        bzero(shell_out.stdout, sizeof(shell_out.stdout)); // Remove all contents
        close(filedes[1]);

        shellOutListElmPtr = (shell_out_list *)malloc(sizeof(shell_out_list));
        shellOutListElmPtr->fd = filedes[0];
        shellOutListElmPtr->next = NULL;

        //check if cmds ends with &
        i = 0;
        while (cmds[i])
            ++i;
        j = 0;
        while (cmds[i][j])
            ++j;
        if (cmds[i][j - 1] == '&') // run the cmd in the background
        {
            pthread_t wait_thread;

            shellOutListElmPtr->shellOutPtr = (struct shell_out_t *)malloc(sizeof(struct shell_out_t));
            shellOutListElmPtr->shellOutPtr->pid = pid;

            if (shellOutListTail)
                shellOutListTail->next = shellOutListElmPtr;
            shellOutListTail = shellOutListElmPtr;
            pthread_create(&wait_thread, NULL, wait_child, shellOutListElmPtr); //the thread will wait the child process and copy its output to the shell output list
            pthread_detach(wait_thread);
            shell_out.status = 0;
            sprintf(shell_out.stdout, "process #%d started.\n", pid);
        }
        else
        {
            shellOutListElmPtr->shellOutPtr = &shell_out;
            shell_out.pid = pid;
            shellOutListElmPtr->fd = filedes[0];
            wait_child(shellOutListElmPtr);
            free(shellOutListElmPtr);
        }

        return 1;
    }
}

#define LSH_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 
   *** Need to change to socket readn()
   
char *lsh_read_line(void)
{
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}
*/

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

void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}
Removed ***/

/** Removed by JYS Nov. 2018
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 
int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}

***/

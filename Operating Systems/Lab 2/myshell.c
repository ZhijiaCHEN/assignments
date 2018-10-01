#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
    List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int num_builtins() {
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
    if (args[1] == NULL) {
        fprintf(stderr, "myshell: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
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
    int i;
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < num_builtins(); i++) {
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

int (*builtin_func[]) (char **) = {
    &sh_cd,
    &sh_help,
    &sh_exit
};

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
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("myshell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("myshell");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED); // waitpid can wait multiple process, wpid returns the pid of the signaled process
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
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
    int status, cmdCnt = 0, *pipeFds;

    while (cmds[i]) ++cmdCnt;
    if (cmdCnt == 0) return 1;
    else
    {
        if (cmdCnt == 1) return launch(cmds[0]);
    }

    *pipeFds = malloc(cmdCnt*2*sizeof(int));
    for (int i = 0; i < cmdCnt; ++i)
    {
        if (pipe(&pipeFds[2*i]) < 0) 
        { 
            printf("Pipe could not be initialized\n");
            for (int j = 0; j < i; ++j)
            {
                close(pipeFds[2*j]);
                close(pipeFds[2*j+1]);
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
                close(pipefd[2*(i-1)+1]); 
                dup2(pipefd[2*(i-1)], STDIN_FILENO); 
                close(pipefd[2*(i-1)]);
            }

            if (i != cmdCnt - 1)
            {
                close(pipeFds[2*i]); 
                dup2(pipeFds[2*i+1], STDOUT_FILENO); //duplicate the file descriptor for the write end of the pipe to file descriptor STDOUT_FILENO
                close(pipeFds[2*i+1]); 
            }

            if (execvp(parsed[0], parsed) < 0) 
            { 
                printf("Failed to execute command \"%s\"...", cmds[i][0]); 
                exit(EXIT_FAILURE);
            }
        }
        else if (pid < 0)
        {
            printf("Could not fork\n");
            exit(EXIT_FAILURE);
        }
    }
    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("myshell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("myshell");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED); // waitpid can wait multiple process, wpid returns the pid of the signaled process
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
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

    if (cmds[0] == NULL) {
        // An empty command was entered.
        return 1;
    }

    i = 0;
    while (cmds[i] != NULL)
    {
        for (j = 0; j < num_builtins(); ++j) 
        {
            if (strcmp(args[0], builtin_str[j]) == 0) 
            {
                if(i > 0 || cmds[i+1] != NULL)
                {
                    print("I do not handle pipes with build-in functions...");
                    return 1;
                }
                else
                {
                    return (*builtin_func[j])(args);
                }
            }
        }
        ++i;
    }

    return launch(cmds);
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

    if (!buffer) {
        fprintf(stderr, "myshell: allocation error\n");
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
            bufsize += INPUT_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
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
    char **args = malloc(bufsize * sizeof(char*));
    char *argToken;

    if (!args) {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    argToken = strtok(cmd, ARG_DELIM);
    while (argToken != NULL) 
    {
        args[position] = argToken;
        position++;

        if (position >= bufsize) {
            bufsize += ARG_BUFSIZE;
            args = realloc(args, bufsize * sizeof(char*));
            if (!args) {
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
    char ***cmds = malloc(CMD_BUFSIZE * sizeof(char*));
    char *cmdToken;

    if (!cmds) 
    {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    cmdToken = strtok(line, CMD_DELIM);
    while (cmdToken != NULL) 
    {
        cmds[position] = split_arg(cmdToken);
        position++;

        if (position >= bufsize) 
        {
            bufsize += CMD_BUFSIZE;
            cmds = realloc(cmds, bufsize * sizeof(char**));
            if (!cmds) {
                fprintf(stderr, "myshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        cmdToken = strtok(NULL, CMD_DELIM);
    }

    cmds[position] = NULL;
    return cmds;
}

#define CMD_ANCHORBUF 8
#define CMD_DELIM "|"
#define ARG_ANCHORBUF 64
#define ARG_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **split_line(char *line)
{
  int bufsize = TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "myshell: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
     @brief Loop getting input and executing it.
 */
void loop(void)
{
    char *line;
    char ***cmds;
    int status, cmdIdx;

    do {
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
int main(int argc, char **argv)
{
    // Load config files, if any.

    // Run command loop.
    loop();

    // Perform any shutdown/cleanup.

    return EXIT_SUCCESS;
}


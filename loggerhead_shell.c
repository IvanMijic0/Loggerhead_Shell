#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <limits.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

#define STDIN_FILENO 0
#define BUFFER_SIZE 1024

// Clearing the shell using escape sequences
#define clear() printf("\033[H\033[J")

int main(); // Main Function
void init_shell(); // Greeting shell during startup
int takeInput(char* string); // Function to take input and display CWD
int tokenizeInput(char* args[], int num_args, char* input); // Tokenize the input into arguments
void executeShellCommand(char* command, char* arguments[]); // Executes Shell Commands
void executePipedRedirectCommand(char **args, int num_args);
void handleBuiltInCommands(char* args[], int num_args); // Handling Built-in Commads
void launchExternalProgram(char* args[]); // Launching External Program
int findCharacterIndex(char* str, char ch);
void handle_signal(int signal); // Handles quit process
void reverse(char str[], int start, int end); // Reverses a string

int should_exit = 0; // Global variable to check if shell should exit

int main() {

    // Set terminal settings
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    rl_bind_key('\t', rl_insert);

    char input[BUFFER_SIZE];
    char* args[BUFFER_SIZE];
    int num_args = 0;

    init_shell();

    signal(SIGQUIT, handle_signal);

    


    while (1) {
        // Read the user input
        if(takeInput(input)){continue;};

        // Tokenize the input into arguments
        num_args = tokenizeInput(args, num_args, input);

	executePipedRedirectCommand(args, num_args);
        handleBuiltInCommands(args, num_args);
    }

    return 0;
}

void init_shell(){
    clear();

    printf("\n\x1b[1;32m******************"
           "************************");
    printf("\n\n\n\t\x1b[1;32m\x1b[38;5;208m%s\x1b[0m", "*****LOGGERHEAD SHELL****");
    printf("\n\n\t\x1b[1;32m\x1b[38;5;208m%-30s\x1b[0m%s", "-THE MOST POWERFUL SHELL-", "");
    printf("\n\n\n\n\x1b[1;32m*******************"
           "***********************");

    char* username = getenv("USER");
    printf("\n\n\n\t\x1b[1;32mUSER is \x1b[0;35m@\x1b[1;38;5;208m%s", username);
    printf("\n");
    sleep(1);
    clear();
}

int takeInput(char* string) {
    char hostname[BUFFER_SIZE];
    char username[BUFFER_SIZE];
    char cwd[BUFFER_SIZE];
    char prompt[4096];

    // Get the values
    gethostname(hostname, BUFFER_SIZE);
    getlogin_r(username, BUFFER_SIZE);
    getcwd(cwd, BUFFER_SIZE);

    // Display prompt
    snprintf(prompt, sizeof(prompt), "\x1b[1;31m%s\x1b[0;35m@\x1b[1;32m%s\x1b[0;35m:\x1b[0;38;5;208m%s\x1b[0;35m$\x1b[0;32m ", username, hostname, cwd);

    // Read input from user using readline
    char* buffer = readline(prompt);

    // If input is not empty, add it to history and copy to string
    if (strlen(buffer) != 0) {
        add_history(buffer);
        strcpy(string, buffer);
        free(buffer);
        return 0;
    } else {
        free(buffer);
        return 1;
    }
}

int tokenizeInput(char* args[], int num_args, char* input){
    num_args = 0;
    args[num_args] = strtok(input, " \n");
    while (args[num_args] != NULL) {
        num_args++;
        args[num_args] = strtok(NULL, " \n");
    }
    return num_args;
}


void  executeShellCommand(char* command, char* arguments[]) {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        execvp(command, arguments);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Forking error
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
}


void handleBuiltInCommands(char* args[], int num_args){
    char hostname[HOST_NAME_MAX];
    struct utsname unameData;

    if (strcmp(args[0], "cd") == 0) {
        if (num_args == 1) {
            chdir(getenv("HOME"));
        } else {
            chdir(args[1]);
        }
    } else if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "mv") == 0) {
        if (num_args != 3) {
            printf("mv: missing file operand\n");
        } else {
            executeShellCommand("mv", args);
        }
    } else if (strcmp(args[0], "du") == 0) {

        if (num_args > 1) {

            if (strcmp(args[1], "-s") == 0) {

                executeShellCommand("du", args);
            } else if (strcmp(args[1], "-h") == 0) {

                char* du_args[3];
                du_args[0] = "du";
                du_args[1] = "-h";
                du_args[2] = NULL;
                executeShellCommand("du", du_args);

            }
        } else {

            executeShellCommand("du", args);
        }

    } else if (strcmp(args[0], "date") == 0) {

        executeShellCommand("date", args);

    } else if (strcmp(args[0], "rev") == 0) {

        if (num_args > 1 && strcmp(args[1], "-w") == 0){
            for (int i = 2; i < num_args; i++) {
                reverse(args[i], 0, strlen(args[i]) - 1);
            }
            printf("\n");

        } else {
            executeShellCommand("rev", args);
        }
    } else if (strcmp(args[0], "hostname") == 0) {
        gethostname(hostname, HOST_NAME_MAX);
        printf("%s\n", hostname);
    } else if (strcmp(args[0], "uname") == 0) {
        if (uname(&unameData) != -1) {
            printf("%s %s %s %s %s\n", unameData.sysname, unameData.nodename, unameData.release, unameData.version, unameData.machine);
        } }else {
        launchExternalProgram(args);
    }
}

void launchExternalProgram(char* args[]){
    pid_t pid = fork();
    if (pid == 0) {
        dup2(STDOUT_FILENO, STDOUT_FILENO); // redirect stdout to stdout
        execvp(args[0], args);
        exit(0);
    } else {
        waitpid(pid, NULL, 0);
    }
}

void handle_signal(int signal) {
    if (signal == SIGQUIT){
        printf("You're tearing me apart Lisa!");
        exit(0);
    }
}


void reverse(char* str, int start, int end) {
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
    printf("%s ", str);
}

void executePipedRedirectCommand(char **args, int num_args) {
    int pid1, pid2, fd[2];
    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;

    for (int i = 0; i < num_args; i++) {
        if (strcmp(args[i], "<") == 0) {
            input_fd = open(args[i + 1], O_RDONLY);
            if (input_fd == -1) {
                perror("open");
                return;
            }
            args[i] = NULL;
            i++;
        } else if (strcmp(args[i], ">") == 0) {
            output_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd == -1) {
                perror("open");
                return;
            }
            args[i] = NULL;
            i++;
        } else if (strcmp(args[i], ">>") == 0) {
            output_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (output_fd == -1) {
                perror("open");
                return;
            }
            args[i] = NULL;
            i++;
        } else if (strcmp(args[i], "|") == 0) {
	  if (pipe(fd) == -1) {
	    perror("pipe");
	    return;
	  }
	  args[i] = NULL;
	  
	  pid1 = fork();
	  if (pid1 == -1) {
	    perror("fork");
        return;
	  } else if (pid1 == 0) {
	    // Child process for left side of pipe
	    close(fd[0]); // Close read end of pipe
	    dup2(output_fd, STDOUT_FILENO);
	    close(output_fd);
	    dup2(fd[1], STDOUT_FILENO); // Redirect output to write end of pipe
	    close(fd[1]); // Close write end of pipe
	    execvp(args[0], args);
	    perror(args[0]);
	  } else {
	    pid2 = fork();
	    if (pid2 == -1) {
	      perror("fork");
	      return;
	    } else if (pid2 == 0) {
	      // Child process for right side of pipe
	      close(fd[1]); // Close write end of pipe
	      dup2(input_fd, STDIN_FILENO);
	      close(input_fd);
	      dup2(fd[0], STDIN_FILENO); // Redirect input to read end of pipe
	      close(fd[0]); // Close read end of pipe
	      execvp(args[i+1], &args[i+1]);
	      perror(args[i+1]);
	    } else {
	      // Parent process for both children
	      close(fd[0]);
	      close(fd[1]);
	      waitpid(pid1, NULL, 0);
	      waitpid(pid2, NULL, 0);
	      input_fd = fd[0]; // Set input_fd to read end of pipe for next command
	      output_fd = STDOUT_FILENO; // Reset output_fd to STDOUT_FILENO
	    }
	  }
	}
    }
}

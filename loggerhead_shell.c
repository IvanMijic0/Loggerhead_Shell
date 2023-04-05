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
#include <libgen.h>
#include <setjmp.h>

#define STDIN_FILENO 0
#define BUFFER_SIZE 1024

// Clearing the shell using escape sequences
#define clear() printf("\033[H\033[J")

int main(int argc, char *argv[]); // Main Function
void init_shell(); // Greeting shell during startup
void takeInput(char* string); // Function to take input and display CWD
int tokenizeInput(char* args[], int num_args, char* input); // Tokenize the input into arguments
void executeShellCommand(char* command, char* arguments[]); // Executes Shell Commands
void handleBuiltInCommands(char* args[], int num_args); // Handling Built-in Commads
void launchExternalCommands(char* args[]); // Launching External Program
void handle_signal(int signal); // Handles quit process
void reverse(char str[], int start, int end); // Reverses a string
int containsPipe(char** args, int num_args); // Checks if input contains pipe char "|"
int containsRedirect(char** args, int num_args); // Checks if input contains redirect chars "<", ">", ">>"
void executePipedCommand(char **args, int num_args); // Executes piped command
void executeRedirectCommand(char **args, int num_args); // Execites redirect command
void handle_error(jmp_buf env); // Restarts main() if error occurred
void restart(char* argv[], char* input); // Restarts shell on "restart" input
void handleCD(char* args[], int num_args); // Logic for "cd" command executions
void handleMV(char* args[], int num_args); // Logic for "mv" command executions
void handleDU(char* args[], int num_args); // Logic for "du" command executions
void handleREV(char* args[], int num_args); // Logic for "rev" command executions
void handleHostName(); // Logic for "hostname" command executions
void handleUName(); // Logic for "uname" command executions
void help(); // Provides basic overview of commands
void greetings(); // The all-mighty turtle greets you

int should_exit = 0; // Global variable to check if shell should exit

// This is the main function of the program
int main(int argc, char *argv[]) {
    // Define some variables to hold input and arguments
    char input[BUFFER_SIZE];
    char* args[BUFFER_SIZE];
    int num_args = 0;

    // Call a function to initialize the shell
    init_shell();

    // Set up a signal handler for the QUIT signal
    signal(SIGQUIT, handle_signal);

    // Define a jump buffer, which allows us to jump back to a previous point in the code
    jmp_buf env;
    // Use setjmp to save the current state of the program, so we can return here if longjmp is called
    if (setjmp(env) == 0) {
        // Loop indefinitely
        while (1) {
            // Call a function to read user input from the command line
            takeInput(input);

            // Tokenize the user input into arguments
            num_args = tokenizeInput(args, num_args, input);

            // Check if the user input contains a pipe character
            if (containsPipe(args, num_args)) {
                // If so, execute the piped command
                executePipedCommand(args, num_args);

            // Check if the user input contains a redirect character
            } else if (containsRedirect(args, num_args)) {
                // If so, execute the redirect command
                executeRedirectCommand(args, num_args);

            // Otherwise, the user input is a built-in command or an external command
            } else {
                // Check if the user wants to restart the shell
                restart(argv, input);
                // Handle built-in commands like cd, help, exit, mv, du, and hostname
                handleBuiltInCommands(args, num_args);
            }
        }
    } else {
        // If an error occurred, print an error message and jump back to the start of the program
        fprintf(stderr, "An error occurred. Restarting shell.\n");
        main(argc, argv);
    }

    // Return 0 to indicate success
    return 0;
}

// This function handles errors by jumping back to the start of the program
void handle_error(jmp_buf env) {
    fprintf(stderr, "An error occurred. Restarting shell.\n");
    longjmp(env, 1);
}

// This function restarts the shell if the user types "restart"
void restart(char* argv[], char* input) {
    if(strcmp(input, "restart") == 0){
        execv(argv[0], argv);
    }
}

void init_shell(){
    clear();

    printf("\x1b[38;5;208m _____     ____\n/      \\  |  o |\n|        |/ ___\\|\n|_________/\n|_|_| |_|_|\n");


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

void takeInput(char* string) {

    // TODO: implement functioning input with readline() for history.
  
    char hostname[BUFFER_SIZE];
    char username[BUFFER_SIZE];
    char cwd[BUFFER_SIZE];
    char prompt[BUFFER_SIZE * 10];

    // Get the values
    gethostname(hostname, BUFFER_SIZE);
    getlogin_r(username, BUFFER_SIZE);
    getcwd(cwd, BUFFER_SIZE);

    // Display prompt
    snprintf(prompt, sizeof(prompt),
	     "\x1b[1;31m%s\x1b[0;35m@\x1b[1;32m%s\x1b[0;35m:\x1b[0;38;5;208m%s\x1b[0;35m$\x1b[0;32m ",
	     username, hostname, cwd);

    
    printf("%s", prompt);
    fgets(string, BUFFER_SIZE, stdin);
    string[strlen(string) - 1] = '\0';  // Remove newline character

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
  
    if (strcmp(args[0], "cd") == 0) {

      handleCD(args, num_args);
      
    } else if (strcmp(args[0], "help") == 0) {

      help();

    } else if (strcmp(args[0], "hi") == 0) {

      greetings();
      
    } else if (strcmp(args[0], "exit") == 0) {
      
        exit(0);
	
    } else if (strcmp(args[0], "mv") == 0) {
      
      handleMV(args, num_args);
      
    } else if (strcmp(args[0], "du") == 0) {

      handleDU(args, num_args);

    } else if (strcmp(args[0], "date") == 0) {

      executeShellCommand("date", args);

    } else if (strcmp(args[0], "rev") == 0) {

      handleREV(args, num_args);
      
    } else if (strcmp(args[0], "hostname") == 0) {
      
      handleHostName();
	
    } else if (strcmp(args[0], "uname") == 0) {
      
      handleUName();
      
    } else {
      
      launchExternalCommands(args);
      
    }
}


void greetings(){

 printf("\x1b[38;5;208m                                               \n");
printf("                                                  ,;\n");
printf("                                                .'/\n");
printf("           `-_                                .'.'\n");
printf("             `;-_                           .' /\n");
printf("               `.-.        ,_.-'`'--'`'-._.` .'\n");
printf("                 `.`-.    /    .\"\".   _.'  /\n");
printf("                   `. '-.'_.._/0 \" 0\\/`    \\\n");
printf("                     `.      |'-^Y^- |     //\n");
printf("                      (`\\     \\_.\"._/\\...-;..-.\n");
printf("                      `._'._,'` ```    _.:---''`\n");
printf("                         ;-....----'''\n");
printf("                        /   (\n");
printf("                            (`\n");
printf("                        `.^'\n");




 
  
  printf("\n\t\t   \x1b[32m*****\x1b[38;5;208mGreetings my child\x1b[32m***\n\n\n");

}

void help () {

  printf("\n\t***\x1b[38;5;208mReceive the turtles wisdom\x1b[32m***\n");

  printf("\x1b[38;5;208m-cd\n-hi\n-mv\n-exit\n-du\n-du -h\n-du -s\n-date\n-rev\n-rev -w\n-hostname\n-uname\n-basic external commands\n-allows piping\n-allows redirecting\n\n");

}


void handleUName() {

  struct utsname unameData;
  
  if (uname(&unameData) != -1) {
    printf("%s %s %s %s %s\n", unameData.sysname, unameData.nodename, unameData.release, unameData.version, unameData.machine);
  }
}

void handleHostName() {
  
  char hostname[HOST_NAME_MAX];
  
  gethostname(hostname, HOST_NAME_MAX);
  printf("%s\n", hostname);
}

void handleREV(char* args[], int num_args) {
  
  if (num_args > 1 && strcmp(args[1], "-w") == 0){
    for (int i = 2; i < num_args; i++) {
      reverse(args[i], 0, strlen(args[i]) - 1);
    }
    printf("\n");
    
  } else {
            executeShellCommand("rev", args);
  }
}

void handleDU(char* args[], int num_args) {
  
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
}

void handleMV(char* args[], int num_args) {
  
  if (num_args != 3) {
    printf("mv: missing file operand\n");
  } else {
    executeShellCommand("mv", args);
  }	
}

void handleCD(char* args[], int num_args) {

  if (num_args == 1) {
    chdir(getenv("HOME"));
  } else {
    chdir(args[1]);
  }
  
}

void launchExternalCommands(char* args[]) {
  
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

int containsPipe(char** args, int num_args) {
  
    for (int i = 0; i < num_args; i++) {
      
        if (strcmp(args[i], "|") == 0) {
            return 1;
        }
    }
    return 0;
}

int containsRedirect(char** args, int num_args) {
  
    for (int i = 0; i < num_args; i++) {

        if (strcmp(args[i], "<") == 0 || strcmp(args[i], ">") == 0 ||
	    strcmp(args[i], ">>") == 0) {
            return 1;
        }
    }
    return 0;
}


void executeRedirectCommand(char **args, int num_args) {
  
    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;
    int saved_input_fd = dup(STDIN_FILENO);

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
        }
    }

    int pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
	
    } else if (pid == 0) {
        // Child process
        dup2(input_fd, STDIN_FILENO);
        dup2(output_fd, STDOUT_FILENO);
        close(input_fd);
        close(output_fd);
        execvp(args[0], args);
        perror(args[0]);
	
    } else {
        // Parent process
        waitpid(pid, NULL, 0);
        dup2(saved_input_fd, STDIN_FILENO);
        close(saved_input_fd);
        close(output_fd); // close output_fd after child process is done writing
    }
}


void executePipedCommand(char **args, int num_args) {
  
    int pid1, pid2, fd[2];
    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;

    for (int i = 0; i < num_args; i++) {
      
        if (strcmp(args[i], "|") == 0) { // Handle piping
	  
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

#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "readline/readline.h"
#include "readline/history.h"

#define MAXCOMMANDS 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported

// Clearing the shell using escape sequences
#define clear() printf("\033[H\033[J")

int main(); // Main Function
void init_shell(); // Greeting shell during startup
int takeInput(char* string); // Function to take input
void printDirectory(); // Function to print Current Directory
void executeArguments(char** parsed); // Function where the system command is executed
void executePipedArguments(char** parsed, char** parsedPipe); // Function where the piped system commands are executed
void openHelp(); // Help command built in
int ownCMDHandler(char** parsed); // Function to execute built in commands
int parsePipe(char* string, char** stringPiped); // Function for finding pipe
void parseSpace(char* string, char** parsed); // Function for parsing command words
int processString(char* string, char** parsed, char** parsedPipe);

int main(){
    char inputString[MAXCOMMANDS], *parsedArguments[MAXLIST];
    char* parsedPipedArguments[MAXLIST];
    int executionFlag;

    init_shell();

    while (1){
        // Print Shell line
        printDirectory();
        // Take input
        if (takeInput(inputString)){ continue;}
        // Process
        executionFlag = processString(inputString,parsedArguments, parsedPipedArguments);
        // Execution Flag returns zero if there is no command
        // or it is a builtin command
        // 1 if it is a simple command
        // 2 if pipe is included

        // Execute
        if (executionFlag == 1){ executeArguments(parsedArguments);}
        if (executionFlag == 2){ executePipedArguments(parsedArguments, parsedPipedArguments);}
    }
    return 0;
}

void init_shell(){
    clear();

    printf("\n\n\n\n******************"
           "************************");
    printf("\n\n\n\t****LOGGERHEAD SHELL****");
    printf("\n\n\t-THE MOST POWERFUL SHELL-");
    printf("\n\n\n\n*******************"
           "***********************");

    char* username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    sleep(1);
    clear();
}

int takeInput(char* string){
    char* buffer;

    buffer = readline("\n>>> ");
    if (strlen(buffer) != 0) {
        add_history(buffer);
        strcpy(string, buffer);
        return 0;
    }
    else{
        return 1;
    }
}

void printDirectory(){
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\nDirectory: %s", cwd);
}

void executeArguments(char** parsed){
    // Forking a child
    pid_t pid = fork();

    if (pid == -1){
        printf("\nFailed forking child...");
        return;
    }
    else if (pid == 0){
        if (execvp(parsed[0], parsed) < 0){
            printf("\nCould not execute command...");
        }
        exit(0);
    }
    else{
        // Waiting for a child to terminate
        wait(NULL);
        return;
    }
}

void executePipedArguments(char** parsed, char** parsedPipe){
    // 0 is "read" end, 1 is "write" end
    int pipeFD[2];
    pid_t process1, process2;

    if (pipe(pipeFD) < 0){
        printf("\nPipe could not be initialized");
        return;
    }

    process1 = fork();

    if (process1 == 0){
        // Child 1 is executing...
        // It only needs to write at the write end
        close(pipeFD[0]);
        dup2(pipeFD[1], STDOUT_FILENO);
        close(pipeFD[1]);

        if (execvp(parsed[0], parsed) < 0){
            printf("\nCould not execute command 1...");
            exit(0);
        }
    } else {
        // Parent executing
        process2 = fork();

        if (process2 < 0){
            printf("\nCould not fork");
            return;
        }

        // Child 2 is executing...
        // It only needs to read at the "read" end
        if (process2 == 0){
            close(pipeFD[1]);
            dup2(pipeFD[0], STDOUT_FILENO);
            close(pipeFD[0]);
            if (execvp(parsedPipe[0], parsedPipe) < 0){
                printf("\nCould not execute command 2...");
                exit(0);
            }
        } else{
            // Parent executing, waiting for two children
            wait(NULL);
            wait(NULL);
        }
    }
}

void openHelp(){
    puts("\n***WELCOME TO LOGGERHEAD SHELL HELP***"
         "\nCopyright @imijic"
         "\n***TELL THE MIGHTY TURTLE WHAT TROUBLES YOU***"
         "\nList of supported commands:"
         "\n>cd"
         "\n>ls"
         "\n>exit"
         "\n>all other general commands available in UNIX shell"
         "\n>pipe handling"
         "\n>improper space handling");
}

int ownCMDHandler(char** parsed){
    int numberOfOwnCMDs = 4, i, switchOwnArgument = 0;
    char* ListOfOwnCMDs[numberOfOwnCMDs];
    char* username;

    ListOfOwnCMDs[0] = "exit";
    ListOfOwnCMDs[1] = "cd";
    ListOfOwnCMDs[2] = "help";
    ListOfOwnCMDs[3] = "hello";

    for (i = 0; i <  numberOfOwnCMDs; i++) {
        if (strcmp(parsed[0], ListOfOwnCMDs[i]) == 0){
            switchOwnArgument = i + 1;
            break;
        }
    }

    switch (switchOwnArgument) {
        case 1:
            printf("\nSee ya!\n");
            exit(0);
        case 2:
            chdir(parsed[1]);
            return 1;
        case 3:
            openHelp();
            return 1;
        case 4:
            username = getenv("USER");
            printf("\nGreetings %s.\nMind that this is "
                   "not a place to play around in."
                   "\nUse \"help\" to know more...\n",
                   username);
            return 1;
        default:
            break;
    }
    return 0;
}

int parsePipe(char* string, char** stringPiped){
    int i;
    for (i = 0; i < 2; i++) {
        stringPiped[i] = strsep(&string, "|");
        if (stringPiped[i] == NULL){break;} // Returns a zero if no pipe is found
    }
    if (stringPiped[1] == NULL){return 0;}
    else {return 1;}
}

void parseSpace(char* string, char** parsed){
    int i;

    for (i = 0; i < MAXLIST; i++) {
        parsed[i] = strsep(&string, " ");

        if (parsed[i] == NULL){break;}
        if (strlen(parsed[i]) == 0){i--;}
    }
}

int processString(char* string, char** parsed, char** parsedPipe){
    char* stringPiped[2];
    int piped;

    piped = parsePipe(string, stringPiped);

    if (piped){
        parseSpace(stringPiped[0], parsed);
        parseSpace(stringPiped[1], parsedPipe);
    } else{
        parseSpace(string, parsed);
    }

    if (ownCMDHandler(parsed)){return 0;}
    else {return 1 + piped;}
}
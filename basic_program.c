#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int status;

    printf("Parent process started...\n");

    pid = fork();

    if (pid == -1) {
        printf("Fork failed.\n");
        exit(1);
    } else if (pid == 0) {
        // Child process
        printf("Child process started...\n");

        // Execute the "ls" command using execvp
        char *args[] = {"ls", "-l", NULL};
        execvp("ls", args);

        // This line will only be reached if execvp fails
        printf("Exec failed.\n");
        exit(1);
    } else {
        // Parent process
        printf("Parent process waiting for child...\n");

        // Wait for the child to exit
        wait(&status);

        printf("Parent process finished.\n");
    }

    return 0;
}

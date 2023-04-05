#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed.\n");
        exit(1);
    } else if (pid == 0) {  // Child process
        printf("Child process started.\n");
        char *args[] = {"ls", "-l", NULL};
        execvpe(args[0], args, NULL);
        printf("This line will not be printed if execvpe() is successful.\n");
    } else {  // Parent process
        printf("Parent process waiting for child process to complete.\n");
        waitpid(pid, &status, 0);
        printf("Child process finished with status code %d.\n", status);
    }

    return 0;
}

# Hello!

# Welcome to my repository, featuring the Loggerhead Shell 🐢, a basic shell implemented through a C script that can handle piping and redirection, and basic terminal commands.

# Files 📁
basic_program.c -> for task 1.3
intermediate_program -> for task 1.3
loggerhead_shell.c -> my shell
Questions.txt -> answered questions for task 1.5
Task 1.5 ❓
Provide a concise and descriptive answer to the following questions.

## Q1: What is the purpose of the fork() system call?

📝 Answer1: The fork() system call creates a new process by duplicating the calling process. The new process is known as the child process, while the original process is known as the parent process. Both processes continue to execute the same code from the point at which the fork() call was made, but they have different memory spaces and process IDs. The purpose of fork() is to allow a single program to perform multiple tasks simultaneously, with each task running in its own process.

## Q2: Explain the following code snippet and write down the list of process state transitions that occur during the following program. You may assume that this is the only process that the CPU is executing.

int i = 1;
while (i < 100) { i++; }
printf("%d ", i);
while (i > 0) { i--; }
printf("%d ", i);

📝 Answer2: The given code snippet initializes an integer variable i with the value 1, and then runs two loops. The first loop increments i until it reaches 100, and the second loop decrements i until it reaches 0. Finally, the program prints the value of i twice: once after the first loop, and once after the second loop.

Assuming that this is the only process that the CPU is executing, the following process state transitions occur during the execution of this program:

The process starts in the "running" state.
The process enters a loop that increments the value of i until it reaches 100. During this time, the process is in the "running" state, executing instructions.
Once the loop completes, the process prints the value of i. During this time, the process is in the "running" state, executing the printf() function.
The process enters a second loop that decrements the value of i until it reaches 0. During this time, the process is in the "running" state, executing instructions.
Once the second loop completes, the process prints the value of i again. During this time, the process is in the "running" state, executing the printf() function.
The process reaches the end of the program and exits, returning to the operating system. During this time, the process is in the "terminated" state.

# Assignment outline 📋

This is a C program that creates a command-line shell, also known as a terminal emulator. It provides the user with an interface to enter commands and execute them on a Unix-like operating system. The script uses several libraries, including stdio.h, stdlib.h, string.h, unistd.h, sys/utsname.h, limits.h, sys/wait.h, fcntl.h, termios.h, errno.h, signal.h, libgen.h, and setjmp.h.

The script starts by defining a macro for STDIN_FILENO and the buffer size. It also defines a function to clear the shell using escape sequences.

The main() function initializes the shell 🐚, sets a signal handler for the SIGQUIT signal 🚩, and starts an infinite loop that repeatedly reads the user's input 🔄, tokenizes it into separate arguments 📄, and executes the appropriate shell command 💻. It also handles any errors that occur during execution 💥 and restarts the shell 🔄.

The init_shell() function prints a greeting 😊 and information about the shell during startup 🚀, including the logo of the shell 🐚. The takeInput() function takes input from the user 💬 and displays the current working directory 📁, hostname 🏠, and username 👤 in the prompt.

The tokenizeInput() function tokenizes the input into arguments 📄, splitting the input string based on whitespace characters 🔠. The executeShellCommand() function executes the shell commands and handles both built-in and external commands 💾. The handleBuiltInCommands() function handles the built-in shell commands such as "cd," "mv," "du," and "rev" 🛠️. The launchExternalCommands() function launches external programs 🚀.

The script also contains several other functions, including handle_signal() 🚨, reverse() 🔙, containsPipe() 🔍, containsRedirect() 🔜, executePipedCommand() 🌊, executeRedirectCommand() 🌐, handle_error() 💥, handleCD() 📂, handleMV() 📁, handleDU() 💾, handleREV() 🔙, handleHostName() 🏠, handleUName() 💻, help() ❓, and greetings() 😊. These functions handle various aspects of the shell, including signals 🚨, strings 🔤, pipes 🌊, redirects 🔜, errors 💥, and built-in commands 🛠️.

The script uses setjmp() and longjmp() functions to handle errors and restart the shell 🔄. The should_exit variable is used to determine whether the shell should exit or continue running 🛑.

Functions 🛠️:

1. main(int argc, char *argv[]): This is the main function of the program. It initializes the shell 🐚 and handles user input 💬. It also sets up signal handling for the SIGQUIT signal 🚩.

2. void init_shell(): This function clears the terminal ⚠️, displays a greeting message 😊, and sets up the environment for the shell 🚀.

3. void takeInput(char* string): This function takes input from the user 💬, displays the current working directory 📁 and the hostname 🏠 in the prompt.

4. int tokenizeInput(char* args[], int num_args, char* input): This function takes a string and tokenizes it into individual arguments 📄.

5. void executeShellCommand(char* command, char* arguments[]): This function executes shell commands entered by the user 💾.

6. void handleBuiltInCommands(char* args[], int num_args): This function handles built-in commands like cd, mv, du, rev, hostname, uname, help and exit 🛠️.

7. void launchExternalCommands(char* args[]): This function launches external commands, i.e. programs not built into the shell, using execvp 🚀.

8. void handle_signal(int signal): This function handles signals, specifically the SIGQUIT signal, which is used to exit the shell 🚨.

9. void reverse(char str[], int start, int end): This function reverses a string 🔙.

10 💻 int containsPipe(char** args, int num_args): This function checks whether the input string contains a pipe character |.

11. 💻 int containsRedirect(char** args, int num_args): This function checks whether the input string contains any of the redirect characters <, > or >>.

12. 💻 void executePipedCommand(char **args, int num_args): This function executes a command containing a pipe |.

13. 💻 void executeRedirectCommand(char **args, int num_args): This function executes a command containing a redirect <, > or >>.

14. 💥 void handle_error(jmp_buf env): This function handles errors in the program by jumping back to the start of the program.

15. 🔁 void restart(char* argv[], char* input): This function restarts the shell if the user enters the command restart.

16. 📁 void handleCD(char* args[], int num_args): This function handles the cd command, which changes the current working directory.

17. 📂 void handleMV(char* args[], int num_args): This function handles the mv command, which moves or renames files.

18. 💾 void handleDU(char* args[], int num_args): This function handles the du command, which shows disk usage statistics for files and directories.

19. 🔙 void handleREV(char* args[], int num_args): This function handles the rev command, which reverses the characters in a string.

20. 🖥️ void handleHostName(): This function handles the hostname command, which displays the name of the current host.

21. 💻 void handleUName(): This function handles the uname command, which displays system information.

22. ❓ void help(): This function displays help information for the shell.

23. 🌅 void greetings(): This function displays a greeting message when the shell starts up.


# 🎯 Separate tasks:

## 1️⃣ basic_program ->

👉 This is a C program that demonstrates how to create a new process using the fork() system call, and how to execute a different program using the execvp() system call. 🚀

👉 First, the program includes several header files: stdio.h for standard input/output functions, stdlib.h for exit functions, unistd.h for fork functions, and sys/wait.h for wait functions. 📚

👉 The main() function starts by declaring two variables: pid, which is of type pid_t (a process ID data type) and status, which is an integer used to store the exit status of the child process. 💻

👉 Next, the parent process prints a message indicating that it has started. Then, the fork() function is called. 📢 The fork() function creates a new process by duplicating the calling process. The parent process and the child process run concurrently after the fork() function returns. The fork() function returns different values for the parent process and the child process:

👉 For the parent process, fork() returns the process ID of the child process. 🆔 For the child process, fork() returns 0. The program checks the return value of fork() to determine whether it was successful. If fork() fails, it prints an error message and exits the program. ❌

👉 If fork() succeeds and the current process is the child process, it prints a message indicating that it has started. Then, it creates a new array of strings called args, which contains the command to be executed and its arguments. 📄 In this case, ls and -l are passed as arguments to the ls command. Then, the execvp() function is called to replace the child process's image with the ls command. 🚀 The execvp() function takes the name of the command and an array of strings as arguments. The first element of the array should be the name of the command to be executed, and the last element should be NULL. If execvp() fails, the program prints an error message and exits. ❌

👉 If fork() succeeds and the current process is the parent process, it prints a message indicating that it is waiting for the child process to finish. The wait() function is then called to wait for the child process to terminate. The wait() function returns the exit status of the child process, which is stored in the status variable. The parent process then prints a message indicating that it has finished. ✔️

👉 Finally, the program returns 0 to indicate successful execution. ✅

## 2️⃣ intermediate_program ->

👉 This is a C program that creates a child process using the fork() system call and then executes the "ls -l" command using the execvpe() system call. 🚀

👉 The program starts by declaring two variables, pid and status, of type pid_t and int, respectively. The pid_t type is used for process IDs and is defined in the <sys/types.h> header file. The int type is used for storing the exit status of the child process. 💻

👉 Next, the program creates a child process by calling the fork() system call. The fork() call creates a copy of the parent process, including all the memory, file descriptors, and program state, and then returns the process ID of the child process to the parent process and 0 to the child process. 📢

🌟 Next, our lovely program creates a child process by calling the fork() system call.
The fork() call magically creates a copy of the parent process, including all the memory, file descriptors, and program state, and then returns the process ID of the child process to the parent process and 0 to the child process.

Our program then checks the return value of fork() to determine whether the call was successful.
If fork() returns a negative value, it means the call failed, so our program kindly prints an error message and exits.
If fork() returns 0, it means the current process is the child process, so our program executes the "ls -l" command using the execvpe() system call.

In the child process, our program prints a message indicating that it has started its magic, and then creates an array of strings called args that contains the command to be executed and its arguments.
The execvpe() call takes three arguments: the name of the program to be executed (in this case, "ls"),
an array of strings containing the command and its arguments, and an environment variable array (NULL in this case).

If the execvpe() call is successful, the child process will execute the "ls -l" command and then vanish into thin air. However, if the call fails, our program prints an error message and exits with an error code. Sad, but we still love our program.

In the parent process, our program proudly announces that it is waiting for the child process to complete its task.
The waitpid() system call is used to wait for the child process to terminate and retrieve its exit status.
The waitpid() call takes three arguments: the process ID of the child process, a pointer to an integer variable to store the child process's exit status, and an options parameter (0 in this case).
Once the child process has vanished into thin air, our program prints a message indicating the child process's exit status, and then exits itself, feeling accomplished.

## Overall, our charming program demonstrates how to create a child process and execute a command in the child process using the fork() and execvpe() system calls in C. We're so proud of our program!

Instructions for running our program are easy-peasy: just run "gcc -o 🚀script_name🚀 📜script_name.c📜 -Wall". Voila!

# Sources/Tutorials:

GeeksForGeeks: https://www.geeksforgeeks.org/making-linux-shell-c/
StackOverflow

# Challenges:

We faced some challenges along the way, but we conquered them like the heroes we are! We struggled with piping at first, but then realized we were calling two functions separately, which should not be done. We also couldn't get readline() working without breaking after a certain amount of characters in the buffer, so we had to admit defeat. But hey, we're still learning and growing! We really wanted to get the history working, but maybe we'll tackle that when we're feeling more chill about deadlines. And, we didn't have time to comment the code as much as we would like to, but we promise to do so for our own sake :D.

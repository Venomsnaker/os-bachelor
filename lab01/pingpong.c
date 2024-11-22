#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    char buff[255]; // Buffer to hold the message
    int p[2]; // Pipe array, p[0] for reading, p[1] for writing
    pipe(p); // Create the pipe

    int pid = fork(); // Fork the process
    if (pid == 0){ // In child process
        printf("%d: received ping\n", getpid()); // Print the process ID of the child and message
        close(p[0]); // Close the read end of the pipe (child doesn't need it)
        write(p[1], "pong", 4); // Write the message "pong" to the write end of the pipe
        close(p[1]); // Close the write end of the pipe after writing
        exit(0); // Exit the child process
    }
    else if (pid > 0){ // In parent process
        wait(0); // Wait for the child process to finish
        printf("%d: received pong\n", getpid()); // Print the process ID of the parent and message
        close(p[1]); // Close the write end of the pipe (parent doesn't need it anymore)
        read(p[0], buff, 4); // Read 4 bytes from the pipe into the buffer
        buff[4] = '\0'; // Add the null terminator to mark the end of the string
        close(p[0]); // Close the read end of the pipe
    }
    else{ // If fork failed
        fprintf(2, "fork failed\n"); // Print an error message
        exit(1); // Exit with failure
    }
    exit(0); // Exit the parent process
}
#include "kernel/types.h"
#include "user/user.h"

int sieve(int in_fd, int first_prime);

int main(int argc, char *argv[])
{
    int THRESHOLD = 280; // Default value
    if (argc == 2) {
        THRESHOLD = atoi(argv[1]); // Set threshold from command-line argument
    }

    int p[2];
    if (pipe(p) < 0) {
        fprintf(2, "pipe creation failed\n");
        exit(1);
    }

    if (fork() == 0) { // Child process
        close(p[0]); // Close the read end of the pipe in the child
        for (int i = 2; i < THRESHOLD; ++i) {
            if (write(p[1], &i, sizeof(int)) != sizeof(int)) {
                fprintf(2, "write failed\n");
                exit(1);
            }
        }
        close(p[1]); // Close write end when done
        exit(0); // Exit child process
    }

    close(p[1]); // Parent closes write end of pipe

    int first_prime;
    while (read(p[0], &first_prime, sizeof(int)) == sizeof(int)) { // Read primes
        printf("prime %d\n", first_prime);
        p[0] = sieve(p[0], first_prime); // Call sieve to filter multiples
    }

    close(p[0]); // Close the read end of the pipe
    exit(0); // Exit parent process
}

int sieve(int in_fd, int first_prime)
{
    int out_pipe[2];
    if (pipe(out_pipe) < 0) {
        fprintf(2, "pipe creation failed\n");
        exit(1);
    }

    if (fork() == 0) { // Child process
        close(out_pipe[0]); // Close the read end of the output pipe in the child
        int num;
        while (read(in_fd, &num, sizeof(int)) == sizeof(int)) {
            if (num % first_prime != 0) {
                if (write(out_pipe[1], &num, sizeof(int)) != sizeof(int)) {
                    fprintf(2, "write failed\n");
                    exit(1);
                }
            }
        }

        close(in_fd); // Close the input pipe after done reading
        close(out_pipe[1]); // Close write end after done writing
        exit(0); // Exit child process
    }

    close(in_fd); // Parent closes input file descriptor
    close(out_pipe[1]); // Close write end of the output pipe

    return out_pipe[0]; // Return the new read end of the pipe for next sieve
}
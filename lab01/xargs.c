#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

int main(int argc, char *argv[]){
    int idx = 0;
    int argcCounter = 1;
    char *arguments[MAXARG]; // arguments[0] = command to be executed

    // Move idx to pos of the command to be executed, argv[0] == "xargs"
    if (strcmp(argv[1], "-n") == 0) {
        // argv[1] == "-n", argv[2] == var(Int)
        idx = 3;
    } else {
        idx = 1;
    }
    // Storing the command
    arguments[0] = malloc(strlen(argv[idx]) + 1);
    strcpy(arguments[0], argv[idx]);

    // Collect remaining arguments
    for (int i = idx + 1; i < argc; i++) {
        arguments[argcCounter] = malloc(strlen(argv[i]) + 1);
        strcpy(arguments[argcCounter++], argv[i]);
    }

    // Allocate memory for lines from stdin
    arguments[argcCounter] = malloc(256);
    char charBuffer;
    int i = 0;

    while (read(0, &charBuffer, 1)) {
        if (charBuffer == '\n') {
            // New input string when encourter newline character
            arguments[argcCounter][i++] = '\0';

            // Create process
            if (fork() == 0) {
                // exec is kind odd, it requires arguments[0] == command
                exec(argv[idx], arguments);
            } else {
                i = 0;
                wait(0);
            }
        } else {
            arguments[argcCounter][i++] = charBuffer;
        }
    }
    exit(0);
}
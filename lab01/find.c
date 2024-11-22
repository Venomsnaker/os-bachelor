#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


void searchDir(char *path, char *buf, int fd, struct stat st, char *filename) {
    char *p; 
    struct dirent de; //direct entry 
    struct stat st2; // detail about path

    // Check if path is too long
    if (strlen(path) + 1 + DIRSIZ + 1 > 512) {
        printf("find: path too long\n");
        return;
    }
    //write path to buffer
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    // Iterate through directory 
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
            continue; // Skip empty entries

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0; // Null-terminate the string
        // Check status of path
        if (stat(buf, &st2) < 0) {
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        // If the path is a directory, search through it, ignoring parent and recurrent entries
        if (st2.type == T_DIR) {
            if (strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0) {
                int fd2 = open(buf, 0);
                if (fd2 < 0) {
                    printf("find: cannot open %s\n", buf);
                    continue;
                }
                searchDir(buf, buf, fd2, st2, filename); // search the diretory recursively
                close(fd2);
            }
        } else if (st2.type == T_FILE) {
            if (strcmp(de.name, filename) == 0) {
                printf("%s\n", buf); //If filename matches, file print path
            }
        }
    }
}

void find(char *path, char *filename) {
    char buf[512];//Create buffer to store path
    int fd; // File dicriptor of given path
    struct stat st; // Details about path

    // Path must be a directory
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // Check status of directory
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    // If path is a directory
    if (st.type == T_DIR) {
        searchDir(path, buf, fd, st, filename);
    }
    
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("find: requires a file name to search for\n");
        exit(1);
    } else if (argc == 2) { // If no starting-point specified, '.' is assumed
        find(".", argv[1]);
    } else {
        if (argc != 3) {
            printf("find: requires a directory and a file name to search for\n");
            exit(1);
        }
        // Call find function with directory from argv[1] and file name in argv[2]
        find(argv[1], argv[2]);
    }
    exit(0);
}
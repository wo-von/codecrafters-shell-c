#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define TKN_SIZE 50
#define TKN_NBR 100
#define PATH_SIZE 200

char *builtin[] = {"echo", "exit", "type"};

int parse_input(char input[], char array[][TKN_SIZE], char delim);

void builtin_type(char input_array[][TKN_SIZE], char *path);

void handle_cmd(char input_array[][TKN_SIZE], char *path);

void builtin_type(char input_array[][TKN_SIZE], char *path) {
    // First we check if it is a builtin
    // char found = 0;
    for (int i = 0; i < sizeof(builtin) / sizeof(char *); i++) {
        if (strcmp(input_array[1], builtin[i]) == 0) {
            printf("%s is a shell builtin\n", builtin[i]);
            return;
        }
    }

    // then we check if it is an executable in PATH
    char *path_dup = strdup(path);
    char *delim;
    if (strchr(path_dup, ':')) {
        delim = ":";
    } else {
        delim = ";";
    }
    char full_path[1024];
    struct stat st;
    char *token = strtok(path_dup, delim);

    while (token != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", token, input_array[1]);
        if (!stat(full_path, &st)) {
            if (!access(full_path, X_OK)) {
                // it exists and is executable
                printf("%s is %s\n", input_array[1], full_path);
                free(path_dup);
                return;
            }
        }
        token = strtok(NULL, delim);
    }
    printf("%s not found\n", input_array[1]);
    free(path_dup);
    return;
}

// Runs the non-built in programs with the given args
// if they exist and are in PATH
// if not .. TODO
void handle_cmd(char input_array[][TKN_SIZE], char *path) {
    // then we check if it is an executable in PATH
    char *path_dup = strdup(path);
    char *delim;
    if (strchr(path_dup, ':')) {
        delim = ":";
    } else {
        delim = ";";
    }
    char full_path[1024];
    struct stat st;
    char *token = strtok(path_dup, delim);

    while (token != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", token, input_array[0]);
        if (!stat(full_path, &st)) {
            if (!access(full_path, X_OK)) {
                // it exists and is executable
                pid_t pid = fork();
                if (pid == 0) {
                    // child
                    // hand control to the kernel, replace with new program
                    execve(full_path, input_array, env);
                } else {
                    // parent
                }
                free(path_dup);
                return;
            }
        }
        token = strtok(NULL, delim);
    }
    printf("%s not found\n", input_array[1]);
    free(path_dup);
    return;
}
// Reads the path, parses it into an array of strings, with the last element
// NULL and returns the length (including NULL)
// return values: 0 path not set
// return > 0 : number of elements of the path
int parse_path(char *path, char path_array[][PATH_SIZE]) {
    char *path = getenv("PATH");
    if (!path) {
        printf("path is not set\n");
        return 0;
    }
    char *delim;
    if (strchr(path, ':')) {
        delim = ":";
    } else {
        delim = ";";
    }
    char *token = strtok(path, delim);
    int index = 0;
    int j = 0;
    for (int i = 0; path[i] != '\0'; i++) {
        if (path[i] == delim) {
            path_array[index][j] = '\0';
            index++;
            j = 0;
        } else {
            path_array[index][j] = path[i];
            j++;
        }
    }

    // Null-terminate the last token if any
    if (j > 0) {
        path_array[index][j] = '\0';
        index++;
    }
    // The last element of the array of pointers should be NULL, so that
    // execve works
    path_array[index][0] = NULL;
    return index;
}

// parses the input by the user into an array of strings
// returns the number of tokens
// ToDo: try VLA
int parse_input(char input[], char array[][TKN_SIZE], char delim) {
    int index = 0, j = 0;

    // ToDo: initial spaces and multiple spaces should be trimmed
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] == delim) {
            array[index][j] = '\0';
            index++;
            j = 0;
        } else {
            array[index][j] = input[i];
            j++;
        }
    }

    // Null-terminate the last token if any
    if (j > 0) {
        array[index][j] = '\0';
        index++;
    }
    // The last element of the array of pointers should be NULL, so that
    // execve works
    array[index][0] = NULL;
    return index;
}
int main(int argc, char *argv[]) {
    // Flush after every printf
    setbuf(stdout, NULL);
    char input[1024];
    char input_array[TKN_NBR][TKN_SIZE];
    char path_array[TKN_NBR][PATH_SIZE];
    char *path = getenv("PATH");
    int path_length = parse_path(path, path_array);
    if (path_length == 0) {
        printf("path is not set\n");
        return 1;
    }

    while (1) {
        printf("$ ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        input[strcspn(input, "\n")] = '\0';

        int len = parse_input(input, input_array, ' ');

        if (strcmp(input_array[0], "type") == 0) {
            builtin_type(input_array, path);
        } else if (strcmp(input_array[0], "exit") == 0) {
            break;
        } else if (strcmp(input_array[0], "echo") == 0) {
            for (int i = 1; i < len; i++) {
                printf("%s ", input_array[i]);
            }

            printf("\n");
        } else {
            printf("%s: command not found\n", input_array[0]);
        }
    }

    return 0;
}

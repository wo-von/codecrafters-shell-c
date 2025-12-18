#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define TKN_SIZE 50
#define TKN_NBR 100

char *builtin[] = {"echo", "exit", "type"};

int parse_input(char input[], char array[][TKN_SIZE], char delim);

void builtin_type(char input_array[][TKN_SIZE], char *path);

void builtin_type(char input_array[][TKN_SIZE], char *path)
{
	// First we check if it is a builtin
	// char found = 0;
	for (int i = 0; i < sizeof(builtin) / sizeof(char*); i++) {
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

// parses the input by the user into an array of strings
int parse_input(char input[], char array[][TKN_SIZE], char delim)
{
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

	return index;
}
int main(int argc, char* argv[])
{
	// Flush after every printf
	setbuf(stdout, NULL);
	char input[1024];
	char input_array [TKN_NBR][TKN_SIZE];
	char *path = getenv("PATH");
	if (!path) {
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
			for (int i = 1; i < len; i ++) {
				printf("%s ", input_array[i]);
			}

			printf("\n");
		} else {
			printf("%s: command not found\n", input_array[0]);
		}
	}

	return 0;
}

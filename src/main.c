#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TKN_SIZE 50
#define TKN_NBR 100

int parse_input(char input[], char array[][TKN_SIZE], int input_size);

int parse_input(char input[], char array[][TKN_SIZE], int input_size)
{
	char delim = ' ';
	int index = 0, j = 0;

	// ToDo: initial spaces and multiple spaces should be trimmed
	for (int i = 0; i < input_size; i++) {
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


	while (1) {
		printf("$ ");
		fgets(input, sizeof(input), stdin);
		input[strcspn(input, "\n")] = '\0';

		int len = parse_input(input, input_array, strlen(input));

		if (strcmp(input_array[0], "exit") == 0) {
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

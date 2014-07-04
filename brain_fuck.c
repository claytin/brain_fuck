#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define PROG_DATA_SIZE 250
#define DEBUG_OUTPUT_SIZE 100
#define PROG_BUF_BLOCK 1

void printdebug(char *program, int *program_data,
	unsigned int data_pos, unsigned int prog_pos, char *output);
void printusage(char *cmd);
char* loadprogram(char *path);

//debug stuff
unsigned int break_point = 0, break_step = 0;

int main(int argc, char **argv){
	bool debug = false, path_set = false;
	char *prog_file_path;

	//arg stuff
	if(argc >= 2){
		int i;
		for(i = 1; i < argc; i++){
			if(argv[i][0] == '-'){
				switch(argv[i][1]){
					case 'd':
						debug = true;
						break;
					case 'b':
						break_point = strtol(argv[i + 1], NULL, 10);
						i++;	//don't read next arg
						break;
					case 's':
						break_step = strtol(argv[i + 1], NULL, 10);
						i++;
						break;
				}

			}else{
				prog_file_path = argv[i];
				path_set = true;
			}
		}
	}else{
		fprintf(stderr, "must at least specify a file\n");
		printusage(argv[0]);
		return 0;
	}

	//load from file
	char *program;
	if(path_set){
		program = loadprogram(prog_file_path);
		if(program == NULL){
			return 1;
		}
	}else{
		fprintf(stderr, "must specify a file\n");
		printusage(argv[0]);
		return 1;
	}


	unsigned int cur_cmd = 0;
	int *prog_data;

	//allocate space for program to
	prog_data = (int*)malloc(sizeof(int*) * PROG_DATA_SIZE);

	//more debug stuff
	int *all_data;
	all_data = prog_data;
	char *output;
	unsigned int char_index = 0;
	if(debug)
		output = (char*)malloc(sizeof(char) * DEBUG_OUTPUT_SIZE);

	//the main stuff
	while(cur_cmd < strlen(program)){
		if(debug)
			printdebug(program, all_data, (prog_data - all_data),
				cur_cmd, output);
		
		switch (program[cur_cmd]){
			case '+':
				(*prog_data)++;
				break;
			case '-':
				(*prog_data)--;
				break;
			case '>':
				prog_data++;
				break;
			case '<':
				prog_data--;
				break;
			case '.':
				putchar(*prog_data);
				if(debug){
					output[char_index] = *prog_data;
					char_index++;
				}
				break;
			case ',':
				*prog_data = getchar();
				break;
			case '[':
				if(*prog_data == 0){
					int jump_depth = 0;
					cur_cmd++;
					while(program[cur_cmd] != ']' || jump_depth != 0){
						if(program[cur_cmd] == ']'){
							jump_depth--;
						}else if(program[cur_cmd] == '['){
							jump_depth++;
						}
						cur_cmd++;
					}
				}
				break;
			case ']':
				if(*prog_data != 0){
					int jump_depth = 0;
					cur_cmd--;
					while(program[cur_cmd] != '[' || jump_depth != 0){
						if(program[cur_cmd] == ']'){
							jump_depth++;
						}else if(program[cur_cmd] == '['){
							jump_depth--;
						}
						cur_cmd--;
					}
				}
				break;
			default:
				break;
		}

		cur_cmd++;
	}
	//mmmmm hmmmm
	if(debug){
		printdebug(program, all_data, (prog_data - all_data),
			cur_cmd, output);
		printf("program ended\n");
	}

	return 0;
}



char* loadprogram(char *path){
	FILE *prog_file;
	prog_file = fopen(path, "r");

	if(prog_file == NULL){
		fprintf(stderr, "program file %s could not be opened\n", path);
		return NULL;
	}

	int buff_blocks = 1, c, pos = 0;
	char *file_string;
	file_string = (char*)malloc(sizeof(char) * PROG_BUF_BLOCK * buff_blocks);

	while((c = getc(prog_file)) != EOF){
		switch(c){	//only allow program characters
			case '-': case '+': case '>': case '<':
			case '[': case ']': case ',': case '.':
			file_string[pos] = c;
			pos++;

			if((buff_blocks * PROG_BUF_BLOCK) < (pos + 1)){
				file_string = (char*)realloc(file_string, sizeof(char*) *
					PROG_BUF_BLOCK * buff_blocks);
				buff_blocks++;
			}
			break;
		}
	}
	return file_string;
}

void printdebug(char *program, int *program_data,
unsigned int data_pos, unsigned int prog_pos, char *output){
	static unsigned int steps;
	//clear screen
	printf("\033[2J\033[1;1H");
	fprintf(stdout, "step: %i\n", steps);
	fprintf(stdout, "data:\t");
	unsigned int i;
	//print prog data
	for(i = 0; i < PROG_DATA_SIZE; i++){
		fprintf(stdout, "%3i ", program_data[i]);
	}

	//print arrows
	//unsigned int arrow_max = (data_pos > prog_pos ? data_pos : prog_pos);
	fprintf(stdout, "\n\t  ");
	for(i = 0; i <= data_pos; i++){
		if(data_pos == i){
			fprintf(stdout, "↑");
		}else{
			fprintf(stdout, "    ");
		}
	}
	
	fprintf(stdout, "\n\t");
	for(i = 0; i <= prog_pos; i++){
		if(prog_pos == i){
			fprintf(stdout, "↓");
		}else{
			fprintf(stdout, " ");
		}
	}
	
	//print program
	fprintf(stdout, "\nprog:\t");
	for(i = 0; i < strlen(program); i++){
		if(i == prog_pos){
			fprintf(stdout, "|");
		}
		fprintf(stdout, "%c", program[i]);
	}

	//show output
	fprintf(stdout, "END\noutput:\t%s", output);

	if(break_step == 0 && break_point == 0){
		fprintf(stdout, "\nENTER: step");
		getchar();
	}else if(break_step < steps + 2){
		break_step = 0;
	}

	steps++;

	//sleep(1);
}

void printusage(char *cmd){
	fprintf(stdout, "usage: %s [options] file\noptions:\n"
	"  -h  --help\tyep\n"
	"  -d \t\trun in debug mode\n"
	"debug options:\n"
	"  -s <step>\tbreak at step (overrides -b)\n"
	"  -b <point>\tput break point at position, if omitted\n"
	"\t\tbreakpoin will be set at position 0\n"
	, cmd);
}

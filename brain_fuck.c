#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define DEFAULT_PROG_DATA_SIZE 250
#define DEBUG_OUTPUT_SIZE 100
#define PROG_BUF_BLOCK 16

struct options{
	bool do_nothing_at_eof;
	int eof_value;

	bool source_from_args;
	bool source_from_stdin;
	bool path_set;
	char *file_path;
	char *source;

	bool allow_negative;
	bool debug_mode;
	bool output_and_exit;
}program;

void printdebug(long int *program_data,
	unsigned int data_pos, unsigned int prog_pos, char *output);
void printusage(char *cmd);
char* load_program(FILE * programfile);
int parseargs(int argc, char **argv);

//debug stuff
unsigned int break_point = 0, break_step = 0;
unsigned long int prog_data_size = DEFAULT_PROG_DATA_SIZE;

int main(int argc, char **argv){

	//arg stuff (this could be nicer)
	if(argc >= 2){
		if(parseargs(argc, argv)){
			return 1;
		}
	}else{
		fprintf(stderr, "must at least specify a file\n");
		printusage(argv[0]);
		return 0;
	}

	//load from file
	if(program.path_set && !program.source_from_args && !program.source_from_stdin){
		FILE * program_file = fopen(program.file_path, "r");
		if(program_file == NULL){
			fprintf(stderr, "program file %s could not be opened\n", program.file_path);
			return 1;
		}
		program.source = load_program(program_file);
		if(program.source == NULL){
			return 1;
		}
	}else if(program.source_from_stdin){
		program.source = load_program(stdin);
	}else if(program.source_from_args){

	}else{
		fprintf(stderr, "must specify a file\n");
		printusage(argv[0]);
		return 1;
	}

	if(program.output_and_exit){
		printf("%s", program.source);
		return 0;
	}


	unsigned int cur_cmd = 0;
	unsigned int remaining_cells = prog_data_size;
	long int *prog_data;

	//allocate space for program to
	prog_data = (long int*)malloc(sizeof(long int*) * prog_data_size);

	//more debug stuff
	long int *all_data;
	all_data = prog_data;
	char *output;
	unsigned int char_index = 0;
	if(program.debug_mode)
		output = (char*)malloc(sizeof(char) * DEBUG_OUTPUT_SIZE);

	//the main stuff
	while(cur_cmd < strlen(program.source)){
		if(program.debug_mode){
			printdebug(all_data, (prog_data - all_data),
				cur_cmd, output);
		}
		
		switch (program.source[cur_cmd]){
			case '+':
				(*prog_data)++;
				break;
			case '-':
				if(!program.allow_negative && *prog_data == 0){
					fprintf(stderr, "error: negative value not allowed\n");
					return 0;
				}
				(*prog_data)--;
				break;
			case '>':
				if(remaining_cells == 1){
					fprintf(stderr, "error: cannot move any farther right\n");
					return 1;
				}
				prog_data++;
				remaining_cells--;
				break;
			case '<':
				if(remaining_cells >= prog_data_size){
					fprintf(stderr, "error: cannot move any farther left\n");
					return 1;
				}
				remaining_cells++;
				prog_data--;
				break;
			case '.':
				putchar(*prog_data);
				if(program.debug_mode){
					output[char_index] = *prog_data;
					char_index++;
				}
				break;
			case ',':
				//for variable scope stuff
				{
					//eof stuff
					int inchar = getchar();
					if(inchar == EOF && !program.do_nothing_at_eof){
						*prog_data = program.eof_value;
					}else{
						*prog_data = getchar();
					}
					break;
				}
			case '[':
				if(*prog_data == 0){
					int jump_depth = 0;
					cur_cmd++;
					while(program.source[cur_cmd] != ']' || jump_depth != 0){
						if(program.source[cur_cmd] == ']'){
							jump_depth--;
						}else if(program.source[cur_cmd] == '['){
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
					while(program.source[cur_cmd] != '[' || jump_depth != 0){
						if(program.source[cur_cmd] == ']'){
							jump_depth++;
						}else if(program.source[cur_cmd] == '['){
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
	if(program.debug_mode){
		printdebug(all_data, (prog_data - all_data),
			cur_cmd, output);
		printf("program ended\n");
	}

	return 0;
}



char* load_program(FILE * prog_file){
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

void printdebug(long int *program_data,
unsigned int data_pos, unsigned int prog_pos, char *output){
	static unsigned int steps;
	//clear screen
	printf("\033[2J\033[1;1H");
	fprintf(stdout, "step: %i\n", steps);
	fprintf(stdout, "data:\t");
	unsigned int i;
	//print prog data
	for(i = 0; i < prog_data_size; i++){
		fprintf(stdout, "%3li ", program_data[i]);
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
	for(i = 0; i < strlen(program.source); i++){
		if(i == prog_pos){
			fprintf(stdout, "|");
		}
		fprintf(stdout, "%c", program.source[i]);
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

int parseargs(int argc, char **argv){
	int i;
	for(i = 1; i < argc; i++){
		if(strcmp(argv[i], "--help") == 0
		|| strcmp(argv[i], "-h") == 0){
			printusage(argv[0]);
			return 1;
		}else if(strcmp(argv[i], "--debug") == 0
		|| strcmp(argv[i], "-d") == 0){
			program.debug_mode = true;
		}else if(strcmp(argv[i], "--eof") == 0
		|| strcmp(argv[i], "-e") == 0){
			program.do_nothing_at_eof = false;
			program.eof_value = (int)strtol(argv[i + 1], NULL, 10);
			i++;
		}else if((strcmp(argv[i], "--prog") == 0
		|| strcmp(argv[i], "-p") == 0) && !program.source_from_stdin){
			program.source_from_args = true;
			program.source = argv[i + 1];
			i++;
		}else if(strcmp(argv[i], "--mem") == 0
		|| strcmp(argv[i], "-m") == 0){
			prog_data_size = (int)strtol(argv[i + 1], NULL, 10);
			i++;
		}else if(strcmp(argv[i], "--noneg") == 0
		|| strcmp(argv[i], "n") == 0){
			program.allow_negative = false;
		}else if(strcmp(argv[i], "--step") == 0
		|| strcmp(argv[i], "-s") == 0){
			break_step = (int)strtol(argv[i + 1], NULL, 10);
			i++;
		}else if(strcmp(argv[i], "--break") == 0
		|| strcmp(argv[i], "-b") == 0){
			break_point = (int)strtol(argv[i + 1], NULL, 10);
			i++;
		}else if(strcmp(argv[i], "--stdin") == 0
		|| strcmp(argv[i], "-i") == 0){
			program.source_from_stdin = true;
		}else if(strcmp(argv[i], "--output") == 0
		|| strcmp(argv[i], "-o") == 0){
			program.output_and_exit = true;
		}else if(!program.source_from_args && !program.source_from_stdin){
			program.file_path = argv[i];
			program.path_set = true;
		}
	}
	return 0;
}

void printusage(char *cmd){
	fprintf(stdout, "usage: %s [options] [file | -p]\noptions:\n\n"
	"  -h  --help              yep\n"
	"  -d  --debug             run in debug mode\n"
	"  -e  --eof   <num>       value to set when eof (default is to do nothing)\n"
	"  -p  --prog <prog_str>   get program from argument instead of file\n"
	"  -m  --mem  <cells | 0>  how many cells to allocate (default 256)\n"
	"  -n  --noneg             don't allow cells to be negative\n"
	"  -o  --output            output stripped program and exit\n"
	"  -i  --stdin             get program from stdin until EOF\n"
	"\ndebug options:\n"
	"  -s  --step  <step>      break at step (overrides -b)\n"
	"  -b  --break <point>     put break point at position\n"
	"\nauthor:\n"
	"  fuck\n"
	, cmd);
}

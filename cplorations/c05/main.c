/****************************************
 * C-ploration 5 for CS 271
 * 
 * [NAME] Hunter Scholz
 * [TERM] FALL 2024
 * 
 ****************************************/
#include "parser.h"

int main(int argc, const char *argv[]) {
	
	if(argc != 2){
		printf("Usage: %s [filename]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	FILE *fin = fopen(argv[1], "r");
	if(fin == NULL){
		perror("Unable to open file!");
		exit(EXIT_FAILURE);
	}
	
	parse(fin);
	fclose(fin);
				
}



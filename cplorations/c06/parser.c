/****************************************
 * C-ploration 6 for CS 271
 * 
 * [NAME] Hunter Scholz
 * [TERM] FALL 2024
 * 
 ****************************************/
#include "parser.h"

/* Function: strip
 * -------------
 * remove whitespace and comments from a line
 *
 * s: the char* string to strip
 *
 * returns: the stripped char* string
 */
char *strip(char *s){	
	char s_new[strlen(s)+1];
	int i = 0;
	
	for (char *s2 = s; *s2; s2++) {
		if(*s2 == '/' && *(s2+1) == '/'){
			break;
		} else if(!isspace(*s2)){
			s_new[i++] = *s2;			
		}
	}
	s_new[i] = '\0';
	strcpy(s, s_new);
	
    return s;	
}

/* Function: parse
 * -------------
 * iterate each line in the file and strip whitespace and comments. 
 *
 * file: pointer to FILE to parse
 *
 * returns: nothing
 */
void parse(FILE * file){
	char line[MAX_LINE_LENGTH] = {0};
	char label [MAX_LABEL_LENGTH];
	unsigned int line_num = 0;
	
	while (fgets(line, sizeof(line), file)){
		line_num++;
		strip(line);
		if(!*line){
			continue;
		}
		char inst_type;
		if(is_Atype(line)){
			inst_type = 'A';
		} else if(is_Ctype(line)){
			inst_type = 'C';
		} else if(is_label(line)){
			inst_type = 'L';
			extract_label(line, label);
			strcpy(line, label);
		}
		printf("%c  %s\n",inst_type, line);
	}
}

bool is_Atype(const char *line){
	return line[0] == '@';
}

bool is_label(const char *line){
	int i = 0;
	while(line[i] != '\0'){
		i++;
	}
	return line[0] == '(' && line[i - 1] == ')';
}

bool is_Ctype(const char *line){
	return !is_Atype(line) && !is_label(line);
}

char *extract_label(const char *line, char* label){
	int i = 0;
	while(line[i] != ')' && line[i] != '\0'){
		i++;
	}
	strncpy(label, line+1, i-1);
	label[i-1] = '\0';
	return label;
}

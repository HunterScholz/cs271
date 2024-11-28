/****************************************
 * C-ploration 9 for CS 271
 * 
 * [NAME] Hunter Scholz
 * [TERM] FALL 2024
 * 
 ****************************************/
#include "parser.h"
#include "symtable.h"
#include "error.h"

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
	unsigned int instr_num = 0;
	
	//Time Travel Symbol Finder Loop
	add_predefined_symbols();
	while (fgets(line, sizeof(line), file)){
		line_num++;
		strip(line);
		if(!*line){
			continue;
		}
		if(is_label(line)){
			strcpy(line, extract_label(line, label));
			if(isalpha(label[0]) == 0){
				exit_program(EXIT_INVALID_LABEL, line_num, line);
			} else if(symtable_find(label) != NULL){
				exit_program(EXIT_SYMBOL_ALREADY_EXISTS, line_num, line);
			}
			symtable_insert(label, instr_num);
		} else {instr_num++;}
	}
	
	rewind(file);
    line_num = 0;
    instr_num = 0;
	
	while (fgets(line, sizeof(line), file)){
		line_num++;
		char instr_type;
		strip(line);
		if(!*line || is_label(line)){
			continue;
		}
		instruction instr;
		if(is_Atype(line)){
			instr_type = 'A';
			if(!parse_A_instruction(line, &instr.instr.a)){
				exit_program(EXIT_INVALID_A_INSTR, line_num, line);
			}
			instr.field = Atype;
		} else if(is_Ctype(line)){
			instr_type = 'C';
			instr.field = Ctype;
		}
		//printf("%u: %c  %s\n", instr_num, instr_type, line);
		instr_num++;
		if(instr_num > MAX_INSTRUCTIONS){
			exit_program(EXIT_TOO_MANY_INSTRUCTIONS, MAX_INSTRUCTIONS + 1);
		}
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

void add_predefined_symbols(){
	for(int i = 0; i < NUM_PREDEFINED_SYMBOLS; i++){
		symtable_insert((char*)predefined_symbols[i].name, (int)predefined_symbols[i].address);
	}
}

bool parse_A_instruction(const char *line, a_instruction *instr) {
    char* s = malloc(strlen(line)*sizeof(line));
	strcpy(s, line+1);
    char *s_end = NULL;
    long result = strtol(s, &s_end, 10);
    if (*s_end == '\0') {
        if (result < 0 || result > MAX_HACK_ADDRESS) {return false;}
        instr->type.address = result;
        instr->is_addr = true;
    } else {
        if (symtable_find(s) != NULL) {
            instr->type.address = symtable_find(s)->addr;
            instr->is_addr = true;
        } else if (isalpha(s[0])) {
            instr->type.label = malloc(strlen(line)*sizeof(char));
            instr->is_addr = false;
        } else {return false;}
    }
    return true;
}

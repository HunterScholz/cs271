/****************************************
 * Project 6 for CS 271
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
int parse(FILE * file, instruction *instructions){
	char line[MAX_LINE_LENGTH] = {0};
	char label [MAX_LABEL_LENGTH];
	unsigned int line_num = 0;
	unsigned int instr_num = 0;
	char temp_line [MAX_LINE_LENGTH];
	
	//Time Travel Symbol Finder
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
		if(!*line || is_label(line)){continue;}
		instruction instr;
		if(is_Atype(line)){
			if(!parse_A_instruction(line, &instr.instr.a)){
				exit_program(EXIT_INVALID_A_INSTR, line_num, line);
			}
			instr_type = 'A';
			instr.field = Atype;
			if(instr.instr.a.is_addr){
				//printf("A: %d\n", instr.instr.a.type.address);
			} else {
				//printf("A: %s\n", instr.instr.a.type.label);
			}
		} else if(is_Ctype(line)){
			instr_type = 'C';
			instr.field = Ctype;
			strcpy(temp_line, line);
			parse_C_instruction(temp_line, &instr.instr.c);
			
			if(instr.instr.c.dest == DEST_INVALID){exit_program(EXIT_INVALID_C_DEST, line_num, line);}
			if(instr.instr.c.jump == JMP_INVALID){exit_program(EXIT_INVALID_C_JUMP, line_num, line);}
			if(instr.instr.c.comp == COMP_INVALID){exit_program(EXIT_INVALID_C_COMP, line_num, line);}
			
			//printf("C: d=%d, c=%d, j=%d\n", instr.instr.c.dest, instr.instr.c.comp, instr.instr.c.jump);
		}
		//printf("%u: %c  %s\n", instr_num, instr_type, line);
		instructions[instr_num++] = instr;
		if(instr_num > MAX_INSTRUCTIONS){
			exit_program(EXIT_TOO_MANY_INSTRUCTIONS, MAX_INSTRUCTIONS + 1);
		}
	}
	return instr_num;
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
	
	if (s == s_end){
        instr->type.label = (char*)malloc(strlen(line));
        strcpy(instr->type.label, s);
        instr->is_addr = false;
    }else if (*s_end != 0){return false;}
	else{
        instr->type.address = result;
        instr->is_addr = true;
    }
    return true;
}

void parse_C_instruction(char *line, c_instruction *instr){
	char* token;
	char* jump;
	char* dest;
	char* comp;
	
	token = strtok(line, ";");
	jump = strtok(NULL, ";");
	dest = strtok(token, "=");
	comp = strtok(NULL, "=");
	if(comp == NULL){
		comp = dest;
		dest = NULL;
	}
	
	int a;
	instr->jump = jump ? str_to_jumpid(jump) : JMP_NULL;
	instr->dest = dest ? str_to_destid(dest) : DEST_NULL;
	instr->comp = str_to_compid(comp, &a);
	instr->a = a;
}

void assemble(const char * file_name, instruction* instructions, int num_instructions){	
	const char* hack = ".hack";
    FILE* fin = fopen(file_name, "r");
    size_t len = strlen(file_name) + strlen(hack) + 1;
    char* output_file_name = malloc(len * sizeof(char)); 
    strcpy(output_file_name, file_name);
    strcat(output_file_name, hack);
    FILE* fout = fopen(output_file_name,"w");
	
	opcode op = 0;
	int new = 16;
	for(int i = 0 ; i < num_instructions ; i++){
		if(instructions[i].field == Atype){
            if(instructions[i].instr.a.is_addr){op = instructions[i].instr.a.type.address;}
            else{
				if(symtable_find(instructions[i].instr.a.type.label) == NULL){
                    op = new;
                    symtable_insert(instructions[i].instr.a.type.label, new);
                    new++;
                }else{op = instructions[i].instr.a.type.address;}
                free(instructions[i].instr.a.type.label);
            }
        }
        else if(instructions[i].field == Ctype){op = instruction_to_opcode(instructions[i].instr.c);}
		fprintf(fout, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", OPCODE_TO_BINARY(op));
	}
	fclose(fout);
	fclose(fin);
}

/*
void assemble(const char * file_name, instruction* instructions, int num_instructions){
    const char* suffix = ".hack";
    
    FILE* fin = fopen(file_name, "r");
    size_t len = strlen(file_name) + strlen(suffix) + 1;
    char* output_file_name = malloc(len * sizeof(char)); 
    strcpy(output_file_name, file_name);
    strcat(output_file_name, suffix);
    FILE* fout = fopen( output_file_name,"w");
    
    opcode op = 0;
    int new = 16;
    for(int i = 0; i < num_instructions; i++){
        if(instructions[i].instruction_type == AType){
            if(instructions[i].instr.a.is_addr){
                op = instructions[i].instr.a.type.address;
            }
            else{
                if(symtable_find(instructions[i].instr.a.type.label) == NULL){
                    op = new;
                    symtable_insert(instructions[i].instr.a.type.label, new);
                    new++;
                }
                else{
                    op = instructions[i].instr.a.type.address;
                }
                free(instructions[i].instr.a.type.label);
            }
        }
        else if(instructions[i].instruction_type == CType){
            op = instruction_to_opcode(instructions[i].instr.c);
        }
        fprintf(fout, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", OPCODE_TO_BINARY(op));
        
    }
    fclose(fout);
    fclose(fin);
}
﻿*/

opcode instruction_to_opcode(c_instruction instr){
	opcode op = 0;
	op |= (7 << 13);
	op |= (instr.a << 12);
	op |= (instr.comp << 6);
	op |= (instr.dest << 3);
	op |= (instr.jump << 0);
	
	return op;
}



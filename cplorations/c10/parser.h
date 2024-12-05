/****************************************
 * C-ploration 10 for CS 271
 * 
 * [NAME] Hunter Scholz
 * [TERM] FALL 2024
 * 
 ****************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include "hack.h"

#define MAX_LINE_LENGTH  200
#define MAX_LABEL_LENGTH  MAX_LINE_LENGTH - 2
#define MAX_HACK_ADDRESS INT16_MAX
#define MAX_INSTRUCTIONS MAX_HACK_ADDRESS

#ifndef __PARSER_H__
#define __PARSER_H__

/** function prototypes **/
char *strip(char *s);
typedef int16_t hack_addr;
typedef int16_t opcode;

enum instr_type {
	Invalid = -1,
	Atype,
	Ctype,
};

typedef struct c_instruction {
	opcode a:1;
	opcode comp:7;
	opcode dest:4;
	opcode jump:4;
} c_instruction;

typedef struct a_instruction {
	union {
		hack_addr address;
		char * label;
	}type;
	bool is_addr;
} a_instruction;

typedef struct instruction {
	union {
		a_instruction a;
		c_instruction c;
	}instr;
	enum instr_type field;
} instruction;

int parse(FILE * file, instruction *instructions);

bool is_Atype(const char *);
bool is_label(const char *);
bool is_Ctype(const char *);

char *extract_label(const char *line, char* label);

void add_predefined_symbols();
bool parse_A_instruction(const char *line, a_instruction *instr);

void parse_C_instruction(char *line, c_instruction *instr);

#endif
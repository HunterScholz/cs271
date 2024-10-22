/****************************************
 * C-ploration 4 for CS 271
 * 
 * [NAME] Hunter Scholz
 * [TERM] FALL 2024
 * 
 ****************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LINE_LENGTH  200

/** function prototypes **/
char *strip(char *s);

void parse(FILE * file);

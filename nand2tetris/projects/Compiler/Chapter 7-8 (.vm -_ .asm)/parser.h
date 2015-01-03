#ifndef PARSER_H
#define PARSER_H

#include "vmTranslator.h"

/* All command (including each arithmetic command) */
#define NUMBER_COMMAND 17

enum commandType
{
	C_ERROR 		= -1,
	C_ARITHMETIC  	= 0,
	C_PUSH 	       	= 1,
	C_POP 		= 2,
	C_LABEL 		= 3,
	C_GOTO 		= 4,
	C_IF 			= 5,
	C_FUNCTION 	= 6,
	C_RETURN 		= 7,
	C_CALL 		= 8
};

typedef struct command command;
struct command
{
	const char *name;
	const int type;
};


/* Test the end of currentVMFile, return 1 if there is more commands in the file and 0 else */
int hasMoreCommands(void);


/* Test if there is more letters (or digit, or '_', or '.', or ':') in the string starting at index to the end of the string */
int isMoreLetter(const char string[LENGTH_MAX], const int index);
/* Read the current line of currentVMFile */
void advance(void);


/* Search in string, model (but only the whole word). Return 1 is there is an occurence and 0 else */
int isInString(const char *string, const char *model);
/* Test all of the differences possibilites of command */
int commandType(void);


/* Put in string the first argument of the current command, if it is a arithmetic command string = currentCommand */
void arg1(char string[LENGTH_MAX]);
/* Return the second argument of the current command */
int arg2(void);

#endif

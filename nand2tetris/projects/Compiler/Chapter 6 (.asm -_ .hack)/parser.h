#ifndef PARSER_H
#define PARSER_H

enum command
{
	A_COMMAND = 0,
	C_COMMAND = 1,
	L_COMMAND = 2
};

int hasMoreCommands(void);

void advance(void);

int commandType(void);

void symbol(char *string);
void symbolBinary(int address, int bits[16]);

void destParser(char *string);

void compParser(char *string);

void jumpParser(char *string);

#endif

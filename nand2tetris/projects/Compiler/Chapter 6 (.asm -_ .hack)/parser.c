#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"

#include "assembler.h"

int hasMoreCommands(void)
{
	/* Test the end of asmFile */
	if(feof(asmFile))
		return 0;
	else
		return 1;
}

void advance(void)
{
	int indexLength, indexCommand;
	char buffer[LENGTH_MAX];
	
	/* Read the current line of asmFile */
	if(fgets(buffer, LENGTH_MAX, asmFile) == NULL)
	{
		currentCommand[0] = '\0';
		return ;
	}
	else
		currentCommand[strlen(currentCommand) - 1] = '\0';
			
	/* Copy in currentCommand only characters exepect white space, tabulation,
	and comments */
	for(indexLength = 0, indexCommand = 0; buffer[indexLength] != '\0'; ++indexLength)
	{
		/* If it is a comment */
		if(buffer[indexLength] == '/' && buffer[indexLength + 1] == '/')
		{
			/* End of the current command */
			currentCommand[indexCommand] = '\0';
			return ;
		}
		/* If it is not a white space or a tabulation */
		else if(buffer[indexLength] != ' ' && buffer[indexLength] != '\t')
		{
			/* Copy the character */
			currentCommand[indexCommand] = buffer[indexLength];
			++indexCommand;
		}
	}

	currentCommand[indexCommand - 1] = '\0';
}

int commandType(void)
{
	/* If the command starts with '@' it is a A_COMMAND */
	if(currentCommand[0] == '@')
		return A_COMMAND;
	/* If the command starts with '(' it is a L_COMMAND */
	else if(currentCommand[0] == '(')
		return L_COMMAND;
	/* Else it is a C_COMMAND  */
	else
		return C_COMMAND;
}

void symbol(char *string)
{
	int indexLength, indexString;

	/* In a A instruction (like @Xxx or (Xxx)), it puts in string Xxx (symbol or decimal) */
	for(indexLength = 1, indexString = 0; isalpha(currentCommand[indexLength]) || isdigit(currentCommand[indexLength]) || 
							   currentCommand[indexLength] == '_' || currentCommand[indexLength] == '.' || 
							   currentCommand[indexLength] == '$' || currentCommand[indexLength] == ':'
							   ; ++indexLength, ++indexString)
		string[indexString] = currentCommand[indexLength];

	string[indexString] = '\0';
}

void symbolBinary(int address, int bits[16])
{
	int indexBit;

	/* Convert address into binairy */

	for(indexBit = 15; indexBit >= 0; --indexBit)
	{
		int rest;

		rest = address % 2;

		bits[indexBit] = rest;

		address /= 2;
	}
}

void destParser(char *string)
{
	int indexString;

	/* In a C instruction puts in string the dest part */

	/* If there is not '=', there is no dest part */
	if(strchr(currentCommand, '=') == NULL)
	{
		string[0] = '\0';
		return ;
	}

	/* Else put dest part in string */
	for(indexString = 0; currentCommand[indexString] != '='; ++indexString)
		string[indexString] = currentCommand[indexString];

	string[indexString] = '\0';
}

void compParser(char *string)
{
	int indexLength, indexString;

	indexLength = 0;

	/* In a C instruction puts in string the comp part */

	/* If there is a '=', the comp part starts after the '=' */
	if(strchr(currentCommand, '=') != NULL)
	{
		for(; currentCommand[indexLength] != '='; ++indexLength)
			;

		++indexLength;
	}

	/* Else it starts at the beginning */
	/* And it puts the comp part in string */
	for(indexString = 0; currentCommand[indexLength] != ';' && !iscntrl(currentCommand[indexLength]); ++indexLength, ++indexString)
		string[indexString] = currentCommand[indexLength];

	string[indexString] = '\0';
}

void jumpParser(char *string)
{
	int indexLength, indexString;

	indexLength = 0;

	/* In a C instruction puts in string the jump part */

	/* If there is not ';', there is no jump part */
	if(strchr(currentCommand, ';') == NULL)
	{
		string[0] = '\0';
		return ;
	}

	/* Else, the jum part starts after the ';' */
	for(indexLength = 0; currentCommand[indexLength] != ';'; ++indexLength)
		;

	++indexLength;

	/* And it puts the jump part in string */
	for(indexString = 0; !iscntrl(currentCommand[indexLength]); ++indexLength, ++indexString)
		string[indexString] = currentCommand[indexLength];

	string[indexString] = '\0';
}

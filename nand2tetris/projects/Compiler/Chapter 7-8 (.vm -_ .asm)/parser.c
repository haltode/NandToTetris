#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"

/* Test the end of currentVMFile, return 1 if there is more commands in the file and 0 else */
int hasMoreCommands(void)
{
	if(feof(currentVMFile))
		return 0;
	else
		return 1;
}

/* Test if there is more letters (or digit, or '_', or '.', or ':') in the string starting at index to the end of the string */
int isMoreLetter(const char string[LENGTH_MAX], const int index)
{
	int indexCommand;

	for(indexCommand = index; string[indexCommand] != '\0'; ++indexCommand)
	{
		if(isalnum(string[indexCommand]) 	|| 
		   string[indexCommand] == '_' 		|| 
		   string[indexCommand] == '.' 		|| 
		   string[indexCommand] == ':')
			return 1;
	}

	return 0;
}

/* Read the current line of currentVMFile */
void advance(void)
{
	int indexLength, indexCommand;
	char buffer[LENGTH_MAX];

	/* Read the current line in the current vm file */
	if(fgets(buffer, LENGTH_MAX, currentVMFile) != NULL)
		/* Put the last character of buffer to '\0' */
		buffer[strlen(buffer)] = '\0';
	/* If this is the end of the file, put currentCommand[0] to '\0' and stop reading */
	else
	{
		currentCommand[0] = '\0';
		return ;
	}
		
	/* Copy in currentCommand only characters expect comments */
	for(indexLength = 0, indexCommand = 0; buffer[indexLength] != '\0'; ++indexLength)
	{
		/* If there is more letter in buffer (not only space, or tabulation) */
		if(isMoreLetter(buffer, indexLength))
		{
			/* If it is a comment */
			if(buffer[indexLength] == '/')
			{
				int indexComment;

				for(indexComment = indexLength - 1; buffer[indexComment] == ' '; --indexComment)
					;

				/* End of the current command */
				currentCommand[indexComment + 1] = '\0';
				return ;
			}
			/* Else it is a character */
			else
			{
				/* Copy the character into currentCommand */
				currentCommand[indexCommand] = buffer[indexLength];
				++indexCommand;
			}
		}
		/* Else there is no more letters, put '\0' in currentCommand and stop */
		else
		{
			currentCommand[indexCommand] = '\0';
			return ;
		}
	}

	/* End of the current command */
	currentCommand[indexCommand] = '\0';
}

/* Search in string, model (but only the whole word). Return 1 is there is an occurence and 0 else */
int isInString(const char *string, const char *model)
{
	char *occurence;
	int length;

	occurence = strstr(string, model);
	length = strlen(model);

	/* Test is there is an occurence in string and if it is a whole word */
	if(occurence != NULL && (occurence[length] == ' ' || occurence[length] == '\n' || occurence[length] == '\0'))
		return 1;
	else
		return 0;
}

/* Test all of the differences possibilites of command */
int commandType(void)
{
	static const command baseCommandType[NUMBER_COMMAND] =
	{
		{"add", 		C_ARITHMETIC},
		{"sub", 		C_ARITHMETIC},
		{"neg", 		C_ARITHMETIC},
		{"eq", 		C_ARITHMETIC},
		{"gt", 		C_ARITHMETIC},
		{"lt", 		C_ARITHMETIC},
		{"and", 		C_ARITHMETIC},
		{"or", 		C_ARITHMETIC},
		{"not", 		C_ARITHMETIC},
		{"push", 		C_PUSH},
		{"pop", 		C_POP},
		{"label", 		C_LABEL},
		{"if-goto",		C_IF},
		{"goto", 		C_GOTO},
		{"function", 	C_FUNCTION},
		{"return", 		C_RETURN},
		{"call", 		C_CALL}
	};
	int indexCommand;

	/* Test all commands */
	for(indexCommand = 0; indexCommand < NUMBER_COMMAND; ++indexCommand)
	{
		if(isInString(currentCommand, baseCommandType[indexCommand].name))
			return baseCommandType[indexCommand].type;
	}

	return C_ERROR;
}

/* Put in string the first argument of the current command, if it is a arithmetic command string = currentCommand */
void arg1(char string[LENGTH_MAX])
{
	/* If the command is a C_ARITHMETIC command, string = the currentCommand */
	if(commandType() == C_ARITHMETIC)
		strcpy(string, currentCommand);
	/* Else, string = first argument of the command */
	else
	{
		int indexCommand;
		int indexString;

		for(indexCommand = 0; currentCommand[indexCommand] != ' '; ++indexCommand)
			;

		++indexCommand;

		for(indexString = 0; isalpha(currentCommand[indexCommand]) || isdigit(currentCommand[indexCommand]) || currentCommand[indexCommand] == '_' || currentCommand[indexCommand] == '.' || currentCommand[indexCommand] == ':' ; ++indexCommand, ++indexString)
			string[indexString] = currentCommand[indexCommand];

		string[indexString] = '\0';
	}
}

/* Return the second argument of the current command */
int arg2(void)
{
	char string[LENGTH_MAX];
	int indexCommand;
	int indexString;
	int numberSpace;

	numberSpace = 0;

	/* Goto the second space (the first one is for the argument 1 and the second for the argument 2) */
	for(indexCommand = 0; numberSpace != 2 && currentCommand[indexCommand] != '\0'; ++indexCommand)
	{
		if(currentCommand[indexCommand] == ' ')
			++numberSpace;
	}

	/* Copy all the second argument */
	for(indexString = 0; currentCommand[indexCommand] != '\0'; ++indexCommand, ++indexString)
		string[indexString] = currentCommand[indexCommand];

	string[indexString] = '\0';

	/* Return the number of the second argument */
	return atoi(string);
}

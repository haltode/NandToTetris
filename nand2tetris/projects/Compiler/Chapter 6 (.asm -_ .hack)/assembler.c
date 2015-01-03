#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "assembler.h"

#include "parser.h"
#include "code.h"
#include "symbolTable.h"

void createHackFileName(char hackFileName[LENGTH_MAX])
{
	int hackFileLenght;

	/* Create the hack file name */
	hackFileLenght = strlen(hackFileName);

	hackFileName[hackFileLenght - 3] = '\0';

	hackFileName[hackFileLenght - 3] = 'h';
	hackFileName[hackFileLenght - 2] = 'a';
	hackFileName[hackFileLenght - 1] = 'c';
	hackFileName[hackFileLenght] = 'k';
	hackFileName[hackFileLenght + 1] = '\0';
}

void firstPass(void)
{
	int ROMaddress;

	ROMaddress = 0;

	/* First pass */
	while(hasMoreCommands())
	{
		/* Read the current command */
		advance();

		/* If there is a current command */
		if(currentCommand[0] != '\0')
		{
			int returnCommandType;

			/* Get the type of the command */
			returnCommandType = commandType();

			/* If it is a A_COMMAND or C_COMMAND */
			if(returnCommandType == A_COMMAND || returnCommandType == C_COMMAND)
				++ROMaddress;
			/* Else if it is a L_COMMAND */
			else if(returnCommandType == L_COMMAND)
			{
				char string[LENGTH_MAX];

				/* Get the symbol */
				symbol(string);
				/* Add the symbol into the symbol table associating the ROMaddress variable */
				addEntry(string, ROMaddress);
			}
		}
	}
}

void secondPass(void)
{
	/* Second pass */
	/* While there is a command */
	while(hasMoreCommands())
	{
		/* Read the current command */
		advance();

		/* If there is a current command */
		if(currentCommand[0] != '\0')
		{
			char string[LENGTH_MAX];
			int returnCommandType;
			int bits[16];
			int stringAddress;
			int indexBit;

			/* Get the type of the command */
			returnCommandType = commandType();

			/* If it is a A_COMMAND */
			if(returnCommandType == A_COMMAND)
			{
				/* Get the symbol */
				symbol(string);

				/* If the symbol is not a number */
				if(isalpha(string[0]))
				{
					/* If string is already in the symbol table */
					if(contains(string))
						/* Get string's address */
						stringAddress = getAddress(string);
					/* Else it is a new symbol */
					else
					{
						/* Get the address */
						stringAddress = RAMaddress;
						/* Add string to the symbol table */
						addEntry(string, RAMaddress);

						/* Increment the next available RAM address */
						++RAMaddress;
					}
				}
				/* Else it is a number */
				else
					/* Get string's address */
					stringAddress = atoi(string);
				
				/* Transform it into binairy */
				symbolBinary(stringAddress, bits);

				/* Put it into the hack file */
				for(indexBit = 0; indexBit < 16; ++indexBit)
					fprintf(hackFile, "%d", bits[indexBit]);

				fprintf(hackFile, "\n");
			}
			/* Else if it is a C_COMMAND */
			else if(returnCommandType == C_COMMAND)
			{
				/* Put the 3 first bits to 1 in the hack file */
				for(indexBit = 0; indexBit < 3; ++indexBit)
					fprintf(hackFile, "1");

				/* Get the comp part */
				compParser(string);
				/* Get the binairy */
				comp(string, bits);

				/* Write the comp part into the hack file */
				for(indexBit = 0; indexBit < 7; ++indexBit)
					fprintf(hackFile, "%d", bits[indexBit]);


				/* Get the dest part */
				destParser(string);
				/* Get the binairy */
				dest(string, bits);

				/* Write the dest part into the hack file */
				for(indexBit = 0; indexBit < 3; ++indexBit)
					fprintf(hackFile, "%d", bits[indexBit]);


				/* Get the jump part */
				jumpParser(string);
				/* Get the binairy */
				jump(string, bits);

				/* Write the jump part into the hack file */
				for(indexBit = 0; indexBit < 3; ++indexBit)
					fprintf(hackFile, "%d", bits[indexBit]);

				fprintf(hackFile, "\n");
			}
		}

	}
}

int main(int argc, char *argv[])
{
	char hackFileName[LENGTH_MAX];
	
	/* Init the fisrt available RAM address */
	RAMaddress = 16;

	/* Create the hack file (output) name */
	strcpy(hackFileName, argv[1]);
	createHackFileName(hackFileName);

	/* Init the symbol table */
	constructor();

	asmFile = fopen(argv[1], "r");

	/* First pass : create the symbol table */
	firstPass();

	fclose(asmFile);

	asmFile = fopen(argv[1], "r");
	hackFile = fopen(hackFileName, "w+");

	/* Second pass : create the binairy code */
	secondPass();

	fclose(asmFile);
	fclose(hackFile);

	return 0;
}

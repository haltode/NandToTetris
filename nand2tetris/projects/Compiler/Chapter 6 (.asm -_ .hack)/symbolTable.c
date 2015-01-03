#include <stdio.h>
#include <string.h>
#include "symbolTable.h"

void constructor(void)
{
	currentSymbol = 0;

	/* Init a new symbol table with the predefined symbols */
	addEntry("SP", 0);
	addEntry("LCL", 1);
	addEntry("ARG", 2);
	addEntry("THIS", 3);
	addEntry("THAT", 4);

	addEntry("R0", 0);
	addEntry("R1", 1);
	addEntry("R2", 2);
	addEntry("R3", 3);
	addEntry("R4", 4);
	addEntry("R5", 5);
	addEntry("R6", 6);
	addEntry("R7", 7);
	addEntry("R8", 8);
	addEntry("R9", 9);
	addEntry("R10", 10);
	addEntry("R11", 11);
	addEntry("R12", 12);
	addEntry("R13", 13);
	addEntry("R14", 14);
	addEntry("R15", 15);

	addEntry("SCREEN", 16384);

	addEntry("KBD", 24576);
}

void addEntry(const char symbol[LENGTH_MAX], const int address)
{
	/* Add symbol, address into the symbol table */
	strcpy(symbolTable[currentSymbol].label, symbol);
	symbolTable[currentSymbol].address = address;

	++currentSymbol;
}

int contains(const char symbol[LENGTH_MAX])
{
	int indexTable;

	/* Return 1 if symbol is in the symbol table and 0 else */
	for(indexTable = 0; indexTable < SYMBOL_TABLE_LENGTH; ++indexTable)
	{
		if(strcmp(symbol, symbolTable[indexTable].label) == 0)
			return 1;
	}

	return 0;
}

int getAddress(const char symbol[LENGTH_MAX])
{
	int indexTable;

	/* Return the address of symbol */
	for(indexTable = 0; indexTable < SYMBOL_TABLE_LENGTH; ++indexTable)
	{
		if(strcmp(symbol, symbolTable[indexTable].label) == 0)
			return symbolTable[indexTable].address;
	}

	return 0;
}

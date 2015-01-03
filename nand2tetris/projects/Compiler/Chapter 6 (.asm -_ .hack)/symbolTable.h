#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "assembler.h"

#define SYMBOL_TABLE_LENGTH 24577

typedef struct Symbol Symbol;
struct Symbol
{
	char label[LENGTH_MAX];
	int address;
};	

void constructor(void);

void addEntry(const char symbol[LENGTH_MAX], const int address);

int contains(const char symbol[LENGTH_MAX]);

int getAddress(const char symbol[LENGTH_MAX]);


Symbol symbolTable[SYMBOL_TABLE_LENGTH];
int currentSymbol;

#endif

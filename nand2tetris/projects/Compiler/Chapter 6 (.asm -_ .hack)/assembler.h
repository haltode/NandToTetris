#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#define LENGTH_MAX 500

FILE *asmFile;
FILE *hackFile;
char currentCommand[LENGTH_MAX];
int RAMaddress;

void createHackFileName(char hackFileName[LENGTH_MAX]);

#endif

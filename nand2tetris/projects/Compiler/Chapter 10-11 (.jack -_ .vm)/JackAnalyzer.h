#ifndef JACK_ANALYZER_H
#define JACK_ANALYZER_H

#include <dirent.h>

#ifndef __WIN32__
	#include <sys/types.h>
#endif

#define LENGTH_MAX 256

FILE *currentJackFile;
FILE *vmFile;
char currentFileName[LENGTH_MAX];


void getVMOutputFileName(char *string);


void translateJackFile(void);


void checkDeclaration(const char *line);
void cleanJackFile(const char *fileName);
void prepareJackCompiler(void);


void goThroughDirectory(const char *directoryName, void (*pointer)(void));

#endif

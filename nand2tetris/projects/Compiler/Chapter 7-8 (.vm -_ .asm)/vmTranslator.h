#ifndef VM_TRANSLATOR_H
#define VM_TRANSLATOR_H

#define LENGTH_MAX 500

#include <dirent.h>

#ifndef __WIN32__
	#include <sys/types.h>
#endif

/* Pointer to the currentVMFile (input) */
FILE *currentVMFile;
/* Pointer to the asmFile (output) */
FILE *asmFile;
/* The current command in the current vm file */
char currentCommand[LENGTH_MAX];
/* The current file name of the current vm file */
char currentFileName[LENGTH_MAX];


/* Translate the file path (.vm file) */
void translate(const char *path);

#endif

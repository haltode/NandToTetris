#ifndef VM_WRITER_H
#define VM_WRITER_H

enum segment
{
	CONST_SEGMENT 	= 0,
	ARG_SEGMENT 		= 1,
	LOCAL_SEGMENT 	= 2,
	STATIC_SEGMENT 	= 3,
	THIS_SEGMENT 		= 4,
	THAT_SEGMENT 		= 5,
	POINTER_SEGMENT 	= 6,
	TEMP_SEGMENT 		= 7
};

enum command
{
	ADD 		= 0,
	SUB 		= 1,
	NEG 		= 2,
	EQ 		= 3,
	GT 		= 4,
	LT 		= 5,
	AND 		= 6,
	OR 		= 7,
	NOT 		= 8
};

void writeSegment(const int segment);
void writePush(const int segment, const int index);
void writePop(const int segment, const int index);
void writeArithmetic(const int command);
void writeLabel(const char *label);
void writeGoto(const char *label);
void writeIf(const char *label);
void writeCall(const char *name, const int nArgs);
void writeFunction(const char *name, const int nLocals);
void writeReturn(void);


void printOp(const char *op);
void printUnaryOp(const char *op);


int getSegmentFromKind(const int kind);


void writeAllocateMemoryBlockConstructor(void);


void writeSetObjectInMemoryMethod(void);


void writeArrayMemory(char *varName, char *subroutineName);
void writeArrayAllocation(void);
void writeArrayExpression(char *name);


void writeMethodCall(char *name);
void writeSubroutineCall(char *name);


void writeStringConstant(const char *string);

#endif

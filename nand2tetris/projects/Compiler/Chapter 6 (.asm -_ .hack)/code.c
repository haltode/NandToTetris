#include <stdio.h>
#include <string.h>
#include "code.h"

void init3Bits(int bits[3], const int bit1, const int bit2, const int bit3)
{
	/* Copy 3 bits */
	bits[0] = bit1;
	bits[1] = bit2;
	bits[2] = bit3;
}

void init6Bits(int bits[7], const int bit1, const int bit2, const int bit3, const int bit4, const int bit5, const int bit6)
{
	/* Copy 6 bits */
	bits[1] = bit1;
	bits[2] = bit2;
	bits[3] = bit3;
	bits[4] = bit4;
	bits[5] = bit5;
	bits[6] = bit6;
}

void dest(const char *string, int bits[3])
{
	/* Return the binairy of the dest part corresponding to the section 6.2.2 page 110 of the book */
	if(string[0] == '\0')
		init3Bits(bits, 0, 0, 0);
	else if(string[0] == 'M' && string[1] == '\0')
		init3Bits(bits, 0, 0, 1);
	else if(string[0] == 'D' && string[1] == '\0')
		init3Bits(bits, 0, 1, 0);
	else if(string[0] == 'M' && string[1] == 'D' && string[2] == '\0')
		init3Bits(bits, 0, 1, 1);
	else if(string[0] == 'A' && string[1] == '\0')
		init3Bits(bits, 1, 0, 0);
	else if(string[0] == 'A' && string[1] == 'M' && string[2] == '\0')
		init3Bits(bits, 1, 0, 1);
	else if(string[0] == 'A' && string[1] == 'D' && string[2] == '\0')
		init3Bits(bits, 1, 1, 0);
	else if(string[0] == 'A' && string[1] == 'M' && string[2] == 'D')
		init3Bits(bits, 1, 1, 1);
}

void comp(const char *string, int bits[7])
{
	/* Return the binairy of the comp part corresponding to the section 6.2.2 page 109 of the book */
	if(strchr(string, 'M') == NULL)
		bits[0] = 0;
	else
		bits[0] = 1;

	if(string[0] == '0' && string[1] == '\0')
		init6Bits(bits, 1, 0, 1, 0, 1, 0);
	else if(string[0] == '1' && string[1] == '\0')
		init6Bits(bits, 1, 1, 1, 1, 1, 1);
	else if(string[0] == '-' && string[1] == '1')
		init6Bits(bits, 1, 1, 1, 0, 1, 0);
	else if(string[0] == 'D' && string[1] == '\0')
		init6Bits(bits, 0, 0, 1, 1, 0, 0);
	else if((string[0] == 'A'  && string[1] == '\0') || (string[0] == 'M' && string[1] == '\0'))
		init6Bits(bits, 1, 1, 0, 0, 0, 0);
	else if(string[0] == '!' && string[1] == 'D')
		init6Bits(bits, 0, 0, 1, 1, 0, 1);
	else if((string[0] == '!' && string[1] == 'A') || (string[0] == '!' && string[1] == 'M'))
		init6Bits(bits, 1, 1, 0, 0, 0, 1);
	else if(string[0] == '-' && string[1] == 'D')
		init6Bits(bits, 0, 0, 1, 1, 1, 1);
	else if((string[0] == '-' && string[1] == 'A') || (string[0] == '-' && string[1] == 'M'))
		init6Bits(bits, 1, 1, 0, 0, 1, 1);
	else if(string[0] == 'D' && string[1] == '+' && string[2] == '1')
		init6Bits(bits, 0, 1, 1, 1, 1, 1);
	else if((string[0] == 'A' && string[1] == '+' && string[2] == '1') || (string[0] == 'M' && string[1] == '+' && string[2] == '1'))
		init6Bits(bits, 1, 1, 0, 1, 1, 1);
	else if(string[0] == 'D' && string[1] == '-' && string[2] == '1')
		init6Bits(bits, 0, 0, 1, 1, 1, 0);
	else if((string[0] == 'A' && string[1] == '-' && string[2] == '1') || (string[0] == 'M' && string[1] == '-' && string[2] == '1'))
		init6Bits(bits, 1, 1, 0, 0, 1, 0);
	else if((string[0] == 'D' && string[1] == '+' && string[2] == 'A') || (string[0] == 'D' && string[1] == '+' && string[2] == 'M'))
		init6Bits(bits, 0, 0, 0, 0, 1, 0);
	else if((string[0] == 'D' && string[1] == '-' && string[2] == 'A') || (string[0] == 'D' && string[1] == '-' && string[2] == 'M'))
		init6Bits(bits, 0, 1, 0, 0, 1, 1);
	else if((string[0] == 'A' && string[1] == '-' && string[2] == 'D') || (string[0] == 'M' && string[1] == '-' && string[2] == 'D'))
		init6Bits(bits, 0, 0, 0, 1, 1, 1);
	else if((string[0] == 'D' && string[1] == '&' && string[2] == 'A') || (string[0] == 'D' && string[1] == '&' && string[2] == 'M'))
		init6Bits(bits, 0, 0, 0, 0, 0, 0);
	else if((string[0] == 'D' && string[1] == '|' && string[2] == 'A') || (string[0] == 'D' && string[1] == '|' && string[2] == 'M'))
		init6Bits(bits, 0, 1, 0, 1, 0, 1);
}

void jump(const char *string, int bits[3])
{
	/* Return the binairy of the jump part corresponding to the section 6.2.2 page 110 of the book */
	if(string[0] == '\0')
		init3Bits(bits, 0, 0, 0);
	else if(string[0] == 'J' && string[1] == 'G' && string[2] == 'T')
		init3Bits(bits, 0, 0, 1);
	else if(string[0] == 'J' && string[1] == 'E' && string[2] == 'Q')
		init3Bits(bits, 0, 1, 0);
	else if(string[0] == 'J' && string[1] == 'G' && string[2] == 'E')
		init3Bits(bits, 0, 1, 1);
	else if(string[0] == 'J' && string[1] == 'L' && string[2] == 'T')
		init3Bits(bits, 1, 0, 0);
	else if(string[0] == 'J' && string[1] == 'N' && string[2] == 'E')
		init3Bits(bits, 1, 0, 1);
	else if(string[0] == 'J' && string[1] == 'L' && string[2] == 'E')
		init3Bits(bits, 1, 1, 0);
	else if(string[0] == 'J' && string[1] == 'M' && string[2] == 'P')
		init3Bits(bits, 1, 1, 1);
}

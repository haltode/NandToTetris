#ifndef CODE_H
#define CODE_H

void init3Bits(int bits[3], const int bit1, const int bit2, const int bit3);

void init6Bits(int bits[7], const int bit1, const int bit2, const int bit3, const int bit4, const int bit5, const int bit6);

void dest(const char *string, int bits[3]);

void comp(const char *string, int bits[7]);

void jump(const char *string, int bits[3]);

#endif

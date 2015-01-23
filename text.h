#ifndef TEXT_H_
#define TEXT_H_

//array that holds all of the character maps
extern const unsigned char fontdata_6x8[12288];
typedef unsigned char u8;

void drawChar(int, int, u8, unsigned short);
void drawString(int, int, char*, unsigned short);

#endif

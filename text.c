#include <stdio.h>
#include <math.h>
#include "mylib.h"

#include "text.h"

void drawChar(int x, int y, u8 ch, u16 color) {
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 6; i++) {
            if (fontdata_6x8[OFFSET(i, j, 6) + ch*48]) {
                setPixel(i + x, j + y, color);
            }
        }
    }
}

void drawString(int x, int y, char* str, u16 color) {
    while (*str) {
        drawChar(x, y, *str++, color);
        x += 6;
    }
}

#include "mylib.h"
#include "flappyBird.h"
#include "pipeTop.h"

int main() {
//set mode
    REG_DISPCTL = MODE3 | BG2_ENABLE;

    titleScreenFunc();
    initGame(); 
    // printBird(STARTPOSX, STARTPOSY, 0);
    // drawImage3(100, 100, FLAPPYBIRD_WIDTH, FLAPPYBIRD_HEIGHT, flappyBird);
    // drawImage3(100, 100, PIPETOP_WIDTH, PIPETOP_HEIGHT, pipeTop);
    while(1) {
        for (int i = 0; i < 3; i++) {
            waitForVBlank();
            if (KEY_DOWN_NOW(BUTTON_A)) {
                moveBird(1);
            } 
        }
        movePipes();
        if (KEY_DOWN_NOW(BUTTON_A)) {
            moveBird(1);
        } else {
            moveBird(0);
        }
    }
    return 0;
}
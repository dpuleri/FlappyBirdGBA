
#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "text.h"
#include "flappyBird.h"
#include "bg.h"
#include "grass.h"
#include "pipeTop.h"
#include "pipeMiddle.h"
#include "gameOver.h"
#include "titleScreen.h"

u16* videoBuffer = VID_BFFR; //define global var
const int gravity = 1;
static pipes* mypipes;
static int score;
static int highScore = 0;

//some bird vars
static int vel; //start velocity at zero
static int x;
static int y;
static int gravityCounter;

void initGame() {
    //bird stuff
    vel = 0; //start velocity at zero
    x = STARTPOSX;
    y = STARTPOSY;
    gravityCounter = 0;
    srand(50);

    drawFullWidth(0, BG_WIDTH, BG_HEIGHT, bg);
    drawFullWidth(BG_HEIGHT, MAX_X, GRASS_HEIGHT, grass);
    //draws the ground
    drawRect(0, BG_HEIGHT + GRASS_HEIGHT, MAX_X, 10, BEIGE);
    score = 0;
    drawString(180, (BG_HEIGHT + GRASS_HEIGHT + 2), "Score", BLACK);
    drawString(1, (BG_HEIGHT + GRASS_HEIGHT + 2), "High Score", BLACK);
    printScore();
    printHighScore();

    //making pipes
    mypipes = malloc(sizeof(*mypipes));
    generatePipes(mypipes);
    printPipe(mypipes->head);
}

void titleScreenFunc() {
    drawFullWidth(0, TITLESCREEN_WIDTH, TITLESCREEN_HEIGHT, titleScreen);
    drawString(165, 80, "Press Start", WHITE);
    while(!KEY_DOWN_NOW(BUTTON_START));
}

//sets a pixel
void setPixel(int x, int y, u16 color) {
    int offset = OFFSET(x, y, MAX_X);
    //int offset = (y * MAX_X) + x; //old code
    //*((u16*) (VID_BFFR + offset)) = color;
    videoBuffer[offset] = color;
}

//gets the color of a pixel
u16 getPixelColor(int x, int y) {
    int offset = (y * 240) + x;
    return *((u16*) (VID_BFFR + offset));
}

void drawRect(int x, int y, int width, int height, u16 color) {
    volatile u16 clrptr = (volatile u16) color;
    for (int j = y; j < (y + height); j++) {
        DMA[3].src = &clrptr;
        DMA[3].dst = &videoBuffer[OFFSET(x, j, MAX_X)];
        DMA[3].cnt = width | DMA_SOURCE_FIXED | DMA_ON;
    }
}

void drawHollowRect(int x, int y, int width, int height, u16 color) {
    for (int i = x; i < (x + width); i++) {
        setPixel(i, y, color);
        setPixel(i, y + height, color);
    }
    for (int j = y; j < (y + height); j++) {
        setPixel(x, j, color);
        setPixel(x + width, j, color);
    }
}

void plotLine(int x0, int y0, int x1, int y1, u16 color) {
    int dx = abs(x1 - x0);
    int sx = SIGN(x1 - x0);
    int dy = abs(y1 - y0);
    int sy = SIGN(y1 - y0); 
    int err = (dx > dy ? dx : (~dy + 1)) >> 1;
    int e2;
 
    while (x0 != x1 || y0 != y1) {
        setPixel(x0, y0, color);
        e2 = err;
        if (e2 > (~dx + 1)) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
    setPixel(x1, y1, color);
}

char* int2str(int number) {
    int buffsize = 2; //account for /0 and first char
    int tmpNum = number;
    while (tmpNum > 9) {
        buffsize++;
        tmpNum /= 10;
    }
    char* buffer = malloc(buffsize);
    sprintf(buffer, "%d", number);
    return buffer;
}


void waitForVBlank() {
    while(SCANLINECOUNTER >= 160);
    while(SCANLINECOUNTER < 160);
}

void printBird(int x, int y, int del) {
    int hadCollision = 0;
    //row 1
    hadCollision += printBirdRow(x, y, 7, 1, 6, del);

    //row 2
    hadCollision += printBirdRow(x, y, 5, 2, 9, del);

    //row 3
    hadCollision += printBirdRow(x, y, 4, 3, 11, del);

    //row 4
    hadCollision += printBirdRow(x, y, 3, 4, 13, del);

    //row 5
    hadCollision += printBirdRow(x, y, 2, 5, 14, del);

    //row 6
    hadCollision += printBirdRow(x, y, 2, 6, 14, del);

    //row 7
    hadCollision += printBirdRow(x, y, 1, 7, 16, del);

    //row 8
    hadCollision += printBirdRow(x, y, 1, 8, 17, del);

    //row 9
    hadCollision += printBirdRow(x, y, 2, 9, 15, del);

    //row 10
    hadCollision += printBirdRow(x, y, 3, 10, 14, del);

    //row 11
    hadCollision += printBirdRow(x, y, 4, 11, 12, del);

    //row 12
    hadCollision += printBirdRow(x, y, 6, 12, 5, del);

    if (hadCollision) {
        gameOverFunc();
    }
}

//ALSO CHECK FOR COLLISIONS HERE
int printBirdRow(int x, int y, int i, int j, int width, int del) {
    if (!del && ((videoBuffer[OFFSET((x + i), (y + j), MAX_X)] !=
        bg[OFFSET(((x + i)%BG_WIDTH), ((y + j)%BG_HEIGHT), MAX_X)]) ||
        (videoBuffer[OFFSET((x + i + width), (y + j), MAX_X)] !=
        bg[OFFSET(((x + i + width)%BG_WIDTH), ((y + j)%BG_HEIGHT), MAX_X)]))) {
        return 1; //gotta get out
    }
    if (!del) {
        DMA[3].src = &flappyBird[OFFSET(i, j, FLAPPYBIRD_WIDTH)]; 
    } else {
        DMA[3].src = &bg[OFFSET(((x + i)%BG_WIDTH), ((y + j)%BG_HEIGHT), MAX_X)];
    }
    DMA[3].dst = &videoBuffer[OFFSET((x + i), (y + j), MAX_X)];
    DMA[3].cnt = width | DMA_ON; 
    return 0;
}

void drawImage3(int x, int y, int width, int height, const u16* img) {
    for (int j = 0; j < height; j++) {
        DMA[3].src = &img[OFFSET(0, j, width)];
        DMA[3].dst = &videoBuffer[OFFSET(x, (y + j), MAX_X)];
        DMA[3].cnt = width | DMA_ON;
    }
}

void drawImage3UpsideDown(int x, int y, int width, int height, const u16* img) {
    for (int j = 0, jj = height - 1; j < height; j++, jj--) {
        DMA[3].src = &img[OFFSET(0, jj, width)];
        DMA[3].dst = &videoBuffer[OFFSET(x, (y + j), MAX_X)];
        DMA[3].cnt = width | DMA_ON;
    }
}

void drawFullWidth(int y, int width, int height, const u16* img) {
    DMA[3].src = img;
    DMA[3].dst = &videoBuffer[OFFSET(0, y, MAX_X)];
    DMA[3].cnt = (width*height) | DMA_ON;
}

void moveGrass() {
    static int dx = 0;
    dx = dx % 10;
    drawFullWidth(BG_HEIGHT, MAX_X, GRASS_HEIGHT, &grass[dx++]);
}

void moveBird(int hasJumped) {
    printBird(x, y, 1);
    if (hasJumped) {
        // vel -= 1;
        // y -= 1;
        vel = -2;
    }
    if (gravityCounter == 5) { //slow it down!
        vel += gravity;
        gravityCounter = 0;
    }
    // vel += gravity;
    gravityCounter++;

    y += vel;
    printBird(x, y, 0);
}

void generatePipes(pipes* mypipes) {
    pipe* prevPipe = malloc(sizeof(*prevPipe));
    int xPos = PIPE_START_POS;
    setRandHeight(prevPipe);
    prevPipe->x = xPos;
    mypipes->head = prevPipe;
    int i;
    pipe* curPipe;
    for (i = 1, xPos += PIPE_SPACE; i < 3; i++, xPos += PIPE_SPACE) {
        curPipe = malloc(sizeof(*curPipe));
        curPipe->x = xPos;
        setRandHeight(curPipe);
        prevPipe->next = curPipe;
        curPipe->prev = prevPipe;
        prevPipe = curPipe;
    }
    mypipes->numPipes = i;
}

void setRandHeight(pipe* curPipe) {
    curPipe->botHeight = generateHeight();
    curPipe->topHeight = BG_HEIGHT - SPACE_NEEDED - curPipe->botHeight;
}

int generateHeight() {
    return ((rand() % (BG_HEIGHT - SPACE_NEEDED - (PIPETOP_HEIGHT*2))) + PIPETOP_HEIGHT);
    // return 40;
}

void printPipe(pipe* mypipe) {
    int yPos = BG_HEIGHT - mypipe->botHeight;
    //prints top of pipe
    drawImage3(mypipe->x, yPos, PIPETOP_WIDTH, PIPETOP_HEIGHT, pipeTop);
    for (yPos = yPos + PIPETOP_HEIGHT; yPos < BG_HEIGHT; yPos += PIPEMIDDLE_HEIGHT) {
        if (yPos + PIPEMIDDLE_HEIGHT >= BG_HEIGHT) {
            drawImage3(mypipe->x, (yPos - (yPos + PIPEMIDDLE_HEIGHT - BG_HEIGHT)), PIPEMIDDLE_WIDTH, PIPEMIDDLE_HEIGHT, pipeMiddle);
        } else {
            drawImage3(mypipe->x, yPos, PIPEMIDDLE_WIDTH, PIPEMIDDLE_HEIGHT, pipeMiddle);
        }
    }

    //prints out the bottom of the pipe
    yPos = mypipe->topHeight - PIPETOP_HEIGHT;
    drawImage3UpsideDown(mypipe->x, yPos, PIPETOP_WIDTH, PIPETOP_HEIGHT, pipeTop);
    for (yPos = yPos - PIPEMIDDLE_HEIGHT; yPos >= -20; yPos -= PIPEMIDDLE_HEIGHT) {
        drawImage3(mypipe->x, yPos, PIPEMIDDLE_WIDTH, PIPEMIDDLE_HEIGHT, pipeMiddle);
    }


    //draws the ground
    // drawRect(0, BG_HEIGHT + GRASS_HEIGHT, MAX_X, 10, BEIGE);
}

void clearPipe(pipe* mypipe) {
    for (int j = 0; j < BG_HEIGHT; j++) {
        DMA[3].src = &bg[OFFSET((mypipe->x+PIPEMIDDLE_WIDTH-2), j, BG_WIDTH)];
        DMA[3].dst = &videoBuffer[OFFSET((mypipe->x+PIPEMIDDLE_WIDTH-2), j, MAX_X)];
        DMA[3].cnt = 2 | DMA_ON;
    }
}

void movePipes() {
    pipe* curPipe = mypipes->head;
    while(curPipe != NULL) {
        clearPipe(curPipe);
        if (curPipe->x <= 0) {
            curPipe->x = MAX_X;
        } else if (curPipe->x == MAX_X) {
            setRandHeight(curPipe);            
            curPipe->x--;
        } else if (curPipe->x == (STARTPOSX - PIPETOP_WIDTH)) {
            score++;
            printScore();
            curPipe->x--;
        } else {
            curPipe->x--;
        }
        printPipe(curPipe);
        curPipe = curPipe->next;
    }
    moveGrass();
}

void printScore() {
    drawRect(219, (BG_HEIGHT + GRASS_HEIGHT + 2), 8 * 2, 7, BEIGE);
    char* intString = int2str(score);
    drawString(219, (BG_HEIGHT + GRASS_HEIGHT + 2), intString, BLACK);
    free(intString);

    if (score > highScore) {
        highScore = score;
        printHighScore();
    }
}

void printHighScore() {
    drawRect(70, (BG_HEIGHT + GRASS_HEIGHT + 2), 8 * 2, 7, BEIGE);
    char* intString = int2str(highScore);
    drawString(70, (BG_HEIGHT + GRASS_HEIGHT + 2), intString, BLACK);
    free(intString);
}

void gameOverFunc() {
    drawFullWidth(0, GAMEOVER_WIDTH, GAMEOVER_HEIGHT, gameOver);
    pipe* curPipe = mypipes->head;
    pipe* prevPipe;
    while(curPipe != NULL) {
        prevPipe = curPipe;
        curPipe = curPipe->next;
        free(prevPipe);
    }
    free(mypipes);

    //print final score
    drawString(180, 5, "Score", BLACK);
    char* scoreStrPtr = int2str(score);
    drawString(219, 5, scoreStrPtr, BLACK);
    free(scoreStrPtr);

    //draw final high score
    drawString(1, 5, "High Score", BLACK);
    scoreStrPtr = int2str(highScore);
    drawString(70, 5, scoreStrPtr, BLACK);
    free(scoreStrPtr);

    while(!KEY_DOWN_NOW(BUTTON_START));
    initGame();
}
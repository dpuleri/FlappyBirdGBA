#ifndef MYLIB_H_
#define MYLIB_H_

typedef unsigned short u16; //shortcut for 16bit unsigned

// *** Display Stuff ===================================================

#define REG_DISPCTL (* (u16*) (0x4000000))
#define MODE3 3
#define BG2_ENABLE (1<<10)
#define VID_BFFR ((u16*) (0x6000000))
extern u16* videoBuffer;
#define MAX_X (240)
#define MAX_Y (160)
#define ENABLE_SPRITE (1<<11) //12th bit
#define ONEDMAPPING (1<<5)

// *** Colors =========================================================

#define RGB(r,g,b) ((r)|((g)<<5)|((b)<<10))
#define WHITE RGB(31,31,31)
#define RED RGB(31,0,0)
#define GREEN RGB(0,31,0)
#define BLUE RGB(0,0,31)
#define BLACK RGB(0,0,0)
#define PURPLE RGB(12, 0, 25)
#define ORANGE RGB(31, 12, 0)
//rgb for beige 245, 241, 222
#define BEIGE RGB(23, 21, 15)
#define GRAY RGB(23, 23, 23)
#define FOOD_COLOR RGB(31, 1, 1)

// *** Timer stuff ====================================================

#define SCANLINECOUNTER *(volatile unsigned short *)0x4000006
#define TM_ON (1<<7)
#define TM_FREQ_1024  3
#define REG_TM0CNT *(volatile u16*)0x4000102
#define REG_TM0D *(volatile u16*)0x4000100
#define TM_IRQ (1<<6)

// *** Input =========================================================

// Buttons

#define BUTTON_A	(1<<0)
#define BUTTON_B	(1<<1)
#define BUTTON_SELECT	(1<<2)
#define BUTTON_START	(1<<3)
#define BUTTON_RIGHT	(1<<4)
#define BUTTON_LEFT	(1<<5)
#define BUTTON_UP	(1<<6)
#define BUTTON_DOWN	(1<<7)
#define BUTTON_R	(1<<8)
#define BUTTON_L	(1<<9)

#define BUTTONS *(volatile unsigned int *)0x4000130

#define KEY_DOWN_NOW(key)  (~(BUTTONS) & key)

// *** Other =========================================================
#define OFFSET(x, y, width) (y * width + x)
#define SIGN(x) ((x > 0) ? 1 : ((x < 0) ? -1 : 0)) 



/* DMA */

#define REG_DMA0SAD         *(const volatile u32*)0x40000B0 // source address
#define REG_DMA0DAD         *(volatile u32*)0x40000B4       // destination address
#define REG_DMA0CNT         *(volatile u32*)0x40000B8       // control register

// DMA channel 1 register definitions
#define REG_DMA1SAD         *(const volatile u32*)0x40000BC // source address
#define REG_DMA1DAD         *(volatile u32*)0x40000C0       // destination address
#define REG_DMA1CNT         *(volatile u32*)0x40000C4       // control register

// DMA channel 2 register definitions
#define REG_DMA2SAD         *(const volatile u32*)0x40000C8 // source address
#define REG_DMA2DAD         *(volatile u32*)0x40000CC       // destination address
#define REG_DMA2CNT         *(volatile u32*)0x40000D0       // control register

// DMA channel 3 register definitions
#define REG_DMA3SAD         *(const volatile u32*)0x40000D4 // source address
#define REG_DMA3DAD         *(volatile u32*)0x40000D8       // destination address
#define REG_DMA3CNT         *(volatile u32*)0x40000DC       // control register

typedef struct
{
    const volatile void *src;
    const volatile void *dst;
    unsigned int cnt;
} DMA_CONTROLLER;

#define DMA ((volatile DMA_CONTROLLER *) 0x040000B0)

// Defines
#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2
#define DMA_CHANNEL_3 3

#define DMA_DESTINATION_INCREMENT (0 << 21)
#define DMA_DESTINATION_DECREMENT (1 << 21)
#define DMA_DESTINATION_FIXED (2 << 21)
#define DMA_DESTINATION_RESET (3 << 21)

#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_DECREMENT (1 << 23)
#define DMA_SOURCE_FIXED (2 << 23)

#define DMA_REPEAT (1 << 25)

#define DMA_16 (0 << 26)
#define DMA_32 (1 << 26)

#define DMA_NOW (0 << 28)
#define DMA_AT_VBLANK (1 << 28)
#define DMA_AT_HBLANK (2 << 28)
#define DMA_AT_REFRESH (3 << 28)

#define DMA_IRQ (1 << 30)
#define DMA_ON (1 << 31)



/* Define pipe and struct stuff */
typedef struct pipeobj {
	int x;
	int botHeight;
	int topHeight;
	struct pipeobj *prev;
	struct pipeobj *next;
} pipe;

typedef struct {
	pipe *head;
	int numPipes;
} pipes;

#define STARTPOSX (50)
#define STARTPOSY (50)
#define SPACE_NEEDED (40)

#define PIPE_START_POS (70)
#define PIPE_SPACE (70)


// *** Prototypes =========================================================
void initGame();
void titleScreenFunc();
void setPixel(int, int, u16);
u16 getPixelColor(int, int);
void drawRect(int, int, int, int, u16);
void drawHollowRect(int, int, int, int, u16);
void plotLine(int, int, int, int, u16);
char* int2str(int);
void waitForVBlank();
void printBird(int, int, int);
int printBirdRow(int, int, int, int, int, int);
void drawImage3(int, int, int, int, const u16*);
void drawImage3UpsideDown(int, int, int, int, const u16*);
void drawFullWidth(int, int, int, const u16*);
void moveGrass();
void moveBird(int);
void generatePipes(pipes*);
void setRandHeight(pipe*);
int generateHeight();
void printPipe(pipe*);
void clearPipe(pipe*);
void movePipes();
void printScore();
void printHighScore();
void gameOverFunc();

#endif
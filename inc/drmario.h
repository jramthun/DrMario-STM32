#include "stm32f0xx.h"
#include "fifo.h"
#include "tty.h"
#include "lcd.h"

void drawBackground(char speed[]);
void setupDRMARIO(char speed[]);
void fillPillBottle(int pillLayout[16][8]);
void makeAllPillSegments(void);
void callNextPill(int pillOrder[2], int pillLayout[16][8]);
void moveRightPill(int pillLayout[16][8], int currentPosition[4]);
void moveLeftPill(int pillLayout[16][8], int currentPosition[4]);
void moveDownPill(int pillLayout[16][8], int currentPosition[4]);
void slamDownPill(int pillLayout[16][8], int currentPosition[4]);
int checkStop(int pillLayout[16][8], int currentPosition[4]);
int checkClear(int pillLayout[16][8]);
void rotateRight(int pillLayout[16][8], int currentPosition[4]);
void rotateLeft(int pillLayout[16][8], int currentPosition[4]);
void drawScoreP(int top, int score);
void UpdateScore(int score);
void UpdateTop(int top);
void showNextPiece(int pillOrder[4][2], int index);
int countVirus(int pillLayout[16][8]);

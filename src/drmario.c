#include "stm32f0xx.h"
#include "fifo.h"
#include "tty.h"
#include "lcd.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spritemap.h"
#include "control.h"

/* DR. MARIO GAME */
void drawBackground(char speed[]) { //TODO: change to support game speed (lo, med, hi)
	LCD_Clear(0000);
	LCD_direction(1);
	uint16_t x, y;
//	char speed[] = "HI";
	for (x = 0; x < LCD_H; x += 8)
		for (y = 0; y < LCD_W; y += 8)
			if ((x + y) % 16 == 0)
				LCD_DrawFillRectangle(x, y, x + 8, y + 8, 0x0000);
			else {
				if (strcmp(speed, " LO") == 0)
					LCD_DrawFillRectangle(x, y, x + 8, y + 8, 0x0281); // green check
				else if (strcmp(speed, "MED") == 0)
					LCD_DrawFillRectangle(x, y, x + 8, y + 8, 0x01cf); // blue check
				else
					LCD_DrawFillRectangle(x, y, x + 8, y + 8, 0x738E); // grey check
			}
}

void drawImg(u16 x0, u16 y0, u16 width, u16 height, const u16* img) {
	int x,y;
	for (y = 0; y < height; y++) {
		const u16* row = &img[y * width];
		for (x = 0; x < width; x++) {
			if (img[y*width + x] != 0xffff)
				LCD_DrawPoint(x0 + x, y0 + y, *row++);
			else
				row += 1;
		}
	}
}

void drawBottle(char speed[]) {
//	drawImg(120, 32, 80, 176, bottle_med);
//	char speed[] = "HI";
	u16 x0 = 120;
	u16 y0 = 32;
	u16 width = 80;
	u16 height = 176;
	const u16* img = bottle_med;
	int x,y;
	for (y = 0; y < height; y++) {
		const u16* row = &img[y * width];
		for (x = 0; x < width; x++) {
			if (img[y*width + x] != 0xffff){
				if (img[y*width + x] == 0x738E && strcmp(speed, " LO") == 0){
					LCD_DrawPoint(x0 + x, y0 + y, 0x0281);
					row += 1;
				} else if (img[y*width + x] == 0x738E && strcmp(speed,  "MED") == 0){
					LCD_DrawPoint(x0 + x, y0 + y, 0x3894);
					row += 1;
				}
				else
					LCD_DrawPoint(x0 + x, y0 + y, *row++);
			}
			else
				row += 1;
		}
	}
}

void drawScoreP(int top, int score) {
	drawImg(40, 26, 72, 78, clipboard_small);

	char tst[8] = {0};
	char sst[8] = {0};
	sprintf(tst, "%07d", top);
	sprintf(sst, "%07d", score);
	LCD_DrawString(48, 46, 0x0000, 0xA53B, "TOP", 8, 0x0);
	LCD_DrawString(48, 55, 0x0000, 0xA53B, tst, 8, 0x0);
	LCD_DrawString(48, 70, 0x0000, 0xA53B, "SCORE", 8, 0x0);
	LCD_DrawString(48, 79, 0x0000, 0xA53B, sst, 8, 0x0);
}

void UpdateScore(int score) {
	char sst[8] = {0};
	sprintf(sst, "%07d", score);
	LCD_DrawString(48, 79, 0x0000, 0xA53B, sst, 8, 0x0);
}

void UpdateTop(int top) {
	char tst[8] = {0};
	sprintf(tst, "%07d", top);
	LCD_DrawString(48, 55, 0x0000, 0xA53B, tst, 8, 0x0);

}
void drawGameStatus(int level, char spd[], int virus) {
	drawImg(208, 114, 64, 102, clipboard_large);

	char lvl[3];
	char vrs[3];
	sprintf(lvl, "%02d", level);
	sprintf(vrs, "%02d", virus);
	LCD_DrawString(216, 134, 0x0000, 0xA53B, "LEVEL", 8, 0x0);
	LCD_DrawString(248, 143, 0x0000, 0xA53B, lvl, 8, 0x0);
	LCD_DrawString(216, 158, 0x0000, 0xA53B, "SPEED", 8, 0x0);
	LCD_DrawString(240, 167, 0x0000, 0xA53B, spd, 8, 0x0);
	LCD_DrawString(216, 182, 0x0000, 0xA53B, "VIRUS", 8, 0x0);
	LCD_DrawString(248, 191, 0x0000, 0xA53B, vrs, 8, 0x0);
}

void gameBoundary(void) {
	// 128 H * 64W
	LCD_DrawFillRectangle(128, 72, 191, 199, 0xf81f);
}

typedef struct {
	uint16_t dc_x; //first corner
	uint16_t dc_y;
	uint16_t* color_data;
} pill_s;

void makeAllPillSegments(void) {
//	pill* r1 = malloc(sizeof(pill));
//	LCD_DrawFillRectangle(r1->dc_x, r1->dc_y, r1->ic_x, r1->ic_y, r1->color);
	drawImg(152, 80, 8, 8, pill_ref[2]);
	drawImg(160, 80, 8, 8, pill_ref[1]);
	drawImg(152, 88, 8, 8, pill_ref[0]);
}

void setupDRMARIO(char speed[]) {
	LCD_Setup();
	init_lcd_spi();
	drawBackground(speed);
	drawBottle(speed);
	drawScoreP(0, 0);
	drawGameStatus(1, speed, 0);
	drawImg(32, 120, 80, 88, magnifying_glass);
	drawImg(62, 130, 24, 22, ylw_virus);
	drawImg(79, 156, 23, 24, blu_virus);
	drawImg(52, 160, 24, 24, red_virus);
	drawImg(208, 56, 56, 56, doctor_window);
	drawImg(222, 68, 25, 40, dr_mario);
	drawImg(192, 20, 88, 29, game_title);
	enableController();
	// drawImg(230, 100, 8, 8, pill_ref[2]);
	// gameBoundary();
	// makeAllPillSegments();

//	LCD_DrawString(10, 200, 0x0000, 0xFFFF, "0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ", 8, 0x0); // DEBUG FONT
}


// fill the botle again
void fillPillBottle(int bactOrigLayout[16][8]) {
	// draw first line of holding bacteria
	for(int j = 0; j < 16; j++) {
		for(int i = 0; i < 8;i++) {
			if(bactOrigLayout[j][i] != -1 && bactOrigLayout[j][i] <= 3) {
				drawImg(120+8+8*i, 32+160-8*j, 8, 8, pill_ref[bactOrigLayout[j][i]]);
			} else if (bactOrigLayout[j][i] != -1 && bactOrigLayout[j][i] > 3) {
				drawImg(120+8+8*i, 32+160-8*j, 8, 8, virus_ref[bactOrigLayout[j][i] - 4]);
			}
		}
	}
}

void callNextPill(int pillOrder[2], int pillLayout[16][8]) {
	pillLayout[15][4] = pillOrder[0];
	pillLayout[15][3] = pillOrder[1];

}

void moveRightPill(int pillLayout[16][8], int currentPosition[4], int unison) {
	int moved = 0;
	if ((currentPosition[2] + 1 <= 7) && (currentPosition[0] + 1 <= 7) && ((currentPosition[2] != 100) || (currentPosition[0] != 100))) {
		if ((currentPosition[0] == (currentPosition[2] + 1)) && (pillLayout[currentPosition[1]][currentPosition[0] + 1] == 3)) {
			pillLayout[currentPosition[1]][currentPosition[0] + 1] = pillLayout[currentPosition[1]][currentPosition[0]];
			pillLayout[currentPosition[3]][currentPosition[2] + 1] = pillLayout[currentPosition[3]][currentPosition[2]];
			moved = 1;
		}
		else if((currentPosition[0] == (currentPosition[2] - 1)) && (pillLayout[currentPosition[3]][currentPosition[2] + 1] == 3)) {
			pillLayout[currentPosition[3]][currentPosition[2] + 1] = pillLayout[currentPosition[3]][currentPosition[2]];
			pillLayout[currentPosition[1]][currentPosition[0] + 1] = pillLayout[currentPosition[1]][currentPosition[0]];
			moved = 2;
		}
		else if(((currentPosition[3] == (currentPosition[1] + 1)) || (currentPosition[3] == (currentPosition[1] - 1))) && (((pillLayout[currentPosition[1]][currentPosition[0] + 1] == 3) && (pillLayout[currentPosition[3]][currentPosition[2] + 1] == 3)))) {
			pillLayout[currentPosition[3]][currentPosition[2] + 1] = pillLayout[currentPosition[3]][currentPosition[2]];
			pillLayout[currentPosition[1]][currentPosition[0] + 1] = pillLayout[currentPosition[1]][currentPosition[0]];
			moved = 3;
		}
		if((currentPosition[2] < 8) && (currentPosition[2] != 100) && (currentPosition[0] < 8) && (currentPosition[0] != 100) && (moved > 0)) {
				currentPosition[2] = currentPosition[2] + 1;
				currentPosition[0] = currentPosition[0] + 1;
		}
		if((currentPosition[1] != 100 && currentPosition[0] != 100) && (currentPosition[1] == currentPosition[3]) && (currentPosition[0] == (currentPosition[2] + 1)) && (moved == 1)) {
			pillLayout[currentPosition[3]][currentPosition[2] - 1] = 3;
		}
		else if((currentPosition[1] != 100 && currentPosition[0] != 100) && (currentPosition[1] == currentPosition[3]) && (currentPosition[0] == (currentPosition[2] - 1)) && (moved == 2)) {
			pillLayout[currentPosition[1]][currentPosition[0] - 1] = 3;
		}
		else if((currentPosition[1] != 100 && currentPosition[0] != 100) && ((currentPosition[1] == currentPosition[3] + 1) || (currentPosition[1] == currentPosition[3] - 1)) && (currentPosition[0] == (currentPosition[2])) && (moved == 3)) {
			pillLayout[currentPosition[1]][currentPosition[0] - 1] = 3;
			pillLayout[currentPosition[3]][currentPosition[2] - 1] = 3;
		}
	}
}

void moveLeftPill(int pillLayout[16][8], int currentPosition[4], int unison) {
	int moved = 0;
	if ((currentPosition[2] - 1 >= 0) && (currentPosition[0] >= 0) && ((currentPosition[2] != 100) || (currentPosition[0] != 100))) {
		if ((currentPosition[0] == (currentPosition[2] + 1)) && (pillLayout[currentPosition[3]][currentPosition[2] - 1] == 3)) {
			pillLayout[currentPosition[3]][currentPosition[2] - 1] = pillLayout[currentPosition[3]][currentPosition[2]];
			pillLayout[currentPosition[1]][currentPosition[0] - 1] = pillLayout[currentPosition[1]][currentPosition[0]];
			moved = 1;
		}
		else if((currentPosition[0] == (currentPosition[2] - 1)) && (pillLayout[currentPosition[1]][currentPosition[0] - 1] == 3)) {
			pillLayout[currentPosition[1]][currentPosition[0] - 1] = pillLayout[currentPosition[1]][currentPosition[0]];
			pillLayout[currentPosition[3]][currentPosition[2] - 1] = pillLayout[currentPosition[3]][currentPosition[2]];
			moved = 2;
		}
		else if(((currentPosition[3] == (currentPosition[1] + 1)) || (currentPosition[3] == (currentPosition[1] - 1))) && (((pillLayout[currentPosition[1]][currentPosition[0] - 1] == 3) && (pillLayout[currentPosition[3]][currentPosition[2] - 1] == 3)))) {
			pillLayout[currentPosition[3]][currentPosition[2] - 1] = pillLayout[currentPosition[3]][currentPosition[2]];
			pillLayout[currentPosition[1]][currentPosition[0] - 1] = pillLayout[currentPosition[1]][currentPosition[0]];
			moved = 3;
		}
		if((currentPosition[2] - 1 >= 0) && (currentPosition[2] != 100) && (currentPosition[0] >= 0) && (currentPosition[0] != 100) && (moved > 0)) {
				currentPosition[2] = currentPosition[2] - 1;
				currentPosition[0] = currentPosition[0] - 1;
		}
		if((currentPosition[1] != 100 && currentPosition[0] != 100) && (currentPosition[1] == currentPosition[3]) && (currentPosition[0] == (currentPosition[2] + 1)) && (moved == 1)) {
			pillLayout[currentPosition[1]][currentPosition[0] + 1] = 3;
		}
		else if((currentPosition[1] != 100 && currentPosition[0] != 100) && (currentPosition[1] == currentPosition[3]) && (currentPosition[0] == (currentPosition[2] - 1)) && (moved == 2)) {
			pillLayout[currentPosition[3]][currentPosition[2] + 1] = 3;
		}
		else if((currentPosition[1] != 100 && currentPosition[0] != 100) && ((currentPosition[1] == currentPosition[3] + 1) || (currentPosition[1] == currentPosition[3] - 1)) && (currentPosition[0] == (currentPosition[2])) && (moved == 3)) {
			pillLayout[currentPosition[1]][currentPosition[0] + 1] = 3;
			pillLayout[currentPosition[3]][currentPosition[2] + 1] = 3;
		}
	}
}

int checkStop(int pillLayout[16][8], int currentPosition[4]) {
	if(currentPosition[3] == currentPosition[1]) {
		if(currentPosition[3] != 100 || currentPosition[2] != 100 || (currentPosition[1] != 100 || currentPosition[0] != 100)) {
			if((pillLayout[currentPosition[3] - 1][currentPosition[2]] != 3) || (pillLayout[currentPosition[1] - 1][currentPosition[0]] != 3)) {
				currentPosition[3] = 100;
				currentPosition[2] = 100;
				currentPosition[1] = 100;
				currentPosition[0] = 100;
			}
		}
	}
	else if(currentPosition[3] == currentPosition[1] + 1) {
		if(currentPosition[3] != 100 || currentPosition[2] != 100) {
			if((pillLayout[currentPosition[1] - 1][currentPosition[0]] != 3)) {
				currentPosition[3] = 100;
				currentPosition[2] = 100;
				currentPosition[1] = 100;
				currentPosition[0] = 100;
			}
		}
	}
	else if(currentPosition[3] == currentPosition[1] - 1) {
		if(currentPosition[3] != 100 || currentPosition[2] != 100) {
			if((pillLayout[currentPosition[3] - 1][currentPosition[2]] != 3)) {
				currentPosition[3] = 100;
				currentPosition[2] = 100;
				currentPosition[1] = 100;
				currentPosition[0] = 100;
			}
		}
	}
	return 0;
}

void scrollDownPill(int pillLayout[16][8]) {
	for (int k = 0; k < 3; k++){
		for(int j = 15; j > 1; j--) {
			for(int i = 0; i < 8; i++) {
				if((pillLayout[j][i] < 3 && pillLayout[j - 1][i] == 3) && (pillLayout[j][i+1] >= 3) && (pillLayout[j][i-1] >= 3)) {
					pillLayout[j - 1][i] = pillLayout[j][i];
					pillLayout[j][i] = 3;
				}
			}
		}
	}
}

void moveDownPill(int pillLayout[16][8], int currentPosition[4]) {
	if(currentPosition[1] == currentPosition[3]) {
		if(currentPosition[2] != 100 || currentPosition[3] != 100) {
			pillLayout[currentPosition[3] - 1][currentPosition[2]] = pillLayout[currentPosition[3]][currentPosition[2]];
		}
		if((currentPosition[1] != 100 || currentPosition[0] != 100)) {
			pillLayout[currentPosition[1] - 1][currentPosition[0]] = pillLayout[currentPosition[1]][currentPosition[0]];
		}
		if(currentPosition[3] != 100 || currentPosition[2] != 100) {
			pillLayout[currentPosition[3]][currentPosition[2]] = 3;
		}
		if(currentPosition[1] != 100 || currentPosition[0] != 100) {
			pillLayout[currentPosition[1]][currentPosition[0]] = 3;
		}
	}
	else if(currentPosition[1] == (currentPosition[3] - 1)) {
		if((currentPosition[1] != 100 || currentPosition[0] != 100)) {
			pillLayout[currentPosition[1] - 1][currentPosition[0]] = pillLayout[currentPosition[1]][currentPosition[0]];
		}
		if(currentPosition[2] != 100 || currentPosition[3] != 100) {
			pillLayout[currentPosition[3] - 1][currentPosition[2]] = pillLayout[currentPosition[3]][currentPosition[2]];
		}
		/*if((currentPosition[1] != 100 || currentPosition[0] != 100)) {
			pillLayout[currentPosition[1] - 1][currentPosition[0]] = pillLayout[currentPosition[1]][currentPosition[0]];
		}*/
		if(currentPosition[3] != 100 || currentPosition[2] != 100) {
			pillLayout[currentPosition[3]][currentPosition[2]] = 3;
		}
	}
	else if(currentPosition[1] == currentPosition[3] + 1) {
		if((currentPosition[1] != 100 || currentPosition[0] != 100)) {
			pillLayout[currentPosition[3] - 1][currentPosition[2]] = pillLayout[currentPosition[3]][currentPosition[2]];
		}
		if(currentPosition[2] != 100 || currentPosition[3] != 100) {
			pillLayout[currentPosition[1] - 1][currentPosition[0]] = pillLayout[currentPosition[1]][currentPosition[0]];
		}
		if(currentPosition[3] != 100 || currentPosition[2] != 100) {
			pillLayout[currentPosition[1]][currentPosition[0]] = 3;
		}
	}
	if(currentPosition[1] != 100) {
		currentPosition[1] = currentPosition[1] - 1;
	}
	if(currentPosition[3] != 100) {
		currentPosition[3] = currentPosition[3] - 1;
	}
	// checkStop(pillLayout, currentPosition);
}

void slamDownPill(int pillLayout[16][8], int currentPosition[4]) {
	TIM7->CR1 &= ~TIM_CR1_CEN;
	while(currentPosition[0] != 100 || currentPosition[2] != 100) {
		moveDownPill(pillLayout, currentPosition);
		checkStop(pillLayout, currentPosition);
	}
	TIM7->CR1 |= TIM_CR1_CEN;
}

int checkClear(int pillLayout[16][8]) {
	// check horizontal
	int currentRow[8];
	int score = 0;
	for(int j = 0; j < 16; j++) {
		for(int i = 0; i < 8; i++) {
			currentRow[i] = pillLayout[j][i];
		}
		for(int i = 0; i <= 0; i++) {
			if(currentRow[i]%4 == currentRow[i+1]%4 && (currentRow[i]%4 == currentRow[i+2]%4) && (currentRow[i]%4 == currentRow[i+3]%4) && currentRow[i]%4 == currentRow[i+4]%4 && (currentRow[i]%4 == currentRow[i+5]%4) && (currentRow[i]%4 == currentRow[i+6]%4) && (currentRow[i]%4 == currentRow[i+7]%4) && (pillLayout[j][i] != 3)) {
				pillLayout[j][i] = 3;
				pillLayout[j][i+1] = 3;
				pillLayout[j][i+2] = 3;
				pillLayout[j][i+3] = 3;
				pillLayout[j][i+4] = 3;
				pillLayout[j][i+5] = 3;
				pillLayout[j][i+6] = 3;
				pillLayout[j][i+7] = 3;
				score = score +1000;
			}
		}
		for(int i = 0; i <= 1; i++) {
			if(currentRow[i]%4 == currentRow[i+1]%4 && (currentRow[i]%4 == currentRow[i+2]%4) && (currentRow[i]%4 == currentRow[i+3]%4) && currentRow[i]%4 == currentRow[i+4]%4 && (currentRow[i]%4 == currentRow[i+5]%4) && (currentRow[i]%4 == currentRow[i+6]%4) && (pillLayout[j][i] != 3)) {
				pillLayout[j][i] = 3;
				pillLayout[j][i+1] = 3;
				pillLayout[j][i+2] = 3;
				pillLayout[j][i+3] = 3;
				pillLayout[j][i+4] = 3;
				pillLayout[j][i+5] = 3;
				pillLayout[j][i+6] = 3;
				score = score + 500;
			}
		}
		for(int i = 0; i <= 2; i++) {
			if(currentRow[i]%4 == currentRow[i+1]%4 && (currentRow[i]%4 == currentRow[i+2]%4) && (currentRow[i]%4 == currentRow[i+3]%4) && (currentRow[i]%4 == currentRow[i+4]%4) && (currentRow[i]%4 == currentRow[i+5]%4) && (pillLayout[j][i] != 3)) {
				pillLayout[j][i] = 3;
				pillLayout[j][i+1] = 3;
				pillLayout[j][i+2] = 3;
				pillLayout[j][i+3] = 3;
				pillLayout[j][i+4] = 3;
				pillLayout[j][i+5] = 3;
				score = score + 300;
			}
		}
		for(int i = 0; i <= 3; i++) {
			if(currentRow[i]%4 == currentRow[i+1]%4 && (currentRow[i]%4 == currentRow[i+2]%4) && currentRow[i]%4 == currentRow[i+3]%4 && currentRow[i]%4 == currentRow[i+4]%4 && (pillLayout[j][i] != 3)) {
				pillLayout[j][i] = 3;
				pillLayout[j][i+1] = 3;
				pillLayout[j][i+2] = 3;
				pillLayout[j][i+3] = 3;
				pillLayout[j][i+4] = 3;
				score = score + 100;
			}
		}
		// 4 block horizontal clear
		for(int i = 0; i <= 4; i++) {
			if(currentRow[i]%4 == currentRow[i+1]%4 && (currentRow[i]%4 == currentRow[i+2]%4) && currentRow[i+2]%4 == currentRow[i+3]%4 && (pillLayout[j][i] != 3)) {
				pillLayout[j][i] = 3;
				pillLayout[j][i+1] = 3;
				pillLayout[j][i+2] = 3;
				pillLayout[j][i+3] = 3;
				score = score + 50;
			}
		}
	}
	// check vertical
	for(int j = 0; j < 8; j++) {
		int currentColumn[16];
		// 4 block horizontal clear
		for(int i = 0; i <= 15; i++) {
			currentColumn[i] = pillLayout[i][j];
		}
		for(int x = 0; x <= 9; x++) {
			if(currentColumn[x]%4 == currentColumn[x+1]%4 && (currentColumn[x]%4 == currentColumn[x+2]%4) && currentColumn[x]%4 == currentColumn[x+3]%4 && currentColumn[x]%4 == currentColumn[x+4]%4 && (currentColumn[x]%4 == currentColumn[x+5]%4) && (currentColumn[x]%4 == currentColumn[x+6]%4) && (currentColumn[x]%4 == currentColumn[x+7]%4)&& (pillLayout[x][j] != 3)) {
				pillLayout[x][j] = 3;
				pillLayout[x+1][j] = 3;
				pillLayout[x+2][j] = 3;
				pillLayout[x+3][j] = 3;
				pillLayout[x+4][j] = 3;
				pillLayout[x+5][j] = 3;
				pillLayout[x+6][j] = 3;
				pillLayout[x+7][j] = 3;
				score = score + 1000;
			}
		}
		for(int x = 0; x <= 10; x++) {
			if(currentColumn[x]%4 == currentColumn[x+1]%4 && (currentColumn[x]%4 == currentColumn[x+2]%4) && currentColumn[x]%4 == currentColumn[x+3]%4 && currentColumn[x]%4 == currentColumn[x+4]%4 && (currentColumn[x]%4 == currentColumn[x+5]%4) && (currentColumn[x]%4 == currentColumn[x+6]%4) && (pillLayout[x][j] != 3)) {
				pillLayout[x][j] = 3;
				pillLayout[x+1][j] = 3;
				pillLayout[x+2][j] = 3;
				pillLayout[x+3][j] = 3;
				pillLayout[x+4][j] = 3;
				pillLayout[x+5][j] = 3;
				pillLayout[x+6][j] = 3;
				score = score + 500;
			}
		}
		for(int x = 0; x <= 11; x++) {
			if(currentColumn[x]%4 == currentColumn[x+1]%4 && (currentColumn[x]%4 == currentColumn[x+2]%4) && currentColumn[x]%4 == currentColumn[x+3]%4 && currentColumn[x]%4 == currentColumn[x+4]%4 && (currentColumn[x]%4 == currentColumn[x+5]%4) && (pillLayout[x][j] != 3)) {
				pillLayout[x][j] = 3;
				pillLayout[x+1][j] = 3;
				pillLayout[x+2][j] = 3;
				pillLayout[x+3][j] = 3;
				pillLayout[x+4][j] = 3;
				pillLayout[x+5][j] = 3;
				score = score + 300;
			}
		}
		for(int x = 0; x <= 12; x++) {
			if(currentColumn[x]%4 == currentColumn[x+1]%4 && (currentColumn[x]%4 == currentColumn[x+2]%4) && currentColumn[x]%4 == currentColumn[x+3]%4 && currentColumn[x]%4 == currentColumn[x+4]%4 && (pillLayout[x][j] != 3)) {
				pillLayout[x][j] = 3;
				pillLayout[x+1][j] = 3;
				pillLayout[x+2][j] = 3;
				pillLayout[x+3][j] = 3;
				pillLayout[x+4][j] = 3;
				score = score + 50;
			}
		}
		for(int x = 0; x <= 13; x++) {
			if(currentColumn[x]%4 == currentColumn[x+1]%4 &&(currentColumn[x]%4 == currentColumn[x+2]%4)&& currentColumn[x]%4 == currentColumn[x+3]%4 && (pillLayout[x][j] != 3)) {
				pillLayout[x][j] = 3;
				pillLayout[x+1][j] = 3;
				pillLayout[x+2][j] = 3;
				pillLayout[x+3][j] = 3;
				score = score + 10;
			}
		}
	}
	return score;
}

void rotateRight(int pillLayout[16][8], int currentPosition[4]) {
	if(((currentPosition[2] != 100) || (currentPosition[0] != 100))) {
		if((currentPosition[1] == currentPosition[3]) && currentPosition[0] == (currentPosition[2] + 1)) {
			if((currentPosition[1] - 1 < 16) && pillLayout[currentPosition[1] - 1][currentPosition[0] - 1] == 3) {
					pillLayout[currentPosition[1] - 1][currentPosition[0] - 1] = pillLayout[currentPosition[1]][currentPosition[0]];
				currentPosition[1] = currentPosition[1] - 1;
				currentPosition[0] = currentPosition[0] - 1;
				pillLayout[currentPosition[1] + 1][currentPosition[0] + 1] = 3;
			}
		}
		else if((currentPosition[1] == (currentPosition[3] - 1)) && currentPosition[0] == (currentPosition[2])) {
			if(currentPosition[0] - 1 >= 0 && (pillLayout[currentPosition[1] + 1][currentPosition[0] - 1] == 3)) {
				pillLayout[currentPosition[1] + 1][currentPosition[0] - 1] = pillLayout[currentPosition[1]][currentPosition[0]];
				currentPosition[1] = currentPosition[1] + 1;
				currentPosition[0] = currentPosition[0] - 1;
				pillLayout[currentPosition[1] - 1][currentPosition[0] + 1] = 3;
			}
		}
		else if((currentPosition[1] == (currentPosition[3])) && currentPosition[0] == (currentPosition[2] - 1)) {
			if((currentPosition[1] + 1 < 16) && pillLayout[currentPosition[1] + 1][currentPosition[0] + 1] == 3) {
				pillLayout[currentPosition[1] + 1][currentPosition[0] + 1] = pillLayout[currentPosition[1]][currentPosition[0]];
				currentPosition[1] = currentPosition[1] + 1;
				currentPosition[0] = currentPosition[0] + 1;
				pillLayout[currentPosition[1] - 1][currentPosition[0] - 1] = 3;
			}
		}
	// back to original
		else if((currentPosition[1] == (currentPosition[3] + 1)) && currentPosition[0] == (currentPosition[2])) {
			if(currentPosition[0] + 1 < 8 && (pillLayout[currentPosition[1] - 1][currentPosition[0] + 1] == 3)) {
				pillLayout[currentPosition[1] - 1][currentPosition[0] + 1] = pillLayout[currentPosition[1]][currentPosition[0]];
				currentPosition[1] = currentPosition[1] - 1;
				currentPosition[0] = currentPosition[0] + 1;
				pillLayout[currentPosition[1] + 1][currentPosition[0] - 1] = 3;
			}
		}
	}
}

void rotateLeft(int pillLayout[16][8], int currentPosition[4]) {
	if(((currentPosition[2] != 100) || (currentPosition[0] != 100))) {
		if((currentPosition[1] == currentPosition[3]) && currentPosition[0] == (currentPosition[2] + 1)) {
			if((currentPosition[3] + 1 < 16) && pillLayout[currentPosition[3] + 1][currentPosition[2] + 1] == 3) {
				pillLayout[currentPosition[3] + 1][currentPosition[2] + 1] = pillLayout[currentPosition[3]][currentPosition[2]];
				currentPosition[3] = currentPosition[3] + 1;
				currentPosition[2] = currentPosition[2] + 1;
				pillLayout[currentPosition[3] - 1][currentPosition[2] - 1] = 3;
			}
		}
		else if((currentPosition[1] == (currentPosition[3] - 1)) && currentPosition[0] == (currentPosition[2])) {
			if((currentPosition[2] + 1 < 8) && (pillLayout[currentPosition[3] - 1][currentPosition[2] + 1] == 3)) {
				pillLayout[currentPosition[3] - 1][currentPosition[2] + 1] = pillLayout[currentPosition[3]][currentPosition[2]];
				currentPosition[3] = currentPosition[3] - 1;
				currentPosition[2] = currentPosition[2] + 1;
				pillLayout[currentPosition[3] + 1][currentPosition[2] - 1] = 3;
			}
		}
		else if((currentPosition[1] == (currentPosition[3])) && currentPosition[0] == (currentPosition[2] - 1)) {
			if((currentPosition[3] - 1 < 16) && pillLayout[currentPosition[3] - 1][currentPosition[2] - 1] == 3) {
				pillLayout[currentPosition[3] - 1][currentPosition[2] - 1] = pillLayout[currentPosition[3]][currentPosition[2]];
				currentPosition[3] = currentPosition[3] - 1;
				currentPosition[2] = currentPosition[2] - 1;
				pillLayout[currentPosition[3] + 1][currentPosition[2] + 1] = 3;
			}
		}
		// back to original
		else if((currentPosition[1] == (currentPosition[3] + 1)) && currentPosition[0] == (currentPosition[2])) {
			if(currentPosition[2] - 1 >= 0 && (pillLayout[currentPosition[3] + 1][currentPosition[2] - 1] == 3)) {
				pillLayout[currentPosition[3] + 1][currentPosition[2] - 1] = pillLayout[currentPosition[3]][currentPosition[2]];
				currentPosition[3] = currentPosition[3] + 1;
				currentPosition[2] = currentPosition[2] - 1;
				pillLayout[currentPosition[3] - 1][currentPosition[2] + 1] = 3;
			}
		}
	}
}
void showNextPiece(int pillOrder[4][2], int index) {
	drawImg(208+13, 61, 8, 8, pill_ref[pillOrder[index][1]]);
	drawImg(208+21, 61, 8, 8, pill_ref[pillOrder[index][0]]);
}
int countVirus(int pillLayout[16][8]) {
	int numberOfVirus = 0;
	for(int j = 0; j < 16; j++) {
		for(int i = 0; i < 8;i++) {
			if(pillLayout[j][i] > 3) {
				numberOfVirus = numberOfVirus + 1;
			}
		}
	}
	char vrs[3];
	sprintf(vrs, "%02d", numberOfVirus);
	LCD_DrawString(248, 191, 0x0000, 0xA53B, vrs, 8, 0x0);
	return numberOfVirus;
}
/* END GAME */

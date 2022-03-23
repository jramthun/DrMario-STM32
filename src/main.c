#include "stm32f0xx.h"
#include "drmario.h"
#include "control.h"
#include "commands.h"
#include "music.h"
#include "spritemap.h"
#include <string.h>

extern uint8_t midifile[];
extern uint8_t maybe_tetris[];
char game_speed[4] = " LO";
//char game_speed[4] = "MED";
//char game_speed[4] = " HI";

void init_spi1_slow() {
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= (GPIO_MODER_MODER3_1 | GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1);
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFRL3 | GPIO_AFRL_AFRL4 | GPIO_AFRL_AFRL5);

	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	SPI1->CR1 &= ~SPI_CR1_SPE;
	SPI1->CR1 |= (SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI);
	SPI1->CR1 |= SPI_CR1_BR;
	SPI1->CR2 |= SPI_CR2_FRXTH | SPI_CR2_SSOE | SPI_CR2_NSSP;
	SPI1->CR1 |= SPI_CR1_SPE;
}

void enable_sdcard() {
	GPIOB->BRR = 1<<2;
}

void disable_sdcard() {
	GPIOB->BSRR = 1<<2;
}

void init_sdcard_io() {
	init_spi1_slow();
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= GPIO_MODER_MODER2_0;
	disable_sdcard();
}

void sdcard_io_high_speed() {
	SPI1->CR1 &= ~SPI_CR1_SPE;
	SPI1->CR1 &= ~SPI_CR1_BR;
//	SPI1->CR1 |= SPI_CR1_BR_0;
	SPI1->CR1 |= SPI_CR1_SPE;
}

void init_lcd_spi() {
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOAEN;
	GPIOB->MODER |= (GPIO_MODER_MODER8_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER14_0);
	GPIOB->ODR |= 1<<11;
	GPIOA->MODER |= GPIO_MODER_MODER0_0;
	GPIOA->ODR |= 1<<0;

	init_spi1_slow();
	sdcard_io_high_speed();
}

void init_controls() {
	GPIOC->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER10 | GPIO_MODER_MODER11); //up, down, left, right
	GPIOB->MODER &= ~(GPIO_MODER_MODER1 | GPIO_MODER_MODER2); // b, a
	GPIOC->PUPDR |= (GPIO_PUPDR_PUPDR5_1 | GPIO_PUPDR_PUPDR6_1 | GPIO_PUPDR_PUPDR10_1 | GPIO_PUPDR_PUPDR11_1);
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR1_1 | GPIO_PUPDR_PUPDR2_1);
}

int i = 0;
// Fix these stuff
int pillOrder[9][2] = {
		{0,0},
		{2,1},
		{1,2},
		{0,1},
		{1,0},
		{0,2},
		{2,2},
		{2,0},
		{1,1}
};
int pillLayout[2][16][8] = {
		{
			{3, 3, 3, 3, 3, 6, 3, 3},
			{3, 6, 3, 4, 3, 6, 3, 3},
			{3, 3, 3, 6, 3, 3, 4, 3},
			{3, 3, 3, 3, 3, 6, 4, 3},
			{6, 3, 5, 3, 3, 3, 3, 3},
			{5, 3, 6, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 6, 3, 3, 3},
			{3, 4, 5, 3, 3, 3, 6, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
		},
		{
			{3, 3, 3, 3, 3, 6, 3, 3},
			{3, 6, 3, 4, 3, 6, 3, 3},
			{3, 3, 3, 6, 3, 3, 4, 3},
			{3, 3, 3, 3, 3, 6, 4, 3},
			{6, 3, 5, 3, 3, 3, 3, 3},
			{5, 3, 6, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 6, 3, 3, 3},
			{3, 4, 5, 3, 3, 3, 6, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
		}
};

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void small_delay(void) {
	nano_wait(50000000);
}

void init_tim7() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 48000 - 1;
    TIM7->ARR = 100 - 1;
    TIM7->DIER |= TIM_DIER_UIE;
    TIM7->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= 1 << TIM7_IRQn;
}

int currentPosition[4] = {100, 100, 100, 100};
int unison = 0;
int top __attribute__((section(".persistent"), used));
char key = 0;
int curBoard = 0;
void gameSpeedMed(char speed[]);
void TIM7_IRQHandler(void) {
	TIM7->SR &= ~TIM_SR_UIF;
	if(currentPosition[0] == 100 && currentPosition[2] == 100 && (pillLayout[curBoard][15][3] == 3 && pillLayout[curBoard][15][4] == 3)) {
    	callNextPill(pillOrder[i], pillLayout[curBoard]);
    	currentPosition[0] = 4;
    	currentPosition[1] = 15;
    	currentPosition[2] = 3;
    	currentPosition[3] = 15;
		// showNextPiece(pillOrder, i);
    	i++;
	}
	// fail condition
//	else if (((pillLayout[curBoard][15][3] != 3 || pillLayout[curBoard][15][4] != 3) && (pillLayout[curBoard][14][3] != 3 || pillLayout[curBoard][14][4] != 3)) || checkStop(pillLayout[curBoard], currentPosition) == 1){
//		UpdateTop(999999);
//    	/* if ((GPIOC->IDR & GPIO_IDR_5)) { // up, start
//    		callNextPill(pillOrder[i], pillLayout);
//    		currentPosition[0] = 4;
//    		currentPosition[1] = 15;
//    		currentPosition[2] = 3;
//    		currentPosition[3] = 15;
//    		unison = 1;
//    		i++;
//    	}*/
//		/*if (GPIOC->IDR & GPIO_IDR_6) { // down
//    		moveDownPill(pillLayout, currentPosition, unison);
//    	} else if (GPIOC->IDR & GPIO_IDR_10) { // left
//    		moveRightPill(pillLayout, currentPosition, unison);
//    	} else if (GPIOC->IDR & GPIO_IDR_11) { // right
//    		moveLeftPill(pillLayout, currentPosition, unison);*/
//	}
	checkStop(pillLayout[curBoard], currentPosition);
	if(curBoard > 2) {
		curBoard = 0;
	}
}
void gameDevMode(int score) {
    fillPillBottle(pillLayout[curBoard]);
    // int unison = 0;
	for(;;) {
		showNextPiece(pillOrder, i);
		drawScoreP((currentPosition[2]*1000)+(currentPosition[3]), (currentPosition[0]*1000)+(currentPosition[1]));
//		char key = get_keypress();
//	    if (key == 'A') {
//	        makeAllPillSegments();
//	    }
//	    else if (key == 'B') {
//	        // setrgb(getrgb());
//	    	callNextPill(pillOrder[i], pillLayout[curBoard]);
//	    	currentPosition[0] = 4;
//	    	currentPosition[1] = 15;
//	    	currentPosition[2] = 3;
//	    	currentPosition[3] = 15;
//	    }
//	    else if(key == '4') {
//	    	moveLeftPill(pillLayout[curBoard], currentPosition);
//	    }
//	    else if(key == '6') {
//	    	moveRightPill(pillLayout[curBoard], currentPosition);
//	    }
//	    else if(key == '8') {
//	    	moveDownPill(pillLayout[curBoard], currentPosition);
//	    }
//	    else if(key == '#') {
//	    	rotateRight(pillLayout[curBoard], currentPosition);
//	    }
//	    else if(key == '*') {
//	    	rotateLeft(pillLayout[curBoard], currentPosition);
//	    }
//	    else if(key == '0') {
//	    	slamDownPill(pillLayout[curBoard], currentPosition);
//	    }
	    // else if(key == '')

	    if ((GPIOC->IDR & GPIO_IDR_5)) { // up, start
	    	callNextPill(pillOrder[i], pillLayout[curBoard]);
			currentPosition[0] = 4;
			currentPosition[1] = 15;
			currentPosition[2] = 3;
			currentPosition[3] = 15;
		} else if (GPIOC->IDR & GPIO_IDR_6) { // down
			slamDownPill(pillLayout[curBoard], currentPosition);
		} else if (GPIOC->IDR & GPIO_IDR_10) { // right
			moveRightPill(pillLayout[curBoard], currentPosition);
		} else if (GPIOC->IDR & GPIO_IDR_11) { // left
			moveLeftPill(pillLayout[curBoard], currentPosition);
		} else if (GPIOB->IDR & GPIO_IDR_1) { // b
			rotateRight(pillLayout[curBoard], currentPosition);
		} else if (GPIOB->IDR & GPIO_IDR_2) { // a
			rotateLeft(pillLayout[curBoard], currentPosition);
		}

	    checkStop(pillLayout[curBoard], currentPosition);
	    checkClear(pillLayout[curBoard]);
	    fillPillBottle(pillLayout[curBoard]);
	    if(i > 3) {
	    	i = 0;
	    }
	}
}

void pauseGame() {
	while (GPIOC->IDR & GPIO_IDR_5);
	while (1){
		if ((GPIOC->IDR & GPIO_IDR_5))
			break;
	}
}

void gameSpeedMed(char speed[]) {
	MIDI_Player* mp = init_music();
    fillPillBottle(pillLayout[curBoard]);
    int vrs_cnt = countVirus(pillLayout[curBoard]);
    if (top % 100) top = 0;
    UpdateTop(top);
    int score = 0;
    int prevScore = 0;
    int score_chart[6] = {100, 200, 400, 800, 1600, 3200};
    int mult;
    if (strcmp(speed, " LO") == 0)
		mult = 1;
	else if (strcmp(speed, "MED") == 0)
		mult = 2;
	else
		mult = 3;

	for(;;) {
		int new_vrs_cnt = countVirus(pillLayout[curBoard]);
		showNextPiece(pillOrder, i);
//		moveDownPill(pillLayout[curBoard], currentPosition);
		/*char key = get_keypress();
	    if (key == 'A') {
	        makeAllPillSegments();
	    }
	    else if (key == 'B') {
	        // setrgb(getrgb());
	    	callNextPill(pillOrder[i], pillLayout);
	    	currentPosition[0] = 4;
	    	currentPosition[1] = 15;
	    	currentPosition[2] = 3;
	    	currentPosition[3] = 15;
	    	unison = 1;
	    }
	    else if(key == '4') {
	    	moveLeftPill(pillLayout, currentPosition, unison);
	    }
	    else if(key == '6') {
	    	moveRightPill(pillLayout, currentPosition, unison);
	    }
	    else if(key == '8') {
	    	moveDownPill(pillLayout, currentPosition, unison);
	    }
	    else if(key == '#') {
	    	rotateRight(pillLayout, currentPosition);
	    }
	    else if(key == '*') {
	    	rotateLeft(pillLayout, currentPosition);
	    }
	    // else if(key == '')*/

		if ((GPIOC->IDR & GPIO_IDR_5)) { // up, start
//			callNextPill(pillOrder[i], pillLayout[curBoard]);
//			currentPosition[0] = 4;
//			currentPosition[1] = 15;
//			currentPosition[2] = 3;
//			currentPosition[3] = 15;
			pauseGame();
		} else if (GPIOC->IDR & GPIO_IDR_6) { // down
			slamDownPill(pillLayout[curBoard], currentPosition);
			small_delay();
		} else if (GPIOC->IDR & GPIO_IDR_10) { // right
			moveRightPill(pillLayout[curBoard], currentPosition);
			small_delay();
		} else if (GPIOC->IDR & GPIO_IDR_11) { // left
			moveLeftPill(pillLayout[curBoard], currentPosition);
			small_delay();
		} else if (GPIOB->IDR & GPIO_IDR_1) { // b
			rotateRight(pillLayout[curBoard], currentPosition);
			small_delay();
		} else if (GPIOB->IDR & GPIO_IDR_2) { // a
			rotateLeft(pillLayout[curBoard], currentPosition);
			small_delay();
		} else {
			moveDownPill(pillLayout[curBoard], currentPosition);
		}

	    checkStop(pillLayout[curBoard], currentPosition);
		small_delay();
	    fillPillBottle(pillLayout[curBoard]);
	    prevScore = score;
//	    score = prevScore + checkClear(pillLayout[curBoard]);
	    if (checkClear(pillLayout[curBoard]) > 0)
	    	scrollDownPill(pillLayout[curBoard]);
//	    int new_vrs_cnt = fillPillBottle(pillLayout);
		int vrs_dif = vrs_cnt - new_vrs_cnt;
		if  (vrs_dif > 6) vrs_dif = 6;
		if (vrs_dif > 0){
			score += score_chart[vrs_dif - 1] * mult;
			vrs_cnt = new_vrs_cnt;
//			if (score > top){
//				top = score;
//			}
		}
	    if(score > prevScore) {
	    	UpdateScore(score);
	    }
	    if(score > top) {
	    	top = score;
	    	UpdateTop(top);
	    }

	    // Failure Condition
	    if ((pillLayout[curBoard][15][3] != 3 && pillLayout[curBoard][15][4] != 3) && (pillLayout[curBoard][14][3] != 3 || pillLayout[curBoard][14][4] != 3)){
	    	drawImg(100, 68, 120, 104, game_over);
	    	break;
	    }
	    // Win Condition
	    if (vrs_cnt == 0) {
	    	drawImg(100, 52, 120, 136, stage_clear);
	    	break;
	    }

	    for (int i = (8 - (2*mult)); i > 0; i--){
	    	small_delay();
	    }
//	    small_delay();
//	    small_delay();
//	    small_delay();
//	    small_delay();
//	    small_delay();
	    // fillPillBottle(pillLayout);
	    if(i > (sizeof(pillOrder)/sizeof(pillOrder[0]) - 1)) {
	    	i = 0;
	    }
	    if (mp->nexttick >= 100000)
			midi_init(midifile);
	}
}

int main() {
//	init_usart5();
//    enable_tty_interrupt();
//    setbuf(stdin,0);
//    setbuf(stdout,0);
//    setbuf(stderr,0);
//	int score = 0;
    setupDRMARIO(game_speed);
    init_controls();
//    command_shell();
    /*int pillOrder[4][2] = {
    		{2, 1},
    		{1, 2},
    		{0, 1},
    		{0, 0},
    };
	int pillLayout[16][8] = {
			{3, 3, 3, 3, 3, 2, 3, 3},
			{3, 2, 3, 0, 3, 2, 3, 3},
			{3, 3, 3, 2, 3, 3, 0, 3},
			{3, 3, 3, 3, 3, 2, 0, 3},
			{2, 3, 1, 3, 3, 3, 3, 3},
			{1, 3, 2, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 2, 3, 3, 3},
			{3, 0, 1, 3, 3, 3, 2, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
			{3, 3, 3, 3, 3, 3, 3, 3},
	};*/
    fillPillBottle(pillLayout[curBoard]);
    // int unison = 0;
    // int currentPosition[4] = {100, 100, 100, 100};
	int i = 0;
//	gameDevMode(score);
	init_tim7();
	gameSpeedMed(game_speed);
	for(;;) {
		showNextPiece(pillOrder, i);
//		char key = get_keypress();
//	    if (key == 'A') {
//	        makeAllPillSegments();
//	    }
//	    else if (key == 'B') {
//	    	gameDevMode(score);
//	    }
//	    else if(key == 'C') {
//	    	init_tim7();
//	    	gameSpeedMed();
//	    }
	}
}

/*init_controls();
    for(;;){
    	showNextPiece(pillOrder, i);
    	//drawScoreP((currentPosition[2]*1000)+(currentPosition[3]), (currentPosition[0]*1000)+(currentPosition[1]));
    	if ((GPIOC->IDR & GPIO_IDR_5)) { // up, start
    		callNextPill(pillOrder[i], pillLayout);
    		currentPosition[0] = 4;
    		currentPosition[1] = 15;
    		currentPosition[2] = 3;
    		currentPosition[3] = 15;
    		unison = 1;
    		i++;
    	} else if (GPIOC->IDR & GPIO_IDR_6) { // down
    		moveDownPill(pillLayout, currentPosition, unison);
    	} else if (GPIOC->IDR & GPIO_IDR_10) { // left
    		moveRightPill(pillLayout, currentPosition, unison);
    	} else if (GPIOC->IDR & GPIO_IDR_11) { // right
    		moveLeftPill(pillLayout, currentPosition, unison);
    	} else if (GPIOB->IDR & GPIO_IDR_1) { // b
    		rotateRight(pillLayout, currentPosition);
    	} else if (GPIOB->IDR & GPIO_IDR_2) { // a
    		rotateLeft(pillLayout, currentPosition);
    	}
    	fillPillBottle(pillLayout);
    	if (checkClear(pillLayout)){
    		drawScoreP((currentPosition[2]*1000)+(currentPosition[3]), (currentPosition[0]*1000)+(currentPosition[1]));
    	}
    	if(i > 3) {i = 0;}
    }
*/

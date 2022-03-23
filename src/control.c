/*
 * control.c
 *
 *  Created on: Dec 3, 2021
 *      Author: garci
 */

#include "stm32f0xx.h"
#include "fifo.h"
#include "tty.h"
#include "lcd.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spritemap.h"


//void init_tim6();

// Enable Controller
void enableController(void) {

	RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
//	init_tim6();

    // clear bits for pc0 - pc7 then set them (4-7 outputs)(0-3 inputs) (MODER)
    GPIOC -> MODER &= ~0x0000ffff;
    GPIOC -> MODER |= 0x00005500;

    // clear bits for pc0 - pc4 then set them (0 - 3 outputs) (MODER)
    GPIOC -> PUPDR &= ~0x000000ff;
    GPIOC -> PUPDR |= 0x000000aa;
}
/*
// This is froim lab 6
//===========================================================================
// Debouncing a Keypad
//===========================================================================
const char keymap[] = "DCBA#9630852*741";
uint8_t hist[16];
uint8_t col;
char queue[2];
int qin;
int qout;

void drive_column(int c) {
    GPIOC->BSRR = 0xf00000 | (1 << (c + 4));
}

int read_rows() {
    return GPIOC->IDR & 0xf;
}

void push_queue(int n) {
    n = (n & 0xff) | 0x80;
    queue[qin] = n;
    qin ^= 1;
}

uint8_t pop_queue() {
    uint8_t tmp = queue[qout] & 0x7f;
    queue[qout] = 0;
    qout ^= 1;
    return tmp;
}

void update_history(int c, int rows) {
    for(int i = 0; i < 4; i++) {
        hist[4*c+i] = (hist[4*c+i]<<1) + ((rows>>i)&1);
        if (hist[4*c+i] == 1)
          push_queue(4*c+i);
    }
}

void init_tim6() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 48 - 1;
    TIM6->ARR = 1000 - 1;
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= 1 << TIM6_DAC_IRQn;
}

void TIM6_DAC_IRQHandler(void) {
    TIM6->SR &= ~TIM_SR_UIF;
    int rows = read_rows();
    update_history(col, rows);
    col = (col + 1) & 3;
    drive_column(col);
}

char get_keypress() {
    for(;;) {
        asm volatile ("wfi" : :);   // wait for an interrupt
        if (queue[qout] == 0)
            continue;
        return keymap[pop_queue()];
    }
}*/


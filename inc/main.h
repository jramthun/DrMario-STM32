#include "stm32f0xx.h"
#include "fifo.h"
#include "tty.h"

void init_usart5();
void enable_tty_interrupt();
void USART3_4_5_6_7_8_IRQHandler();
char interrupt_getchar();
int __io_putchar(int c);
int __io_getchar(void);
void init_spi1_slow();
void enable_sdcard();
void disable_sdcard();
void init_sdcard_io();
void sdcard_io_high_speed();
void init_lcd_spi();

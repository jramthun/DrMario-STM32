#include "stm32f0xx.h"
#include "midi.h"
#include "step-array.h"
#include <math.h>

#define N 1000

struct {
	uint8_t note;
	uint8_t chan;
	uint8_t volume;
	int     step;
	int     offset;
} voice[15];

extern uint8_t midifile[];
short int wavetable[N];
MIDI_Player *mp;

void init_TIM2(int filler) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->PSC = 48-1;
	//TIM2->ARR = (1000000/10417)-1;
	TIM2->ARR = filler-1;
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM2_IRQn);
}
void init_TIM6() {
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6->PSC = 1;
	TIM6->ARR = (48000000/RATE/2) - 1;
	TIM6->DIER |= TIM_DIER_UIE;
	TIM6->CR1 |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
void init_DAC() {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODER4;
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;
	DAC->CR &= ~DAC_CR_EN1;
	DAC->CR |= DAC_CR_TEN1;
	DAC->CR |= DAC_CR_TSEL1; //TODO
	//DAC->CR &= ~DAC_CR_TSEL1;
	DAC->CR |= DAC_CR_EN1;
}
void init_DMA() {
	RCC->AHBENR |= RCC_AHBENR_DMAEN;
	DMA1_Channel3->CCR &= ~DMA_CCR_EN;
	DMA1_Channel3->CMAR = (uint32_t) (&(GPIOA->ODR));
	DMA1_Channel3->CPAR = (uint32_t) (&(DAC->DHR12R1));
	DMA1_Channel3->CNDTR = 4;
	DMA1_Channel3->CCR |= DMA_CCR_DIR;

	DMA1_Channel3->CCR &= ~DMA_CCR_MSIZE;
	DMA1_Channel3->CCR |= DMA_CCR_MSIZE_0;
	DMA1_Channel3->CCR &= ~DMA_CCR_PSIZE;
	DMA1_Channel3->CCR |= DMA_CCR_PSIZE_0;
	DMA1_Channel3->CCR |= DMA_CCR_MINC;
	DMA1_Channel3->CCR &= ~DMA_CCR_PINC;
	DMA1_Channel3->CCR |= DMA_CCR_CIRC;
	DMA1_Channel3->CCR |= DMA_CCR_EN;
}
void TIM2_IRQHandler(void)
{
    // TODO: Remember to acknowledge the interrupt right here!
	TIM2->SR &= ~TIM_SR_UIF;
    midi_play();
}
void TIM6_DAC_IRQHandler(void) {
	DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
	//DAC->
	TIM6->SR &= ~TIM_SR_UIF;
	int sample = 0;
	for(int i=0; i < sizeof voice / sizeof voice[0]; i++) {
		if (voice[i].step != 0) {
			sample += (wavetable[voice[i].offset>>16] * voice[i].volume) << 4;
			voice[i].offset += voice[i].step;
			if ((voice[i].offset >> 16) >= sizeof wavetable / sizeof wavetable[0]){
				voice[i].offset -= (sizeof wavetable / sizeof wavetable[0]) << 16;
			}
		}
	}
	sample = (sample >> 16) + 2048;
	DAC->DHR12R1 = sample;
//	if (mp->nexttick >= MAXTICKS)
//		midi_init(midifile);
}

void set_tempo(int time, int value, const MIDI_Header *hdr)
{
	// This assumes that the TIM2 prescaler divides by 48.
	// It sets the timer to produce an interrupt every N
	// microseconds, where N is the new tempo (value) divided by
	// the number of divisions per beat specified in the MIDI header.
	TIM2->ARR = value/hdr->divisions - 1;
}

void note_on(int time, int chan, int key, int velo)
{
  for(int i=0; i < sizeof voice / sizeof voice[0]; i++) {
    if (voice[i].step == 0) {
    	// configure this voice to have the right step and volume
    	//...
    	voice[i].step = step[key];
    	voice[i].note = key;
    	voice[i].volume = velo/10;
    	break;
    }
  }
  return;
}

void note_off(int time, int chan, int key, int velo)
{
  for(int i=0; i < sizeof voice / sizeof voice[0]; i++){
    if (voice[i].step != 0 && voice[i].note == key) {
      // turn off this voice
      voice[i].step = 0;
      voice[i].note = 0;
      voice[i].volume = 0;
      break;
    }
  }
  return;
}

MIDI_Player* init_music(void)
{
	for(int i=0; i < N; i++){
	        wavetable[i] = 32767 * sin(2 * M_PI * i / N);
	}
    init_DAC();
    init_DMA();
    init_TIM6();
    MIDI_Player *mp = midi_init(midifile);
    // The default rate for a MIDI file is 2 beats per second
    // with 48 ticks per beat.  That's 500000/48 microseconds.
    init_TIM2(10417);
    // Nothing else to do at this point.
//    for(;;){
//        if (mp->nexttick >= MAXTICKS) {
//			 midi_init(midifile);
//		}
//        asm("wfi");
//    }
    return mp;
}

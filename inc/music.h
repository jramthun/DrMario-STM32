#include "stm32f0xx.h"
#include "midi.h"
//#include "step-array.h"
#include <math.h>

extern MIDI_Player *mp;
extern uint8_t midifile[];
MIDI_Player* init_music(void);

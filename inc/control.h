/*
 * control.h
 *
 *  Created on: Dec 3, 2021
 *      Author: garci
 */

#ifndef CONTROL_H_
#define CONTROL_H_

void enableController(void);
char get_keypress(void);
uint8_t pop_queue();
void push_queue(int n);

#endif /* CONTROL_H_ */

/*
 * commands.h
 *
 *  Created on: Dec 2, 2021
 *      Author: garci
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

struct commands_t {
    const char *cmd;
    void      (*fn)(int argc, char *argv[]);
};

void command_shell(void);

#endif /* COMMANDS_H_ */

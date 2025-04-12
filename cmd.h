#ifndef SSHELL_CMD_H_
#define SSHELL_CMD_H_

#include <stdio.h>

#include "sshell.h"

/*
 * This object holds information for command input that
 * is processed in the shell routine
 */
struct cmd{

    /* input typed into terminal*/
    char input[CMD_MAX_LENGTH];

    /*length of input*/
    int length;

    /*Arguments parsed from input. args[0] is the command*/
    char* args[MAX_ARG_LENGTH + 1];

    /*The tokenized input string. args[] points to this*/
    char argString[CMD_MAX_LENGTH + 1];

    /*The number of input arguments, includes the command*/
    int numberOfArgs;
};

struct cmd* cmdConstructor();

/*
 * Retrieves shell entry from command prompt
 *
 * If EOF is encountered, user input is set to "exit",
 * which allows the shell to stop.
 *
 * If the input isnt from the terminal, the
 * input will be written to the file
 * stdout is set to.
 *
 * Params: cmd->shell input
 *
 */
void getCmd(struct cmd* cmd);

/*
 * The shell input is tokenized and converted into
 * arguments.
 *
 * Params: cmd->the shell input
 * Returns: 1 if input is adequate length, 0 otherwise
 *
 * Throws: Error if command line has too many arguments
 */
int parseArgs(struct cmd* cmd);

#endif 

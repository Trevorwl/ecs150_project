#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#include "utils.h"

#include "sshell.h"
#include "cmd.h"

/*
 * sshell.c contains the main shell routine, which takes user input
 * and parses it into commands and arguments.
 *
 * Input is first parsed with getCmd(), which sets the command as "exit" if
 * input is EOF. getCmd() also sends user input to a file if stdin isnt connected
 * to the terminal.
 *
 * parseArgs() takes use input and converts it to command and arguments,
 * throwing an error if arguments exceed MAX_ARG_LENGTH.
 *
 * doFork() takes the command and arguments and creates
 * a child process.
 */


/*
 * creates a fork using command line arguments
 *
 * Params: cmd->command line arguments
 *
 * Returns: The return value of the fork
 *
 * Throws: Error if invalid command
 */
int doFork(struct cmd* cmd){
    int returnVal;
    pid_t pid = fork();

    if(pid < 0){
        perror("fork");
        exit(1);
    }

    if(pid==0){
        execvp((cmd->args)[0],cmd->args);
        fprintf(stderr, "Error: command not found\n");
        exit(1);

    } else{
        wait(&returnVal);
    }

    return WEXITSTATUS(returnVal);
}


/*
 * The routine for the shell
 *
 * -User is prompted for input using getCmd()
 * -If "exit" is entered and found by getCmd(),shell exits
 * -When input is empty or whitespace, shell prompts again
 *
 * Command and arguments are parsed with parseArgs(),
 * and passed to doFork(), where the command is executed in another process
 */
int main(void) {
    struct cmd* cmd = cmdConstructor();

    while (1) {
        printf("sshell@ucd$ ");
        fflush(stdout);

        getCmd(cmd);

        if (!strcmp(cmd->input, "exit")) {
            fprintf(stderr, "Bye...\n");
            fprintf(stderr,"+ completed 'exit' [0]\n");
	    break;
        }

        if(cmd->length==0 || isWhiteSpace(cmd->input)){
            continue;
        }

        if(!parseArgs(cmd)){
            continue;
        }

        int returnVal = doFork(cmd);

        fprintf(stderr, "+ completed '%s' [%d]\n",
                cmd->input, returnVal);
    }

    free(cmd);

    return 0;
}





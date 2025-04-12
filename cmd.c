#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cmd.h"


struct cmd* cmdConstructor(){
    struct cmd* cmd = (struct cmd*)malloc(sizeof(struct cmd));
    if(cmd==NULL){
        perror("malloc failed");
        exit(1);
    }

    cmd->length=-1;
    cmd->numberOfArgs=-1;

    return cmd;
}

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
void getCmd(struct cmd* cmd){
    char* input=cmd->input;

    if (!fgets(input, CMD_MAX_LENGTH, stdin)){
        printf("\n");
        strncpy(input, "exit\n", CMD_MAX_LENGTH);
    }

    if (!isatty(STDIN_FILENO)) {
        printf("%s", input);
        fflush(stdout);
    }

    char* endOfCmd = strchr(input, '\n');
    if (endOfCmd){
        *endOfCmd = '\0';
    }

    cmd->length = strlen(input);
}

/*
 * The shell input is tokenized and converted into
 * arguments.
 *
 * Params: cmd->the shell input
 * Returns: 1 if input is adequate length, 0 otherwise
 *
 * Throws: Error if command line has too many arguments
 */
int parseArgs(struct cmd* cmd){
    strcpy(cmd->argString,cmd->input);

    char** args = cmd->args;

    char* token = strtok(cmd->argString," ");
    int count = 0;

    while(token!=NULL && count < MAX_ARG_LENGTH){
        args[count++]=token;
        token=strtok(NULL," ");
    }

    if(token!=NULL){
        fprintf(stderr, "Error: too many process arguments\n");
        return 0;
    }

    args[count]=NULL;
    cmd->numberOfArgs = count - 1;

    return 1;
}


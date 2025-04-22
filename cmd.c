#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cmd.h"
#include "sshell.h"
#include "utils.h"

struct cmd *cmdConstructor() {
    struct cmd *cmd = (struct cmd *)malloc(sizeof(struct cmd));
    if (cmd == NULL) {
        perror("malloc failed");
        exit(1);
    }

    cmd->length = -1;
    cmd->numberOfArgs = -1;
    cmd->is_background = 0;
    cmd->isLast = 0;
    cmd->isFirst = 0;


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
void getCmds(struct cmd *cmd) {
    char input[CMD_MAX_LENGTH + 1];

    if (!fgets(input, CMD_MAX_LENGTH, stdin)) {
        printf("\n");
        strncpy(input, "exit\n", CMD_MAX_LENGTH);
    }

    if (!isatty(STDIN_FILENO)) {
        printf("%s", input);
        fflush(stdout);
    }

    char *endOfCmd = strchr(input, '\n');
    if (endOfCmd) {
        *endOfCmd = '\0';
    }
    // —— ADD: detect missing command errors ——
    if (input[0] == '|' || input[0] == '>') {
        fprintf(stderr, "Error: missing command\n");
        return;
    }
    // Skip blank lines
    if (isWhiteSpace(input)) {
        return;
    }

    // Change all pipe characters | to '\0'
    // and then return the segmentation coordinate array
    int *offsets = parseInput(input);
    int pos = 0;
    struct cmd *now = cmd;
    now->isFirst = 1;
    while (offsets[pos]!=-1) {
        strcpy(now->input, input + offsets[pos]);
        pos++;
        if (offsets[pos]!=-1) {
            struct cmd *next = cmdConstructor();
            now->next = next;
            now = next;
        }else {
            now->isLast = 1;
            now->next = NULL;
        }
    }
    free(offsets);
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
int parseArgs(struct cmd *cmd) {
    cmd->length = strlen(cmd->input);
    strcpy(cmd->argString, cmd->input);
    cmd->argString[strlen(cmd->argString)] = ' ';
    cmd->argString[strlen(cmd->argString)+1] = '\0';
    cmd->is_background = 0;
    char **args = cmd->args;

    //  detect background '&'
    {
        char *bg = strchr(cmd->argString, '&');
        if (bg) {
            char *p = bg + 1;
            while (*p == ' ') p++;
            if (*p != '\0') {
                fprintf(stderr, "Error: mislocated background sign\n");
                return 0;
            }
            cmd->is_background = 1;
            *bg = ' ';  // remove '&'
        }
    }

    // detect > <
    char* outputRedirect = strchr(cmd->argString, '>');
    if (outputRedirect) {
        *outputRedirect = ' ';
        if(cmd->isLast==0) {
            fprintf(stderr, "Error: mislocated output redirection\n");
            fflush(stderr);
        }
        // find output file
        char * ptr = outputRedirect+1;
        if(ptr && *ptr==' ') ptr++;
        char * ptr_n = strchr(ptr, ' ');
        if (ptr_n==NULL) {
            fprintf(stderr, "Error: no output file\n");
            fflush(stderr);
        }else {
            // copy file name
            *ptr_n = '\0';
            strcpy(cmd->out_file, ptr);
            *(ptr-1) = '\0';
        }
    }
    char* inputRedirect = strchr(cmd->argString, '<');
    if(inputRedirect) {
        *inputRedirect = ' ';
        if(cmd->isFirst==0) {
            fprintf(stderr, "Error: mislocated input redirection\n");
            fflush(stderr);
        }
        // find input file
        char * ptr = inputRedirect+1;
        if(ptr && *ptr==' ') ptr++;
        char * ptr_n = strchr(ptr, ' ');
        if (ptr_n==NULL) {
            fprintf(stderr, "Error: no input file\n");
            fflush(stderr);
        }else {
            // copy file name
            *ptr_n = '\0';
            strcpy(cmd->in_file, ptr);
            *(ptr-1) = '\0';
        }
    }

    char *token = strtok(cmd->argString, " ");
    int count = 0;

    while (token != NULL && count < MAX_ARG_LENGTH) {
        args[count++] = token;
        token = strtok(NULL, " ");
    }

    if (token != NULL) {
        fprintf(stderr, "Error: too many process arguments\n");
        return 0;
    }

    args[count] = NULL;
    cmd->numberOfArgs = count - 1;

    return 1;
}

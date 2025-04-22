#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>


#include "utils.h"
#include "pidSet.h"
#include "sshell.h"
#include "cmd.h"

struct vector* backgroundTasks;

char commandLine[CMD_MAX_LENGTH + 1];

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

int doBuiltinTask(struct cmd* cmd){
    if(!strcmp(cmd->args[0],"pwd")){
        char directory[CMD_MAX_LENGTH];
        char* ret = getcwd(directory, CMD_MAX_LENGTH);
        if(ret==NULL){
            perror("pwd");
            return EXIT_FAILURE;
        }

        printf("%s\n",directory);

        return EXIT_SUCCESS;
    }

    if(!strcmp(cmd->args[0],"cd")){
        if(cmd->numberOfArgs==1){
            fprintf(stderr, "Error: cannot cd into directory\n");
            return EXIT_FAILURE;
        }

        int status= chdir(cmd->args[1]);
        if(status==-1){
            fprintf(stderr, "Error: cannot cd into directory\n");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    if(!strcmp(cmd->args[0],"exit")){
        for(int i = 0;i < backgroundTasks->length; i++){
              struct pidSet* pids = vector_find(backgroundTasks, i);

              if(pids->tasksAreDone==false){
                  fprintf(stderr, "Error: active job still running\n");
                  return EXIT_FAILURE;
              }
        }

        fprintf(stderr, "Bye...\n");

        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}


void checkTasks(struct pidSet* pidSet){
    int numberOfTasks=pidSet->numberOfPids;
    pidSet->tasksAreDone=true;

    if(pidSet->isBackGroundTasks==false){
        for(int i = 0; i < numberOfTasks; i++){
            int ret;

            waitpid(pidSet->pids[i],&ret,0);

            if(WIFEXITED(ret)){
                pidSet->returnValues[i]=WEXITSTATUS(ret);
            } else {
                pidSet->returnValues[i]=1;
            }

            pidSet->completionFlags[i] = true;
        }
    } else{

        for(int i = 0; i < numberOfTasks; i++){

            if(pidSet->completionFlags[i]==false){
                int ret;

                pid_t pid = waitpid(pidSet->pids[i],&ret,WNOHANG);

                if(pid == 0){
                    pidSet->tasksAreDone=false;
                    continue;
                }

                if(WIFEXITED(ret)){
                    pidSet->returnValues[i]=WEXITSTATUS(ret);
                } else {
                    pidSet->returnValues[i]=1;
                }

                pidSet->completionFlags[i] = true;
            }
        }
    }
}

struct pidSet* runTasks(struct cmd* cmd){
    int numberofTasks = 0;
    struct cmd* currentCmd = cmd;
    bool is_background = false;

    do{
        numberofTasks++;

        if(currentCmd->is_background == 1){
            is_background = true;
        }

        currentCmd = currentCmd->next;

    } while(currentCmd!=NULL);

    struct pidSet* pidSet = pidSetConstructor(numberofTasks, commandLine);

    pidSet->isBackGroundTasks = is_background;

    int** pipes = NULL;
    int numberOfPipes = numberofTasks - 1;

    if(numberOfPipes >= 1){
         pipes=(int**)malloc(numberOfPipes * sizeof(int*));

         for(int i = 0; i < numberOfPipes; i++){
             pipes[i] = (int*)malloc(2 * sizeof(int));
         }
     }

     for(int i = 0; i < numberOfPipes; i++){
         if(pipe(pipes[i]) == -1){
             perror("pipe");
             exit(EXIT_FAILURE);
         }
     }

     currentCmd = cmd;

     for(int i = 0; i < numberofTasks; i++){
         pid_t pid = fork();

         if(pid==-1){
            perror("fork");
            exit(EXIT_FAILURE);
         }

         addPid(pidSet, pid);

         if(pid==0){
             if(i > 0){
                 if(dup2(pipes[i-1][0],STDIN_FILENO)==-1){
                     perror("dup2");
                     exit(EXIT_FAILURE);
                 }
             }

             if(i < numberofTasks-1){
                 if(dup2(pipes[i][1],STDOUT_FILENO)==-1){
                     perror("dup2");
                     exit(EXIT_FAILURE);
                 }
             }

             if(currentCmd->hasOutput){
                 int outFD = open(
                           currentCmd->out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);


                 dup2(outFD,STDOUT_FILENO);
                 close(outFD);
             }

             if(currentCmd->hasInput){
                int inFD = open(
                          currentCmd->in_file, O_RDONLY);

                if(inFD==-1){
                     perror("open");
                     exit(EXIT_FAILURE);
                 }

                dup2(inFD,STDIN_FILENO);
                close(inFD);
            }

             for(int j = 0; j < numberOfPipes; j++){
                 close(pipes[j][0]);
                 close(pipes[j][1]);
             }

             execvp(currentCmd->args[0],currentCmd->args);
             fprintf(stderr, "Error: command not found\n");
             fflush(stderr);
             exit(EXIT_FAILURE);
         }

         currentCmd = currentCmd->next;
     }

     for(int i = 0; i < numberOfPipes; i++){
         close(pipes[i][0]);
         close(pipes[i][1]);
     }

     if(pipes!=NULL){
         for(int i=0; i<numberOfPipes;i++){
             free(pipes[i]);
         }

         free(pipes);
     }


     return pidSet;
}

void printFinishedBackgroundTasks(){
    for(int i = 0;i < backgroundTasks->length; i++){
       struct pidSet* pids = vector_find(backgroundTasks, i);

       if(pids->tasksAreDone){
          printExecutionResult(pids);
       }
   }
}

void removeFinishedBackgroundTasks(){
    for(int i = 0;i < backgroundTasks->length; i++){

       struct pidSet* pids = vector_find(backgroundTasks, i);

       if(pids->tasksAreDone){
          vector_remove(backgroundTasks,i);
          i--;
       }
   }
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
    backgroundTasks = vectorConstructor(pidSetDestructor);

    struct cmd* cmd = cmdConstructor();

    while (1) {
        cmdDestructor(cmd);
        cmd=cmdConstructor();
//        resetCmd(cmd);

        printf("sshell@ucd$ ");
        fflush(stdout);

        bool parsingErrors = (getCmds(cmd)==0);

        bool whiteSpace = strlen(commandLine) == 0 || isWhiteSpace(commandLine);

        if(whiteSpace == false && parsingErrors  == false){

            struct cmd* currentCmd = cmd;

            do{

                if(parseArgs(currentCmd)==0){
                   parsingErrors = true;
                   break;
                }

                currentCmd = currentCmd->next;

            } while(currentCmd != NULL);

        }

        if(parsingErrors == true){
           continue;
        }

        struct pidSet* taskPids = NULL;
        char* firstProgram = NULL;

        if(whiteSpace == false){
            firstProgram = cmd->args[0];
        }

        if(whiteSpace==false && strcmp(firstProgram,"pwd") != 0
                && strcmp(firstProgram,"cd") != 0
                && strcmp(firstProgram,"exit") != 0){

            taskPids = runTasks(cmd);

            if(taskPids->isBackGroundTasks == false){
                checkTasks(taskPids);
            }
        }

        for(int i = 0;i < backgroundTasks->length; i++){
            struct pidSet* pids = vector_find(backgroundTasks, i);
            checkTasks(pids);
       }

        int builtInStatus=-1;

        if(whiteSpace==false && (!strcmp(firstProgram,"pwd")
                 || !strcmp(firstProgram,"cd")
                 || !strcmp(firstProgram,"exit"))){
            builtInStatus=doBuiltinTask(cmd);
        }

        printFinishedBackgroundTasks();
        removeFinishedBackgroundTasks();

        if(whiteSpace==false && (!strcmp(firstProgram,"pwd")
                        || !strcmp(firstProgram,"cd")
                        || !strcmp(firstProgram,"exit"))){

            fprintf(stderr, "+ completed '%s' [%d]\n", commandLine, builtInStatus);
        }

        if(whiteSpace==false && strcmp(firstProgram,"pwd") != 0
                        && strcmp(firstProgram,"cd") != 0
                        && strcmp(firstProgram,"exit") != 0){

            if(taskPids->isBackGroundTasks == false){
               printExecutionResult(taskPids);
               pidSetDestructor(taskPids);
               taskPids=NULL;
           }
        }

        if(whiteSpace==false && !strcmp(firstProgram,"exit")
                && builtInStatus!=EXIT_FAILURE){
            if(taskPids!=NULL){
                 pidSetDestructor(taskPids);
             }
             break;
        }

        if(whiteSpace==false && taskPids!=NULL
                && taskPids->isBackGroundTasks==true){
            vector_add(backgroundTasks, taskPids);
        }
    }

    vectorDestructor(backgroundTasks);
    backgroundTasks=NULL;

    cmdDestructor(cmd);

    return 0;
}





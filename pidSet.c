#include <stdlib.h>

#include "cmd.h"
#include "utils.h"
#include "sshell.h"
#include "pidSet.h"
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>


struct pidSet* pidSetConstructor(int numberOfPids,char* commandLine){
    struct pidSet* pidSet=(struct pidSet*)malloc(sizeof(struct pidSet));

    if(!pidSet){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    pidSet->pids=(pid_t*)malloc(numberOfPids*(sizeof(pid_t)));

    if(!(pidSet->pids)){
       perror("malloc");
       exit(EXIT_FAILURE);
   }

    pidSet->completionFlags=(bool*)malloc(numberOfPids*sizeof(bool));

    if(!(pidSet->completionFlags)){
           perror("malloc");
           exit(EXIT_FAILURE);
     }

    for(int i=0;i<numberOfPids;i++){
        pidSet->completionFlags[i] = false;
    }

    pidSet->numberOfPids=numberOfPids;
    pidSet->pidCounter=0;

    pidSet->isBackGroundTasks=false;

    pidSet->returnValues=(int*)malloc(numberOfPids*sizeof(int));

    if(!(pidSet->returnValues)){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    pidSet->tasksAreDone=false;

    pidSet->terminalInput = strdup(commandLine);

    if(!(pidSet->terminalInput)){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    return pidSet;
}

void printPidSet(struct pidSet* ps){
    printf("\n###########################\n");
    printf("pids:");
    for(int i=0;i<ps->numberOfPids;i++){
        printf(" %d",ps->pids[i]);
    }
    printf("\n");
    printf("#pids: %d\n",ps->numberOfPids);
    printf("tinput: %s\n",ps->terminalInput);

    printf("array sizes: %lu %lu %lu %lu\n",
            sizeof(ps->pids),sizeof(ps->returnValues),
            sizeof(ps->completionFlags),sizeof(ps->terminalInput));
    printf("\n###########################\n");
}

void pidSetDestructor(void* pidSet){
    struct pidSet* ps=(struct pidSet*)pidSet;

    free(ps->pids);
    ps->pids=NULL;

    free(ps->terminalInput);
    ps->terminalInput=NULL;

    free(ps->returnValues);
    ps->returnValues=NULL;

    free(ps->completionFlags);
    ps->completionFlags=NULL;

    free(ps);
}

void addPid(struct pidSet* pidSet, pid_t pid){
    pidSet->pids[pidSet->pidCounter++]=pid;
}

void printExecutionResult(struct pidSet* pidSet){
    fprintf(stderr, "+ completed '%s'", pidSet->terminalInput);

    for(int i = 0; i < pidSet->numberOfPids; i++){
       fprintf(stderr," [%d]",pidSet->returnValues[i]);
    }

    fprintf(stderr,"\n");
}

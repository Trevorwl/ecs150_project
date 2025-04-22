#ifndef PID_SET
#define PID_SET

#include <stdbool.h>

#include "sshell.h"
#include "cmd.h"

struct pidSet{
    pid_t* pids;
    int numberOfPids;
    int pidCounter;

    int* returnValues;
    bool tasksAreDone;

    bool*completionFlags;

    bool isBackGroundTasks;

    char* terminalInput;
};

struct pidSet* pidSetConstructor(int numberOfPids, char* commandLine);

void pidSetDestructor(void* pidSet);

void addPid(struct pidSet* pidSet, pid_t pid);

void printExecutionResult(struct pidSet* pidSet);

void printPidSet(struct pidSet* ps);



#endif

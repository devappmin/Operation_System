#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>

#define PROCESS_COUNT 21

void hardJob();
int separateNice(int processNum);

int main() {
    pid_t pid;
    int ret;

    printf("PARENT: %d\n", getpid());
    for (int i = 0; i < PROCESS_COUNT; i++) {
        if((pid = fork()) == 0) {
            ret = setpriority(PRIO_PROCESS, pid, separateNice(i));
            printf("CREATE PROCESS: %d\tNICE: %d\n", getpid(), getpriority(PRIO_PROCESS, pid));
            hardJob();
            exit(0);
        }
    }

    if (pid != 0 && pid != -1) {
        for (int i = 0; i < PROCESS_COUNT; i++) {
            int status, retval;
            retval = waitpid(-1, &status, 0);
            printf("FINISHED PROCESS: %d\n", retval);
        }
        printf("ALL PROCESSES HAVE BEEN FINISHED\n");
    }

    return 0;
}

void hardJob() {
    for(int i = 0; i <= 999999999; i++) {}
}

int separateNice(int processNum) {
    return processNum < 7 ? 19 : processNum < 14 ? 10 : 5;
}
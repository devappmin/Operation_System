#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PROCESS_COUNT 21

void hardJob();

int main() {
    pid_t pid;
    printf("PARENT: %d\n", getpid());
    for (int i = 0; i < PROCESS_COUNT; i++) {
        if((pid = fork()) == 0) {
            printf("[*] CREATE PROCESS: %d\n", getpid());
            hardJob();
            exit(0);
        }
    }

    if (pid != 0 && pid != -1) {
        for (int i = 0; i < PROCESS_COUNT; i++) {
            int status, retval;
            retval = waitpid(-1, &status, 0);
            printf("[*] FINISHED PROCESS: %d\n", retval);
        }
        printf("[!] ALL PROCESSES HAVE BEEN FINISHED\n");
    }

    return 0;
}

void hardJob() {
    for(int i = 0; i <= 999999999; i++) {}
}
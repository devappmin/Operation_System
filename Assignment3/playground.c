#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>

#define ADD 442
#define SUB 443
#define MUL 444
#define MOD 445
#define OTH -1

int findOperator(char* buf, int len) {
    const char operators[4] = {'+', '-', '*', '%'};

    for(int i = 1; i < len; i++) {
        for(int j = 0; j < 4; j++) {
            if(buf[i] == operators[j])
                return ADD + j;
        }
    }

    return OTH;
}

int main() {
    char buf[100];
    int op;
    long int result;
    while(1) {
        printf(">> ");
        scanf("%s", buf);

        if((op = findOperator(buf, strlen(buf))) == -1) {
            printf("Undefined Opeator\n");
            continue;
        }

        syscall(op, buf, &result, strlen(buf));
        printf("%ld\n", result);
    }



    return 0;
}
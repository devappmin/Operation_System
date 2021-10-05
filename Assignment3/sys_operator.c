#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

int atoi(char *input){
    int len;
    int cur;
    int temp;
    int result = 0;
    int isNegative = 1; // 1 for Positive, -1 for Negative

    if (input[0] == '-') {
        isNegative = -1;
        input++;
    }

    for (len = 0; input[len] != '\0'; len++);
    len--;
    cur = len;

    for (temp = 1; cur >= 0; cur--, temp *= 10)
        result += (input[cur] - '0') * temp;

    return result * isNegative;
}

void separates(char *input, char op, int *left, int *right) {
    int pos = 0, opPos = 0;
    char leftStr[20] = {0};
    char rightStr[20] = {0};

    while (input[pos] != op || pos == 0) {
        leftStr[pos] = input[pos];
        printk("%c\n", leftStr[pos]);
        pos++;
    }
    opPos = pos++;

    while (input[pos] != '\0'){
        rightStr[pos - opPos - 1] = input[pos];
        pos++;
    }
    printk("[%s] [%s]\n", leftStr, rightStr);
    *left = atoi(leftStr);
    *right = atoi(rightStr);
    printk("[%d] [%d]\n", *left, *right);
}

asmlinkage long sys_my_add(char* input) {
    int left, right;
    separates(input, '+', &left, &right);
    printk("%s\n%d %d\n", input, left, right);
    return left + right;
}

asmlinkage long sys_my_sub(char* input) {
    int left, right;
    separates(input, '-', &left, &right);
    printk("%s\n%d %d\n", input, left, right);
    return left - right;
}

asmlinkage long sys_my_mul(char* input) {
    int left, right;
    separates(input, '*', &left, &right);
    printk("%s\n%d %d\n", input, left, right);
    return left * right;
}

asmlinkage long sys_my_mod(char* input) {
    int left, right;
    separates(input, '%', &left, &right);
    printk("%s\n%d %d\n", input, left, right);
    return (int)left % (int)right;
}

SYSCALL_DEFINE3(my_add, char *, input, long int *, output, int, size) {
    char* buf;
    int err;
    buf = kmalloc(size, GFP_KERNEL);
    if((err = copy_from_user(buf, input, size)) > 0) return err;
    long int result = sys_my_add(buf);
    kfree(buf);

    copy_to_user(output, &result, 8);
    return result;
}

SYSCALL_DEFINE3(my_sub, char *, input, long int *, output, int, size) {
    char* buf;
    int err;
    buf = kmalloc(size, GFP_KERNEL);
    if((err = copy_from_user(buf, input, size)) > 0) return err;
    long int result = sys_my_sub(buf);
    kfree(buf);

    copy_to_user(output, &result, 8);
    return result;
}

SYSCALL_DEFINE3(my_mul, char *, input, long int *, output, int, size) {
    char* buf;
    int err;
    buf = kmalloc(size, GFP_KERNEL);
    if((err = copy_from_user(buf, input, size)) > 0) return err;
    long int result = sys_my_mul(buf);
    kfree(buf);
    copy_to_user(output, &result, 8);
    return result;
}

SYSCALL_DEFINE3(my_mod, char *, input, long int *, output, int, size) {
    char* buf;
    int err;
    buf = kmalloc(size, GFP_KERNEL);
    if((err = copy_from_user(buf, input, size)) > 0) return err;
    long int result = sys_my_mod(buf);
    kfree(buf);
    copy_to_user(output, &result, 8);
    return result;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// static inline void native_cpuid(unsigned int *eax, unsigned int *ebx, unsigned int *ecx, unsigned int *edx)
// {
//     asm volatile("cpuid"
//                  : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
//                  : "0"(*eax), "2"(*ecx));
// }

// int main(int argc, char **argv)
// {
//     unsigned eax, ebx, ecx, edx;
//     eax = 1;
//     native_cpuid(&eax, &ebx, &ecx, &edx);

//     return 0;
// }

int cache_size_kb(void)
{
    char line[512], buffer[32];
    int column;
    FILE *cpuinfo;

    if (!(cpuinfo = fopen("/proc/cpuinfo", "r")))
    {
        perror("/proc/cpuinfo: fopen");
        return -1;
    }

    while (fgets(line, sizeof(line), cpuinfo))
    {
        if (strstr(line, "cache size"))
        {
            column = strstr(line, ":");
            strncpy(buffer, line + column + 1, sizeof(buffer));
            fclose(cpuinfo);
            return (int)strtol(buffer, NULL, 10);
        }
    }
    fclose(cpuinfo);
    return -1;
}

int main(int argc, char **argv)
{
    // FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
    // char *arg = 0;
    // size_t size = 0;
    // while (getdelim(&arg, &size, 0, cpuinfo) != -1)
    // {
    //     puts(arg);
    // }
    // free(arg);
    // fclose(cpuinfo);
    printf("%d", cache_size_kb());
    return 0;
}
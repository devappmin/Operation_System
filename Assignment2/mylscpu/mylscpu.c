#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Dict
{
    char *key;
    char *value;
} Dict;

int cache_size_kb(void)
{
    char line[512], buffer[32];
    FILE *cpuinfo;

    if (!(cpuinfo = fopen("/proc/cpuinfo", "r")))
    {
        perror("/proc/cpuinfo: fopen");
        return -1;
    }

    while (fgets(line, sizeof(line), cpuinfo))
    {
        printf(">> %s\n", line);
        if (strstr(line, "cache size"))
        {
            char *column = strstr(line, ":");
            strncpy(buffer, line + strlen(column) + 1, sizeof(buffer));
            fclose(cpuinfo);
            return (int)strtol(buffer, NULL, 10);
        }
    }
    fclose(cpuinfo);
    return -1;
}

void removeBlanks(char *str, int size)
{
    for (int i = size; i > 0; i--)
    {
        if ((str[size - 1] == ' ') || (str[size - 1] == '\n'))
        {
            printf("%d", i);
            continue;
        }
        else
        {
            str[size] == '\0';
            return;
        }
    }
}

void getCPUInfo(FILE *fp)
{
    const char from[][11] = {"vendor_id", "model name", "cpu MHz"};
    const char to[][15] = {"Vendor ID\t\t", "Model name\t\t", "CPU MHz\t\t\t"};
    char *line = NULL;
    size_t size = 0;
    ssize_t ret;

    int cpuCores;

    while ((ret = getline(&line, &size, fp)) != -1)
    {
        // printf("(%d)>> %s", strlen(line), line);
        char *value = strtok(line, ":");
        char *key = strtok(NULL, ",");

        value = strtok(value, "\t");
        key = strtok(key, "\n");

        if (key == NULL)
            break;

        if (key[0] == ' ')
            key++;
        // printf("[%ld]%s.[%ld]%s.\n", strlen(value), value, strlen(key), key);

        if (strcmp("cpu cores", value) == 0)
            cpuCores = atoi(key);

        for (int i = 0; i < 3; i++)
        {
            if (strcmp(from[i], value) == 0)
            {
                printf("%s%s\n", to[i], key);
                break;
            }
        }
    }
}

void getCatchInfo(int cores)
{
    //DEBUG MODE
    cores = 6;

    FILE *cacheInfo[4];

    const char cacheName[][4] = {"L1d", "L1i", "L2", "L3"};

    for (int i = 0; i < 4; i++)
    {
        char temp[50];
        sprintf(temp, "/sys/devices/system/cpu/cpu0/cache/index%d/size", i);
        cacheInfo[i] = fopen(temp, "r");

        int cacheSizePerCore;
        fscanf(cacheInfo[i], "%d", &cacheSizePerCore);

        printf("%s cache:\t\t%d KiB\n", cacheName[i], cacheSizePerCore * cores);
        fclose(cacheInfo[i]);
    }
}

int main(int argc, char **argv)
{
    FILE *cpuInfo = fopen("/proc/cpuinfo", "r");
    // char *arg = 0;
    // size_t size = 0;
    // while (getdelim(&arg, &size, 0, cpuinfo) != -1)
    // {
    //     puts(arg);
    // }
    // free(arg);
    // fclose(cpuinfo);
    getCPUInfo(cpuInfo);
    fclose(cpuInfo);

    getCatchInfo(6);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void printCPUInfo();
void getCacheInfo(int cores);
void printCore(int cores);

int main(int argc, char **argv)
{
    printCPUInfo();

    return 0;
}

void printCPUInfo()
{
    // /proc/cpuinfo에서 CPU 정보를 불러온다.
    FILE *cpuInfo = fopen("/proc/cpuinfo", "r");

    // cpuinfo 내에 존재하는 key 값
    const char from[][15] = {"vendor_id", "model name", "cpu MHz", "stepping",
                             "flags", "model", "cpu family", "bogomips"};

    // from 내에 있는 값을 lscpu와 동일한 값으로 변경하기 위한 값
    const char to[][50] = {"Vendor ID:\t\t", "Model name:\t\t", "CPU MHz:\t\t",
                           "Stepping:\t\t", "Flags:\t\t\t", "Model:\t\t\t",
                           "CPU family:\t\t", "BogoMIPS:\t\t"};

    // cpuInfo에서 한 줄 씩 불러오기 위해서 선언한 변수
    char *line = NULL;
    size_t size = 0;
    ssize_t ret;

    // CPU 코어 개수를 저장하기 위한 변수
    int cores;

    // cpuInfo의 모든 줄을 읽을 때 까지 반복
    while ((ret = getline(&line, &size, cpuInfo)) != -1)
    {
        // 읽어온 줄의 ':' 문자를 기준으로 key값과 value값을 나눈다.
        char *key = strtok(line, ":");
        char *value = strtok(NULL, ",");

        // key값과 value값에 불필요한 \t와 \n을 삭제해준다.
        key = strtok(key, "\t");
        value = strtok(value, "\n");

        // 첫번째 프로세스를 다 읽고 빈 칸이 나왔으면 반복문을 탈출한다.
        if (value == NULL)
            break;

        // value 문자열 앞에 띄어쓰기가 있는 것을 삭제해준다.
        if (value[0] == ' ')
            value++;

        // key값이 cpu core값이면 value의 값을 cores에 저장한다.
        if (strcmp("cpu cores", key) == 0)
            cores = atoi(value);

        // 반복을 하면서 from에 있는 문자들을 찾은 후 해당 위치의
        // to 값과 value를 출력한다.
        for (int i = 0; i < sizeof(from); i++)
        {
            if (strcmp(from[i], key) == 0)
            {
                printf("%s%s\n", to[i], value);
                break;
            }
        }
    }

    // 코어와 관련된 내용을 출력한다.
    printCore(cores);

    // 캐시와 관련된 내용을 출력한다.
    getCacheInfo(cores);

    // /proc/cpuinfo 파일을 닫는다.
    fclose(cpuInfo);
}

void printCore(int cores)
{
    // sysconf를 통해서 프로세서의 개수를 출력한다.
    printf("CPU(s):\t\t\t%ld\n", sysconf(_SC_NPROCESSORS_ONLN));

    // 프로세서의 개수와 코어의 개수를 나누면 1 코어당 가지고 있는 쓰레드가 나오므로
    // 해당 값을 출력한다.
    printf("Thread(s) per core:\t%ld\n", sysconf(_SC_NPROCESSORS_ONLN) / cores);

    // CPU 실제 코어의 값을 출력한다.
    printf("Core(s) per socket:\t%d\n", cores);
}

void getCacheInfo(int cores)
{
    // L1i L1d L2 L3의 값을 가진 파일을 열기위한 파일 배열
    FILE *cacheInfo[4];

    // 캐시 명을 저장하는 문자열 배열
    const char cacheName[][4] = {"L1d", "L1i", "L2", "L3"};

    // KiB, MiB, GiB를 나누기 위한 문자열 배열
    const char sizeName[3] = {'K', 'M', 'G'};

    // 캐시의 수(4) 만큼 반복한다.
    for (int i = 0; i < 4; i++)
    {
        // 캐시의 값이 저장되어있는 폴더에 접근하여 size 파일을 연다.
        char temp[50];
        sprintf(temp, "/sys/devices/system/cpu/cpu0/cache/index%d/size", i);
        cacheInfo[i] = fopen(temp, "r");

        // 파일 내에 저장되어있는 코어 당 캐시 값을 변수에 저장한다.
        int cacheSizePerCore;
        fscanf(cacheInfo[i], "%d", &cacheSizePerCore);

        // l3 캐시의 경우 공유캐시이므로 특별하게 관리를 해줘야 한다.
        if (i == 3)
        {
            // 해당 캐시를 공유하는 논리 코어의 숫자를 저장하는 파일에 접근한다.
            FILE *shareMemoryInfo = fopen("/sys/devices/system/cpu/cpu0/cache/index3/shared_cpu_list", "r");

            // 해당 파일에서 불러올 첫번째 프로세스 값과 마지막 프로세스 값을 저장하기 위한 변수
            int front = 0, end = 0, sharedMemory;

            // X-X 형식으로 되어있는 값에서 front와 end의 값을 뽑아온다.
            fscanf(shareMemoryInfo, "%d-%d", &front, &end);

            // 캐시를 공유하는 논리 코어의 수
            sharedMemory = end - front + 1;

            // 해당 값을 코어당 캐시 사이즈에 나눈다.
            cacheSizePerCore /= sharedMemory;

            // /sys/devices/system/cpu/cpu0/cache/index3/shared_cpu_list 파일을 닫는다.
            fclose(shareMemoryInfo);
        }

        // KiB MiB GiB를 나누기 위한 변수
        int multiplier = 0;

        // 캐시 사이즈를 cacheSizePerCore와 cores의 개수 (l3의 경우, 총 쓰레드의 개수)를 곱해서 구한다.
        int cacheSize = cacheSizePerCore * (i != 3 ? cores : sysconf(_SC_NPROCESSORS_ONLN));

        // KiB MiB GiB를 구한다.
        while (cacheSize >= 1000)
        {
            cacheSize /= 1000;
            multiplier++;
        }

        // 얻은 값을 통해서 출력한다.
        printf("%s cache:\t\t%d %ciB\n", cacheName[i], cacheSize, sizeName[multiplier]);

        // /sys/devices/system/cpu/cpu0/cache/index[X]/size 파일을 닫는다.
        fclose(cacheInfo[i]);
    }
}

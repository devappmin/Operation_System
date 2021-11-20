#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FRAME_SIZE 4
#define MAX_PRS_SIZE 30
#define NULL_FRAME -1

static const char* METHOD[] = {"OPT", "FIFO", "LRU", "Second-Chance"};

int readFile(FILE* fp, int* frameLength, int* prs);
void readLine(char* buffer, int size, FILE* fp);
void printHeader(int frameLength, const char* method, int prsLength, int* prs);
void printRow(int time, int frameLength, int* frame, bool pageFault);
void printFooter(int totalPageFaults);
bool fifoRow(int time, int* pos, int* frame, int frameLength, int prsInput);
void fifo(int frameLength, int prsLength, int* prs);
bool optimalRow(int pos, int* frame, int frameLength, int prsLength, int* prs);
void optimal(int frameLength, int prsLength, int* prs);
bool lruRow(int pos, int* frame, int frameLength, int* prs);
void lru(int frameLength, int prsLength, int* prs);
bool secondChanceRow(int time, int* pos, int* ref, int* frame, int frameLength, int prsInput);
void secondChance(int frameLength, int prsLength, int* prs);

int main() {
    char fileName[100];
    FILE* fp;

    int frameLength;
    int prs[MAX_PRS_SIZE];

    printf("SELECT INPUT FILE: ");
    scanf("%s", fileName);

    if ((fp = fopen(fileName, "r")) == NULL) {
        printf("ERROR: Could not open input file\n");
        exit(1);
    }

    int prsLength = readFile(fp, &frameLength, prs);

    fifo(frameLength, prsLength, prs);
    optimal(frameLength, prsLength, prs);
    lru(frameLength, prsLength, prs);
    secondChance(frameLength, prsLength, prs);

    fclose(fp);
    return 0;
}

int readFile(FILE* fp, int* frameLength, int* prs) {
    char buffer[300];
    char* ptr;
    int prsCount = 0;

    // frameLength 값을 구하는 부분
    readLine(buffer, sizeof(buffer), fp);
    *frameLength = buffer[0] - '0';

    readLine(buffer, sizeof(buffer), fp);
    ptr = strtok(buffer, " ");
    prs[prsCount++] = atoi(ptr);
    while ((ptr = strtok(NULL, " ")) != NULL) {
        prs[prsCount++] = atoi(ptr);
    }

    return prsCount;
}

void readLine(char* buffer, int size, FILE* fp) {
    if (fgets(buffer, size, fp) == NULL) {
        printf("ERROR: Failed to read file\n");
        exit(1);
    }
}

void printHeader(int frameLength, const char* method, int prsLength, int* prs) {
    // Line: 1
    printf("\n\nUsed method : %s\n", method);

    // Line: 2
    printf("page reference string : ");
    for (int i = 0; i < prsLength; i++) printf("%d ", prs[i]);

    // Line: 3
    printf("\n\n");

    // Line: 4
    printf("\tframe\t");
    for (int i = 1; i <= frameLength; i++) printf("%d\t", i);
    printf("page fault\n");

    // Line: 5
    printf("time\n");
}

void printRow(int time, int frameLength, int* frame, bool pageFault) {
    printf("%d\t\t", time);

    for (int i = 0; i < frameLength; i++) {
        if (frame[i] != NULL_FRAME) printf("%d", frame[i]);
        printf("\t");
    }

    printf("%c\n", pageFault ? 'F' : '\0');
}

void printFooter(int totalPageFaults) {
    printf("Number of page faults : %d times\n", totalPageFaults);
}

bool fifoRow(int time, int* pos, int* frame, int frameLength, int prsInput) {
    bool pageFault = true;

    for (int i = 0; i < frameLength; i++) {
        if (frame[i] == prsInput) {
            pageFault = false;
            break;
        }
    }

    if (pageFault) {
        if (*pos == frameLength) *pos = 0;
        frame[(*pos)++] = prsInput;
    }

    printRow(time, frameLength, frame, pageFault);

    return pageFault;
}

void fifo(int frameLength, int prsLength, int* prs) {
    int* frame = (int*)malloc(sizeof(int) * frameLength);
    memset(frame, NULL_FRAME, sizeof(int) * frameLength);

    int pos = 0;
    int totalPageFaults = 0;

    printHeader(frameLength, METHOD[1], prsLength, prs);

    for (int i = 0; i < prsLength; i++) {
        if (fifoRow(i + 1, &pos, frame, frameLength, prs[i]))
            totalPageFaults++;
    }

    printFooter(totalPageFaults);
}

bool optimalRow(int pos, int* frame, int frameLength, int prsLength, int* prs) {
    bool pageFault = true;

    for (int i = 0; i < frameLength; i++) {
        if (frame[i] == prs[pos]) {
            pageFault = false;
            break;
        }
    }

    if (pageFault) {
        int shortest = 0;
        int delta = 0;
        bool isExist = true;

        for (int frameNumber = 0; frameNumber < frameLength; frameNumber++) {
            if (frame[frameNumber] == NULL_FRAME) {
                shortest = frameNumber;
                break;
            }

            for (int searchPos = pos, moved = 0; searchPos < prsLength; searchPos++, moved++) {
                if (prs[searchPos] == frame[frameNumber]) {
                    if (moved > delta) {
                        delta = moved;
                        shortest = frameNumber;
                    }
                    break;
                } else {
                    if (searchPos == prsLength - 1) {
                        isExist = false;
                        delta = moved;
                        shortest = frameNumber;
                    }
                }
            }

            if (!isExist) break;
        }
        frame[shortest] = prs[pos];
    }

    printRow(pos + 1, frameLength, frame, pageFault);

    return pageFault;
}

void optimal(int frameLength, int prsLength, int* prs) {
    int* frame = (int*)malloc(sizeof(int) * frameLength);
    memset(frame, NULL_FRAME, sizeof(int) * frameLength);

    int totalPageFaults = 0;

    printHeader(frameLength, METHOD[0], prsLength, prs);

    for (int i = 0; i < prsLength; i++) {
        if (optimalRow(i, frame, frameLength, prsLength, prs))
            totalPageFaults++;
    }

    printFooter(totalPageFaults);
}

bool lruRow(int pos, int* frame, int frameLength, int* prs) {
    bool pageFault = true;

    for (int i = 0; i < frameLength; i++) {
        if (frame[i] == prs[pos]) {
            pageFault = false;
            break;
        }
    }

    if (pageFault) {
        int least = 0;
        int delta = 0;

        for (int frameNumber = 0; frameNumber < frameLength; frameNumber++) {
            if (frame[frameNumber] == NULL_FRAME) {
                least = frameNumber;
                break;
            }

            for (int searchPos = pos, moved = 0; searchPos > 0; searchPos--, moved++) {
                if (prs[searchPos] == frame[frameNumber]) {
                    if (moved > delta) {
                        delta = moved;
                        least = frameNumber;
                    }
                    break;
                }
            }
        }
        frame[least] = prs[pos];
    }

    printRow(pos + 1, frameLength, frame, pageFault);

    return pageFault;
}

void lru(int frameLength, int prsLength, int* prs) {
    int* frame = (int*)malloc(sizeof(int) * frameLength);
    memset(frame, NULL_FRAME, sizeof(int) * frameLength);

    int totalPageFaults = 0;

    printHeader(frameLength, METHOD[2], prsLength, prs);

    for (int i = 0; i < prsLength; i++) {
        if (lruRow(i, frame, frameLength, prs)) totalPageFaults++;
    }

    printFooter(totalPageFaults);
}

bool secondChanceRow(int time, int* pos, int* ref, int* frame, int frameLength, int prsInput) {
    bool pageFault = true;

    for (int i = 0; i < frameLength; i++) {
        if (frame[i] == prsInput) {
            pageFault = false;
            ref[prsInput]++;
            break;
        }
    }

    if (pageFault) {
        if (*pos == frameLength) *pos = 0;

        if (frame[*pos] != NULL_FRAME) {
            while (ref[frame[*pos]] > 0) {
                ref[frame[*pos]]--;
                (*pos)++;
                if (*pos == frameLength) *pos = 0;
            }
        }

        frame[(*pos)++] = prsInput;
    }

    printRow(time, frameLength, frame, pageFault);

    return pageFault;
}

void secondChance(int frameLength, int prsLength, int* prs) {
    int* frame = (int*)malloc(sizeof(int) * frameLength);
    memset(frame, NULL_FRAME, sizeof(int) * frameLength);
    int* ref = (int*)calloc(MAX_PRS_SIZE, sizeof(int));

    int pos = 0;
    int totalPageFaults = 0;

    printHeader(frameLength, METHOD[3], prsLength, prs);

    for (int i = 0; i < prsLength; i++) {
        if (secondChanceRow(i + 1, &pos, ref, frame, frameLength, prs[i]))
            totalPageFaults++;
    }

    printFooter(totalPageFaults);
}
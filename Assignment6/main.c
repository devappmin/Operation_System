#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NULL_VEHICLE -1
#define THREAD_LENGTH 4

typedef struct WaitNode {
    struct WaitNode* node;
    int data;
} WaitNode;

typedef struct ThreadData {
    int* vehicleList;
    int vehicleLength;
    int totalLoaded;
    int time;
    WaitNode* waiting;
    int waitLength;
    int onProgress;
    int passed;
    int totalPassed[THREAD_LENGTH];
} ThreadData;

int possible[5] = {-1, 3, 4, 1, 2};
int next = -1;

ThreadData threadData;
pthread_mutex_t mutexLock;

bool isFinished() {
    int sum = 0;
    for (int i = 0; i < 4; i++) {
        sum += threadData.totalPassed[i];
    }

    return (sum == threadData.vehicleLength) &&
                   (threadData.waitLength == 0) &&
                   (threadData.onProgress == NULL_VEHICLE)
               ? true
               : false;
}

void addNode(WaitNode* head, int data) {
    WaitNode* node = (WaitNode*)malloc(sizeof(WaitNode));
    node->data = data;
    node->node = NULL;

    if (head->node == NULL) {
        head->node = node;
    } else {
        WaitNode* cur = head;
        while (cur->node != NULL) {
            cur = cur->node;
        }
        cur->node = node;
    }
}

int popNode(WaitNode* head, int pos) {
    int result;
    WaitNode* cur = head;

    for (int i = 0; i < pos; i++) {
        cur = cur->node;
    }
    result = cur->node->data;
    cur->node = cur->node->node;

    return result;
}

int getPosNode(WaitNode* head, int value) {
    int result = -1;
    WaitNode* cur = head;

    while (cur->data != value) {
        if (cur->node == NULL) {
            result = NULL_VEHICLE;
            break;
        }
        result++;
        cur = cur->node;
    }

    return result;
}

void printNode(WaitNode* head) {
    WaitNode* cur = head->node;
    while (cur != NULL) {
        printf(" %d", cur->data);
        cur = cur->node;
    }
}

void printHeader(int vehicleLength, int* vehicleList) {
    printf("Total number of vehicles : %d\n", vehicleLength);
    printf("Start point : ");
    for (int i = 0; i < vehicleLength; i++)
        printf("%d ", vehicleList[i]);
    printf("\n");
}

void printPerTimes() {
    printf("tick : %d\n", threadData.time);
    printf("=======================\n");
    printf("Passed Vehicle\nCar ");
    if (threadData.passed != NULL_VEHICLE)
        printf("%d", threadData.passed);

    printf("\nProgressing Vehicle\nCar %c", threadData.onProgress == -1 ? ' ' : (threadData.onProgress + '0'));

    printf("\nWaiting Vehicle\nCar");
    if (threadData.waitLength != NULL_VEHICLE)
        printNode(threadData.waiting);

    printf("\n=======================\n");
}

void printFooter() {
    printf("Number of vehicles passed from each start point\n");
    for (int i = 0; i < THREAD_LENGTH; i++) {
        printf("P%d : %d times\n", i + 1, threadData.totalPassed[i]);
    }
    printf("Total time : %d ticks\n", threadData.time);
}

// void printPerTimes(int tick, int passed, int* waiting, int waitLength) {
//     printf("tick : %d\n", tick);
//     printf("=======================\n");
//     printf("Passed Vehicle\nCar ");
//     if (passed != NULL_VEHICLE)
//         printf("%d", passed);
//     printf("\nWaiting Vehicle\nCar ");
//     if (waitLength != NULL_VEHICLE)
//         for (int i = 0; i < waitLength; i++)
//             printf("%d ", waiting[i]);
//     printf("\n=======================\n");
// }

bool isAllLoaded() {
    return threadData.vehicleLength == threadData.totalLoaded;
}

void* threadJob(void* arg) {
    int threadNum = (int)arg + 1;

    while (!isFinished()) {
        pthread_mutex_lock(&mutexLock);
        if (threadNum == threadData.vehicleList[threadData.totalLoaded]) {
            // 실질적으로 움직이는 부분
            if (possible[threadNum] == threadData.onProgress && next == -1) {
                threadData.passed = threadData.onProgress;
                threadData.onProgress = threadNum;
                threadData.totalLoaded++;
            } else {
                addNode(threadData.waiting, threadNum);
                threadData.waitLength++;
                if (threadData.onProgress == NULL_VEHICLE) {
                    threadData.passed = threadData.onProgress;
                    threadData.onProgress = popNode(threadData.waiting, rand() % threadData.waitLength);
                    threadData.waitLength--;
                } else {
                    threadData.passed = threadData.onProgress;
                    int pos = getPosNode(threadData.waiting, possible[threadData.onProgress]);

                    threadData.onProgress = (pos == NULL_VEHICLE ? NULL_VEHICLE : popNode(threadData.waiting, pos));

                    if (threadData.onProgress != NULL_VEHICLE)
                        threadData.waitLength--;
                }
                threadData.totalLoaded++;
            }

            if (threadData.passed != NULL_VEHICLE)
                threadData.totalPassed[threadData.passed - 1]++;

            threadData.time++;
            printPerTimes();

            if (isAllLoaded()) {
                next = popNode(threadData.waiting, rand() % threadData.waitLength);
                threadData.waitLength--;
            }

        } else if (threadNum == next) {
            if (threadData.waitLength > 0) {
                threadData.passed = threadData.onProgress;
                threadData.onProgress = next;
                next = popNode(threadData.waiting, rand() % threadData.waitLength);
                threadData.waitLength--;
                threadData.time++;
                printPerTimes();
            } else {
                printf("FINISHED");
                break;
            }
            // if (possible[next] == threadData.onProgress) {
            //     threadData.passed = threadData.onProgress;
            //     threadData.onProgress = next;
            // } else {
            //     if (threadData.onProgress == NULL_VEHICLE) {
            //         threadData.passed = threadData.onProgress;
            //         threadData.onProgress = next;
            //         threadData.waitLength--;
            //     } else {
            //         threadData.onProgress = threadData.onProgress;
            //         int pos = getPosNode(threadData.waiting, possible[threadData.onProgress]);

            //         if (pos == NULL_VEHICLE)
            //             threadData.onProgress = NULL_VEHICLE;
            //         else {
            //             threadData.onProgress = popNode(threadData.waiting, pos);
            //             addNode(threadData.waiting, next);
            //         }
            //     }
            // }

            // if (threadData.passed != NULL_VEHICLE)
            //     threadData.totalPassed[threadData.passed - 1]++;

            // printf("IMHERE");
            // if (possible[next] == threadData.onProgress) {
            //     threadData.passed = threadData.onProgress;
            //     threadData.onProgress = threadNum;
            // } else {
            //     if (threadData.onProgress == NULL_VEHICLE) {
            //         threadData.passed = threadData.onProgress;
            //         threadData.onProgress = next;
            //         threadData.waitLength--;
            //     } else {
            //         threadData.passed = threadData.onProgress;
            //         int pos = getPosNode(threadData.waiting, possible[threadData.onProgress]);

            //         threadData.onProgress = (pos == NULL_VEHICLE ? NULL_VEHICLE : next);

            //         if (threadData.onProgress != NULL_VEHICLE)
            //             threadData.waitLength--;
            //     }
            // }

            // if (threadData.passed != NULL_VEHICLE)
            //     threadData.totalPassed[threadData.passed - 1]++;
        }

        pthread_mutex_unlock(&mutexLock);
    }
}

void initThreadData(int vehicleLength, int* vehicleList) {
    threadData.vehicleList = vehicleList;
    threadData.vehicleLength = vehicleLength;
    threadData.time = 0;
    threadData.totalLoaded = 0;
    threadData.waitLength = 0;
    threadData.waiting = (WaitNode*)malloc(sizeof(WaitNode));
    threadData.passed = NULL_VEHICLE;
    threadData.onProgress = NULL_VEHICLE;
    for (int i = 0; i < THREAD_LENGTH; i++) {
        threadData.totalPassed[i] = 0;
    }
}

void run(pthread_t* tid, int vehicleLength, int* vehicleList) {
    printHeader(vehicleLength, vehicleList);

    initThreadData(vehicleLength, vehicleList);

    int rc;
    for (int i = 0; i < THREAD_LENGTH; i++) {
        char threadName[10];
        sprintf(threadName, "THREAD %d", i);
        pthread_create(&tid[i], NULL, threadJob, (void*)i);
    }
    for (int i = 0; i < THREAD_LENGTH; i++) {
        pthread_join(tid[i], (void*)&rc);
    }

    printFooter();
}

int main() {
    srand(time(NULL));
    pthread_mutex_init(&mutexLock, NULL);

    // Get vehicle Length
    // int vehicleLength;
    // scanf("%d", &vehicleLength);
    int vehicleLength = 10;

    // Get vehicle List
    // int* vehicleList = (int*)malloc(sizeof(int) * vehicleLength);
    // for (int i = 0; i < vehicleLength; i++)
    //     vehicleList[i] = rand() % 4 + 1;

    int vehicleList[10] = {4, 4, 3, 4, 1, 2, 2, 3, 3, 3};

    // Create thread List
    pthread_t tid[THREAD_LENGTH];

    run(tid, vehicleLength, vehicleList);

    return 0;
}

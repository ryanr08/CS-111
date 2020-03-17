//NAME: Ryan Riahi
//EMAIL: ryanr08@gmail.com
//ID: 105138860

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include "SortedList.h"

char yieldOpts[3] = "\0";
int opt_yield = 0;
long lock_time = 0;
int numLists;
pthread_mutex_t* lock;
volatile int* spinlock;
int iterations;
int threads;
int numElements;
char listOption[2] = "N\0";
SortedListElement_t *elements;
SortedList_t *list;

void error(char *msg)
{
    perror(msg);
    exit(1);
}

unsigned int getHash(const char *key)
{
    unsigned int index = 97 * (int)(*key);
    index = index % numLists;
    return index;
}

int argChecker(char *optarg)
{
    for (size_t i = 0; i < strlen(optarg); i++)
    {
        if (!isdigit(optarg[i]))
            return 0;
    }
    return 1;
}

void setStartTime(struct timespec *timer)
{
    if (clock_gettime(CLOCK_REALTIME, timer) < 0)
        error("Error getting start time");
}

long Runtime(struct timespec *starttimer, struct timespec *endtimer)
{
    if (clock_gettime(CLOCK_REALTIME, endtimer) < 0)
        error("Error getting end time");
    long runtime = 1000000000l * (endtimer->tv_sec - starttimer->tv_sec) + endtimer->tv_nsec - starttimer->tv_nsec;
    return runtime;
}

void signalHandler()
{
    fprintf(stderr, "Error: Segmentation Fault\n");
    exit(2);
}

void yieldargs(char *optarg)
{
    int len = strlen(optarg);
    if (len > 3)
    {
        fprintf(stderr, "Too many arguments passed to --yield\n");
        exit(1);
    }

    for (int i = 0; i < len; i++)
    {
        switch (optarg[i])
        {
        case 'i':
            opt_yield |= INSERT_YIELD;
            strcat(yieldOpts, "i");
            break;
        case 'd':
            opt_yield |= DELETE_YIELD;
            strcat(yieldOpts, "d");
            break;
        case 'l':
            opt_yield |= LOOKUP_YIELD;
            strcat(yieldOpts, "l");
            break;
        default:
            fprintf(stderr, "Invalid arguments passed to --yield\n");
            exit(1);
        }
    }
}

void *listRoutine(void *arg)
{
    struct timespec start;
    struct timespec end;
    long thread_lock_time = 0;
    unsigned int index;

    int *threadNum = (int *)(arg);

    int i = *threadNum * iterations;

    for (; i < (*threadNum + 1) * iterations; i++)
    {
        if (listOption[0] == 'N')
        {
            index = getHash(elements[i].key);
            SortedList_insert(&list[index], &elements[i]);
        }
        else if (listOption[0] == 'm')
        {
            index = getHash(elements[i].key);
            setStartTime(&start);
            pthread_mutex_lock(&lock[index]);
            thread_lock_time += Runtime(&start, &end);
            SortedList_insert(&list[index], &elements[i]);
            pthread_mutex_unlock(&lock[index]);
        }
        else if (listOption[0] == 's')
        {
            index = getHash(elements[i].key);
            setStartTime(&start);
            while (__sync_lock_test_and_set(&spinlock[index], 1))
                ;
            thread_lock_time += Runtime(&start, &end);
            SortedList_insert(&list[index], &elements[i]);
            __sync_lock_release(&spinlock[index]);
        }
    }

    int len = 0;
    if (listOption[0] == 'N')
    {
        for (int i = 0; i < numLists; i++)
        {
            int templen = SortedList_length(&list[i]);
            if (templen == -1)
            {
                fprintf(stderr, "Error getting length of list: Corrupted List\n");
                exit(2);
            }
            len += templen;
        }
    }
    else if (listOption[0] == 'm')
    {
        for (int i = 0; i < numLists; i++)
        {
            setStartTime(&start);
            pthread_mutex_lock(&lock[i]);
            thread_lock_time += Runtime(&start, &end);
            int templen = SortedList_length(&list[i]);
            pthread_mutex_unlock(&lock[i]);
            if (templen == -1)
            {
                fprintf(stderr, "Error getting length of list: Corrupted List\n");
                exit(2);
            }
            len += templen;
        }
    }
    else if (listOption[0] == 's')
    {
        for (int i = 0; i < numLists; i++)
        {
            setStartTime(&start);
            while (__sync_lock_test_and_set(&spinlock[i], 1))
                ;
            thread_lock_time += Runtime(&start, &end);
            int templen = SortedList_length(&list[i]);
            __sync_lock_release(&spinlock[i]);
            if (templen == -1)
            {
                fprintf(stderr, "Error getting length of list: Corrupted List\n");
                exit(2);
            }
            len += templen;
        }
    }

    SortedListElement_t *element;

    int j = *threadNum * iterations;
    for (; j < (*threadNum + 1) * iterations; j++)
    {
        if (listOption[0] == 'N')
        {
            index = getHash(elements[j].key);
            element = SortedList_lookup(&list[index], elements[j].key);
            if (element == NULL)
            {
                fprintf(stderr, "Error looking up element: Corrupted List\n");
                exit(2);
            }
            if (SortedList_delete(element) == 1)
            {
                fprintf(stderr, "Error deleting element: Corrupted List\n");
                exit(2);
            }
        }
        else if (listOption[0] == 'm')
        {
            index = getHash(elements[j].key);
            setStartTime(&start);
            pthread_mutex_lock(&lock[index]);
            thread_lock_time += Runtime(&start, &end);
            element = SortedList_lookup(&list[index], elements[j].key);
            if (element == NULL)
            {
                fprintf(stderr, "Error looking up Element: Corrupted List\n");
                exit(2);
            }
            if (SortedList_delete(element) == 1)
            {
                fprintf(stderr, "Error deleting element: Corrupted List\n");
                exit(2);
            }
            pthread_mutex_unlock(&lock[index]);
        }
        else if (listOption[0] == 's')
        {
            index = getHash(elements[j].key);
            setStartTime(&start);
            while (__sync_lock_test_and_set(&spinlock[index], 1))
                ;
            thread_lock_time += Runtime(&start, &end);
            element = SortedList_lookup(&list[index], elements[j].key);
            if (element == NULL)
            {
                fprintf(stderr, "Error looking up Element: Corrupted List\n");
                exit(2);
            }
            if (SortedList_delete(element) == 1)
            {
                fprintf(stderr, "Error deleting element: Corrupted List\n");
                exit(2);
            }
            __sync_lock_release(&spinlock[index]);
        }
    }

    return (void *)(thread_lock_time);
}

int main(int argc, char **argv)
{
    signal(SIGSEGV, signalHandler);
    char testname[15] = "list-\0";
    listOption[0] = 'N';
    listOption[1] = '\0';
    iterations = -1;
    threads = -1;
    struct timespec endtime;
    struct timespec starttime;
    int mutex = 0;
    long total_thread_lock_time = 0;
    numLists = 1;

    static struct option long_options[] = {
        {"threads", required_argument, NULL, 1},
        {"iterations", required_argument, NULL, 2},
        {"yield", required_argument, NULL, 3},
        {"sync", required_argument, NULL, 4},
        {"lists", required_argument, NULL, 5},
        {0, 0, 0, 0}};

    while (1)
    {
        int c;
        c = getopt_long(argc, argv, "", long_options, NULL);
        if (c == -1)
            break;
        switch (c)
        {
        case 1:
            if (!argChecker(optarg))
            {
                fprintf(stdout, "Error, only positive integer values allowed for --threads argument.\n");
                exit(1);
            }
            threads = atoi(optarg);
            break;

        case 2:
            if (!argChecker(optarg))
            {
                fprintf(stdout, "Error, only positive integer values allowed for --iterations argument.\n");
                exit(1);
            }
            iterations = atoi(optarg);
            break;

        case 3:
            opt_yield = 1;
            yieldargs(optarg);
            break;

        case 4:
            if (!strcmp(optarg, "m"))
            {
                mutex = 1;
                listOption[0] = 'm';
            }
            else if (!strcmp(optarg, "s"))
            {
                listOption[0] = 's';
            }
            else
            {
                error("The --sync options are: m, s");
            }

            break;
        case 5:
            if (!argChecker(optarg))
            {
                fprintf(stdout, "Error, only positive integer values allowed for --lists argument.\n");
                exit(1);
            }
            numLists = atoi(optarg);
            break;

        default:
            fprintf(stderr, "Unrecognized argument. The options are: \n--thread=#\n--iterations=#\n--yield=[idl]\n--sync=_\n");
            exit(1);
        }
    }
    if (opt_yield)
    {
        strcat(testname, yieldOpts);
        strcat(testname, "-");
    }
    else
    {
        strcat(testname, "none-");
    }

    if (strcmp(listOption, "N"))
        strcat(testname, listOption);
    else
        strcat(testname, "none");

    if (threads < 0)
        threads = 1;
    if (iterations < 0)
        iterations = 1;

    list = malloc(sizeof(SortedList_t) * numLists);
    for (int i = 0; i < numLists; i++)
    {
        list[i].next = &list[i];
        list[i].prev = &list[i];
        list[i].key = NULL;
    }

    numElements = threads * iterations;

    elements = malloc(numElements * sizeof(SortedListElement_t));

    srand(time(NULL));
    for (int i = 0; i < numElements; i++)
    {
        char *newKey = malloc(2 * sizeof(char));
        if ((rand() % 2) == 1)
            newKey[0] = ((rand() % 26) + 'a');
        else
            newKey[0] = ((rand() % 26) + 'A');
        newKey[1] = '\0';
        elements[i].key = newKey;
    }

    spinlock = malloc(sizeof(volatile int) * numLists);
    lock = malloc(sizeof(pthread_mutex_t) * numLists);

    if (mutex == 1)
    {
        for (int i = 0; i < numLists; i++)
        {
            int rc = pthread_mutex_init(&lock[i], NULL);
            if (rc != 0)
                error("Error creating mutex lock");
        }
    }

    int *thread_nums = malloc(threads * sizeof(int));

    setStartTime(&starttime);    

    pthread_t threadArr[threads];
    for (int i = 0; i < threads; i++)
    {
        thread_nums[i] = i;
        if (pthread_create(&threadArr[i], NULL, listRoutine, &thread_nums[i]) != 0)
            error("Error creating thread");
    }

    void *thread_lock_time;
    for (int i = 0; i < threads; i++)
    {
        if (pthread_join(threadArr[i], &thread_lock_time) != 0)
            error("Error joining thread");
        total_thread_lock_time += (long)(thread_lock_time);
    }

    long runtime = Runtime(&starttime, &endtime);

    int length = SortedList_length(list);
    if (length != 1)
    {
        fprintf(stderr, "Error: list length is: %d, not one\n", length);
        exit(2);
    }

    long operations = threads * iterations * 3;
    long wait_for_lock_time = total_thread_lock_time / operations;
    long time_per_operation = runtime / operations;

    fprintf(stdout, "%s,%d,%d,%d,%ld,%ld,%ld,%ld\n", testname, threads, iterations, numLists, operations, runtime, time_per_operation, wait_for_lock_time);

    exit(0);
}
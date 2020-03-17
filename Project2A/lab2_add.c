//NAME: Ryan Riahi
//EMAIL: ryanr08@gmail.com
//ID: 105138860

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

int iterations;
int opt_yield;
pthread_mutex_t lock;
char addOption;
volatile int spinlock;

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void mutexAdd(long long *pointer, long long value)
{
    pthread_mutex_lock(&lock);
    long long sum = *pointer + value;
    *pointer = sum;
    pthread_mutex_unlock(&lock);
}

void compswapAdd(long long *pointer, long long value)
{
    long long old;
    long long new;
    do
    {
        old = *pointer;
        new = old + value;
        if (opt_yield)
            sched_yield();
    } while (old != __sync_val_compare_and_swap(pointer, old, new));
    
}

void spinAdd(long long *pointer, long long value)
{

    while (__sync_lock_test_and_set(&spinlock, 1))
        while(spinlock)
            ; 
    long long sum = *pointer + value;
    if (opt_yield)
        sched_yield();

    *pointer = sum;
    __sync_lock_release(&spinlock);
}

void add(long long *pointer, long long value)
{
    long long sum = *pointer + value;
    if (opt_yield)
        sched_yield();
    *pointer = sum;
}

void *threadAddition(void *arg)
{
    long long *counter = (long long *)(arg);
    for (int i = 0; i < iterations; i++)
    {
        if (addOption == 'N')
            add(counter, 1);
        else if (addOption == 'm')
            mutexAdd(counter, 1);
        else if (addOption == 's')
            spinAdd(counter, 1);
        else if (addOption == 'c')
            compswapAdd(counter, 1);
    }
    for (int i = 0; i < iterations; i++)
    {
        if (addOption == 'N')
            add(counter, -1);
        else if (addOption == 'm')
            mutexAdd(counter, -1);
        else if (addOption == 's')
            spinAdd(counter, -1);
        else if (addOption == 'c')
            compswapAdd(counter, -1);
    }
    return NULL;
}

int main(int argc, char **argv)
{
    char testname[15] = "add-\0";
    addOption = 'N';
    opt_yield = 0;
    iterations = -1;
    int threads = -1;
    long long x = 0;
    long long *counter = &x;
    struct timespec endtime;
    struct timespec starttime;
    int mutex = 0;
    spinlock = 0;

    static struct option long_options[] = {
        {"threads", required_argument, NULL, 1},
        {"iterations", required_argument, NULL, 2},
        {"yield", no_argument, NULL, 3},
        {"sync", required_argument, NULL, 4},
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
            threads = atoi(optarg);
            break;

        case 2:
            iterations = atoi(optarg);
            break;

        case 3:
            opt_yield = 1;
            break;

        case 4:
            if (!strcmp(optarg, "m"))
            {
                mutex = 1;
                addOption = 'm';
            }
            else if (!strcmp(optarg, "s"))
            {
                addOption = 's';
            }
            else if (!strcmp(optarg, "c"))
            {
                addOption = 'c';
            }
            else
            {
                error("The --sync options are: m, s, c");
            }
            
            break;

        default:
            fprintf(stderr, "Unrecognized argument. The options are: \n--thread=#\n--iterations=#\n--yield\n--sync=_\n");
            exit(1);
        }
    }
    if (opt_yield)
        strcat(testname, "yield-");
    
    if (addOption != 'N')
        strcat(testname, &addOption);

    if (strcmp(testname, "add-") == 0 || strcmp(testname, "add-yield-") == 0)
        strcat(testname, "none");

    if (threads < 0)
        threads = 1;
    if (iterations < 0)
        iterations = 1;

    if (mutex == 1)
    {
        int rc = pthread_mutex_init(&lock, NULL);
        if (rc != 0)
            error("Error creating mutex lock");
    }
    if (clock_gettime(CLOCK_REALTIME, &starttime) < 0)
        error("Error getting start time");

    pthread_t threadArr[threads];
    for (int i = 0; i < threads; i++)
    {
        if (pthread_create(&threadArr[i], NULL, threadAddition, counter) != 0)
            error("Error creating thread");
    }

    for (int i = 0; i < threads; i++)
    {
        if (pthread_join(threadArr[i], NULL) != 0)
            error("Error joining thread");
    }
    if (clock_gettime(CLOCK_REALTIME, &endtime) < 0)
        error("Error getting end time");

    long operations = threads * iterations * 2;
    long runtime = 1000000000l * (endtime.tv_sec - starttime.tv_sec) + endtime.tv_nsec - starttime.tv_nsec;
    long time_per_operation = runtime / operations;

    fprintf(stdout, "%s,%d,%d,%ld,%ld,%ld,%lld\n", testname, threads, iterations, operations, runtime, time_per_operation, *counter);

    exit(0);
}
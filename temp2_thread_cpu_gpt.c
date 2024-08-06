#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <err.h>
#include <assert.h>

#define BLOCK_SIZE 1024  // Size of each memory block
#define TWO_MB (2044 * 1024)

void *current_break;
void *current_ret_addr;
void *next_ret_addr;

int is_first_time = 1;
int do_alloc = 1;

pthread_mutex_t alloc_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int size;
    void *result;
} ThreadArgs;

pthread_t t_id;
cpu_set_t cpuset;


void *my_sbrk(void *args_) {
    ThreadArgs *args = (ThreadArgs *)args_;
    int size = args->size;


    if (size < TWO_MB) {
        pthread_mutex_lock(&alloc_lock);
        current_break = sbrk((ptrdiff_t)TWO_MB);
        pthread_mutex_unlock(&alloc_lock);

        printf("INF, cur %d %p\n", BLOCK_SIZE, current_break);
        if (current_break == (void *)-1) {
            perror("sbrk");
            pthread_exit(NULL);
        }
        pthread_exit(NULL);
    } else {
        current_break = sbrk(size);
        printf("cur %d %p\n", BLOCK_SIZE, current_break);
        if (current_break == (void *)-1) {
            perror("sbrk");
            pthread_exit(NULL);
        }
    }
    pthread_exit(current_break);
}

void *my_malloc(size_t size) {
    printf("Inside my_malloc\n");
    if (is_first_time == 1 || do_alloc == 1) {
        is_first_time = 0;
        do_alloc = 0;

        ThreadArgs args = {size, NULL};
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        clock_t start_sbrk = clock();
        int thread_create_status = pthread_create(&t_id, &attr, my_sbrk, &args);
        if (thread_create_status != 0) {
            fprintf(stderr, "Error creating thread: %s\n", strerror(thread_create_status));
            exit(EXIT_FAILURE);
        }

        printf("CPU : %d\n", pthread_getaffinity_np(t_id, sizeof(cpu_set_t), &cpuset));

        // Set the CPU affinity for the newly created thread
        CPU_ZERO(&cpuset);
        CPU_SET(1, &cpuset);  // Pin the thread to CPU 1
        int set_affinity = pthread_setaffinity_np(t_id, sizeof(cpu_set_t), &cpuset);


        if (set_affinity != 0) {
            perror("pthread_setaffinity_np");
            exit(EXIT_FAILURE);
        }

        pthread_attr_destroy(&attr);

        int join_status = pthread_join(t_id, NULL);
        if (join_status != 0) {
            fprintf(stderr, "Error joining thread: %s\n", strerror(join_status));
            exit(EXIT_FAILURE);
        }

        clock_t end_sbrk = clock();
        printf("Time for just sbrk: %ld\n", (end_sbrk - start_sbrk));

        current_ret_addr = current_break;
        next_ret_addr = current_ret_addr + size;
        return current_ret_addr;
    } else {
        if ((next_ret_addr + size - current_break) > TWO_MB) {
            do_alloc = 1;

            ThreadArgs args = {size, NULL};
            int thread_create_status = pthread_create(&t_id, NULL, my_sbrk, &args);
            if (thread_create_status != 0) {
                fprintf(stderr, "Error creating thread: %s\n", strerror(thread_create_status));
                exit(EXIT_FAILURE);
            }

            // Set the CPU affinity for the newly created thread
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(1, &cpuset);  // Pin the thread to CPU 1
            int set_affinity = pthread_setaffinity_np(t_id, sizeof(cpu_set_t), &cpuset);
            if (set_affinity != 0) {
                perror("pthread_setaffinity_np");
                exit(EXIT_FAILURE);
            }


            int join_status = pthread_join(t_id, NULL);
            if (join_status != 0) {
                fprintf(stderr, "Error joining thread: %s\n", strerror(join_status));
                exit(EXIT_FAILURE);
            }

            current_ret_addr = current_break;
            next_ret_addr = current_ret_addr + size;
            return current_ret_addr;
        } else {
            current_ret_addr = next_ret_addr;
            next_ret_addr = next_ret_addr + size;
            return current_ret_addr;
        }
    }
}

void my_free(void *ptr) {
    // sbrk() does not support freeing memory directly.
    // You would need to implement your own memory management to handle this.
    printf("Hello\n");
}

int main() {
    printf("Allocating memory using sbrk()\n");

    // Checking time for the first time allocation
    clock_t start_my_malloc = clock();
    int *arr = (int *)my_malloc(10000 * sizeof(int));
    clock_t end_my_malloc = clock();

    // Checking time for the second time allocation
    clock_t start_my_malloc2 = clock();
    int *arr1 = (int *)my_malloc(10 * sizeof(int));
    clock_t end_my_malloc2 = clock();

    arr1[0] = 5;
    printf("Time for my_malloc: %ld\n", (end_my_malloc - start_my_malloc));
    printf("Time for my_malloc2: %ld, %d\n", (end_my_malloc2 - start_my_malloc2), arr1[0]);

    clock_t start_malloc = clock();
    int *arr2 = (int *)malloc(1000 * sizeof(int));
    clock_t end_malloc = clock();

    // Checking time for the second time allocation
    clock_t start_malloc2 = clock();
    int *arr3 = (int *)malloc(100 * sizeof(int));
    clock_t end_malloc2 = clock();

    arr1[0] = 5;
    printf("Time for malloc: %ld\n", (end_malloc - start_malloc));
    printf("Time for malloc2: %ld, %d\n", (end_malloc2 - start_malloc2), arr1[0]);

    printf("arr: %p\n", arr);
    if (!arr) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Initialize allocated memory
    for (int i = 0; i < BLOCK_SIZE; i++) {
        arr[i] = i;
    }

    // Print some of the allocated memory
    for (int i = 0; i < 10; ++i) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }

    // Note: my_free() is not implemented because sbrk() does not support freeing memory
    // However, you can use sbrk() to allocate more memory if needed

    return 0;
}

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
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_t t_id;
cpu_set_t cpuset;

int size_to_allocate;
int ready_to_allocate = 0;

typedef struct {
    int size;
    void *result;
} ThreadArgs;

void *my_sbrk(void *args_) {
    while (1) {
        pthread_mutex_lock(&alloc_lock);

        // Wait until there's an allocation request
        while (!ready_to_allocate) {
            pthread_cond_wait(&cond, &alloc_lock);
        }

        int size = size_to_allocate;
        if (size < TWO_MB) {
            current_break = sbrk((ptrdiff_t)TWO_MB);
            printf("INF, cur %d %p\n", BLOCK_SIZE, current_break);
            if (current_break == (void *)-1) {
                perror("sbrk");
                pthread_mutex_unlock(&alloc_lock);
                continue;
            }
        } else {
            current_break = sbrk(size);
            printf("cur %d %p\n", BLOCK_SIZE, current_break);
            if (current_break == (void *)-1) {
                perror("sbrk");
                pthread_mutex_unlock(&alloc_lock);
                continue;
            }
        }

        // Signal that the allocation is done
        ready_to_allocate = 0;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&alloc_lock);
    }

    return NULL;
}

void *my_malloc(size_t size) {
    printf("Inside my_malloc\n");
    if (is_first_time == 1 || do_alloc == 1) {
        is_first_time = 0;
        do_alloc = 0;

        pthread_mutex_lock(&alloc_lock);

        // Set the allocation request
        size_to_allocate = size;
        ready_to_allocate = 1;

        // Signal the sbrk thread to perform the allocation
        pthread_cond_signal(&cond);

        // Wait for the allocation to complete
        while (ready_to_allocate) {
            pthread_cond_wait(&cond, &alloc_lock);
        }

        pthread_mutex_unlock(&alloc_lock);

        current_ret_addr = current_break;
        next_ret_addr = current_ret_addr + size;
        return current_ret_addr;
    } else {
        if ((next_ret_addr + size - current_break) > TWO_MB) {
            do_alloc = 1;

            pthread_mutex_lock(&alloc_lock);

            // Set the allocation request
            size_to_allocate = size;
            ready_to_allocate = 1;

            // Signal the sbrk thread to perform the allocation
            pthread_cond_signal(&cond);

            // Wait for the allocation to complete
            while (ready_to_allocate) {
                pthread_cond_wait(&cond, &alloc_lock);
            }

            pthread_mutex_unlock(&alloc_lock);

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

    // Create the global sbrk thread
    pthread_create(&t_id, NULL, my_sbrk, NULL);

    
    printf("CPU : %d\n", pthread_getaffinity_np(t_id, sizeof(cpu_set_t), &cpuset));

    // Set the CPU affinity for the newly created thread
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset);  // Pin the thread to CPU 1
    int set_affinity = pthread_setaffinity_np(t_id, sizeof(cpu_set_t), &cpuset);

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

    // Cancel the global thread
    pthread_cancel(t_id);
    pthread_join(t_id, NULL);

    return 0;
}

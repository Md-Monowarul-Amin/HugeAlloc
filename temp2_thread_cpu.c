#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <pthread.h>
#include <bits/pthreadtypes.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <err.h>

#include <assert.h>


#define BLOCK_SIZE 1024  // Size of each memory block
#define TWO_MB (2044 * 1024)
void * current_break;
void * current_ret_addr;
void * next_ret_addr;

int is_first_time = 1;
int do_alloc = 1;

// void CPU_ZERO(cpu_set_t *set);

// void CPU_SET(int cpu, cpu_set_t *set);

pthread_mutex_t alloc_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int size;
    void *result;
} ThreadArgs;

pthread_t t_id;

void *my_sbrk(void *args_) {

    ThreadArgs *args = (ThreadArgs *)args_;
    int size = args -> size;

    if( size < TWO_MB ){

        pthread_mutex_lock(&alloc_lock);
        current_break = sbrk((ptrdiff_t)TWO_MB);
        pthread_mutex_unlock(&alloc_lock);

        printf("INF, cur %d %p ", BLOCK_SIZE, current_break);
        if (current_break == (void *)-1) {
            perror("sbrk");
            // return NULL;
            pthread_exit(NULL);
        }
        pthread_exit(NULL);
    }
    else{
        current_break = sbrk(size);
        printf("cur %d %p ", BLOCK_SIZE, current_break);
        if (current_break == (void *)-1) {
            perror("sbrk");
            return NULL;
        }
    }
    return current_break;
}

void *my_malloc(size_t size) {
    printf("Inside my_alloc\n");
    pthread_attr_t attr;

    if(is_first_time ==1 || do_alloc == 1){
        is_first_time = 0;
        do_alloc = 0;


        // pthread_mutex_lock(&alloc_lock);
        ThreadArgs args = {size, NULL};
        cpu_set_t set;

        // CPU_ZERO(&set);
        // printf("CPU : %d",  sched_getaffinity(getpid(), sizeof(set), &set));
        clock_t start_sbrk = clock();
        pthread_attr_init(&attr);

        pthread_create(&t_id, &attr, my_sbrk, &args);
        pthread_attr_destroy(&attr);

        CPU_SET(3, &set);
        int set_affinity = sched_setaffinity(t_id, sizeof(cpu_set_t), &set);
        if(set_affinity == -1){
            perror("setaffinity");;
        }
        printf("CPU : %d",  sched_getaffinity(t_id, sizeof(cpu_set_t), &set));

        pthread_join(t_id, NULL);
        clock_t end_sbrk = clock();

        printf("Time for just sbrk: %ld\n", (end_sbrk - start_sbrk));

        // pthread_mutex_unlock(&alloc_lock);
        // void *current_break = my_sbrk(size);
        // if (!current_break) {
        //     return NULL;
        // }
        current_ret_addr = current_break;
        next_ret_addr = current_ret_addr + size + 4;
        return current_ret_addr;

    }
    else{
        if((next_ret_addr + size - current_break) > TWO_MB ){
            
            do_alloc = 1;

            ThreadArgs args = {size, NULL};
            pthread_create(&t_id, NULL, my_sbrk, &args);
            pthread_join(t_id, NULL);

            

            current_ret_addr = current_break;
            next_ret_addr = current_ret_addr + size + 4;
            return current_ret_addr;
        }

        else{
            current_ret_addr = next_ret_addr;
            next_ret_addr = next_ret_addr + size;
            return current_ret_addr;
        }
    }
}

void my_free(void *ptr) {
    // sbrk() does not support freeing memory directly.
    // You would need to implement your own memory management to handle this.
    printf("Hello");
}

int main() {
    printf("Allocating memory using sbrk()\n");

    // Allocate memory

    //Checking time for the first time allocation
    clock_t start_my_malloc = clock();

    int *arr = (int *)my_malloc(10000 * sizeof(int));

    clock_t end_my_malloc = clock();

    //Checking time for the second time allocation
    clock_t start_my_malloc2 = clock();

    int *arr1 = (int *)my_malloc(10 * sizeof(int));

    clock_t end_my_malloc2 = clock();


    arr1[0] = 5;
    printf("Time for my malloc: %ld\n", (end_my_malloc - start_my_malloc));
    printf("Time for my malloc2: %ld, %d\n", (end_my_malloc2 - start_my_malloc2), arr1[0]);

    clock_t start_malloc = clock();

    int *arr2 = (int *)malloc(1000 * sizeof(int));

    clock_t end_malloc = clock();

    //Checking time for the second time allocation
    clock_t start_malloc2 = clock();

    int *arr3 = (int *)malloc(100 * sizeof(int));

    clock_t end_malloc2 = clock();

 
    arr1[0] = 5;
    printf("Time for my malloc: %ld\n", (end_malloc - start_malloc));
    printf("Time for my malloc2: %ld, %d\n", (end_malloc2 - start_malloc2), arr1[0]);


    printf("arr:  %ls", arr);
    if (!arr) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Initialize allocated memory
    for(int i = 0; i < BLOCK_SIZE; i++) {
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

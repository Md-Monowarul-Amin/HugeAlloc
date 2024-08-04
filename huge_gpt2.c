#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define ONE_MB 1000000
#define TWO_MB (2 * 1024 * 1000)
#define PAGE_SIZE TWO_MB

pthread_mutex_t alloc_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int size;
    void *result;
} ThreadArgs;

void *HugeAlloc(void *args_) {
    ThreadArgs *args = (ThreadArgs *)args_;
    int size = args->size;

    printf("Allocating size: %d\n", size);

    void *current_break;
    pthread_mutex_lock(&alloc_lock);
    current_break = sbrk(TWO_MB);
    pthread_mutex_unlock(&alloc_lock);

    if (current_break == (void *)-1) {
        perror("sbrk");
        pthread_exit(NULL);
    }

    void *current_ret_ptr = current_break;
    args->result = current_ret_ptr;
    void *next_ret_ptr = current_break + size;

    pthread_exit(current_ret_ptr);
}

void *alloc(int size) {
    static void *current_break = NULL;
    static void *next_ret_ptr = NULL;
    static int initialized = 0;

    void *current_ret_ptr = NULL;

    pthread_mutex_lock(&alloc_lock);

    if (!initialized || (next_ret_ptr + size - current_break > TWO_MB)) {
        pthread_t t_id;
        ThreadArgs args = {size, NULL};
        pthread_create(&t_id, NULL, HugeAlloc, &args);
        pthread_join(&t_id, &current_ret_ptr);

        current_break = args.result;
        next_ret_ptr = current_break + size;
        initialized = 1;
    } else {
        current_ret_ptr = next_ret_ptr;
        next_ret_ptr += size;
    }

    pthread_mutex_unlock(&alloc_lock);

    return current_ret_ptr;
}

int main() {
    int malloc_size_i = 10;
    int malloc_size_j = 7;

    clock_t start_mlk = clock();
    clock_t time_for_allocation_start_mlk = clock();

    int **arr_malloc = malloc(malloc_size_i * sizeof(int *));
    if (arr_malloc == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    clock_t time_for_allocation_end_mlk = clock();
    printf("Time for allocation of malloc: %f \n", (double)(time_for_allocation_end_mlk - time_for_allocation_start_mlk));

    for (int i = 0; i < malloc_size_i; i++) {
        int *temp_arr = malloc(malloc_size_j * sizeof(int));
        if (temp_arr == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < malloc_size_j; j++) {
            temp_arr[j] = 1;
        }
        arr_malloc[i] = temp_arr;
    }

    clock_t end_mlk = clock();
    printf("Time for malloc: %f \n", (double)(end_mlk - start_mlk));

    clock_t start_brk = clock();
    printf("Starting brk allocation\n");
    clock_t time_for_allocation_start_brk = clock();

    int *arr = alloc(10 * sizeof(int));
    if (arr == NULL) {
        perror("alloc");
        exit(EXIT_FAILURE);
    }

    clock_t time_for_allocation_end_brk = clock();
    printf("Time for allocation of brk: %f\n", (double)(time_for_allocation_end_brk - time_for_allocation_start_brk));

    for (int i = 0; i < 10; i++) {
        arr[i] = 1;
    }

    clock_t end_brk = clock();
    printf("Time for brk: %f \n", (double)(end_brk - start_brk));

    printf("arr[0] = %d, arr[1] = %d\n", arr[0], arr[1]);

    // Free allocated memory
    for (int i = 0; i < malloc_size_i; i++) {
        free(arr_malloc[i]);
    }
    free(arr_malloc);

    return 0;
}

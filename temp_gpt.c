#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

#define BLOCK_SIZE 1024  // Size of each memory block

void *my_sbrk(ptrdiff_t increment) {
    void *ptr = sbrk(increment);
    if (ptr == (void *)-1) {
        perror("sbrk");
        return NULL;
    }
    return ptr;
}

void *my_malloc(size_t size) {
    void *ptr = my_sbrk(size);
    if (!ptr) {
        return NULL;
    }
    return ptr;
}

void my_free(void *ptr) {
    // sbrk() does not support freeing memory directly.
    // You would need to implement your own memory management to handle this.
}

int main() {
    printf("Allocating memory using sbrk()\n");

    // Allocate memory
    int *arr = (int *)my_malloc(BLOCK_SIZE * sizeof(int));
    if (!arr) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Initialize allocated memory
    for (int i = 0; i < BLOCK_SIZE; ++i) {
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

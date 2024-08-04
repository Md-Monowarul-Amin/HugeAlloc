#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

#define BLOCK_SIZE 1024  // Size of each memory block
#define TWO_MB (2044 * 1024)
void * current_break;
void * current_ret_addr;
void * next_ret_addr;

int is_first_time = 1;
int do_alloc = 1;

void *my_sbrk(ptrdiff_t size) {

    if( size < TWO_MB ){
        current_break = sbrk((ptrdiff_t)TWO_MB);
        printf("INF, cur %d %p ", BLOCK_SIZE, current_break);
        if (current_break == (void *)-1) {
            perror("sbrk");
            return NULL;
        }
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
    if(is_first_time ==1 || do_alloc == 1){

        void *current_break = my_sbrk(size);
        if (!current_break) {
            return NULL;
        }
        current_ret_addr = current_break;
        next_ret_addr = current_ret_addr + size + 4;
        return current_ret_addr;

    }
    else{
        if((next_ret_addr + size - current_break) < TWO_MB ){
            

            void *current_break = my_sbrk(size);
            if (!current_break) {
                return NULL;
            }
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
    int *arr = (int *)my_malloc(1000000000 * sizeof(int));

    printf("arr:  %ls", arr);
    if (!arr) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Initialize allocated memory
    for(int i = 0; i < BLOCK_SIZE; ++i) {
        arr[i] = i;
    }

    // Print some of the allocated memory
    for (int i = 0; i < 1000; ++i) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }

    // Note: my_free() is not implemented because sbrk() does not support freeing memory
    // However, you can use sbrk() to allocate more memory if needed

    return 0;
}

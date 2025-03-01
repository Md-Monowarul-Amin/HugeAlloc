#include <stdint.h>
#include <stdio.h>
#include<unistd.h>
// #include "SmallAllocation.h"
// #include "PageAllocation.h"
#include <time.h>
#include <stdlib.h>

#define ONE_MB 1000000
#define TWO_MB (2 * 1024 * 1000)
#define PAGE_SIZE TWO_MB

struct PageAllocation * allocation_table[64];

int current_AT_index = 0;
void * current_ret_ptr; //THe pointer to return
void * current_break;
void * next_ret_ptr;

int do_alloc = 1;

void * HugeAlloc(ssize_t size){


    if (current_AT_index == 0 || do_alloc == 1){
        // struct PageAllocation * temp_allocation;
        // allocation_table[current_AT_index] = temp_allocation;
        current_AT_index += 1;
        do_alloc = 0;

        

        // printf("PID HugeAlloc: %d", getpid());

        // int brk()

        
        //Allocate a Huge Page
        // printf("inside 2MB %d", size); 

        // clock_t system_call_jumping_starts = clock();

        current_break = sbrk(TWO_MB);   

        // clock_t system_call_jumping_ends = clock();

        // printf("Systemcall jumping time: %f \n", (double)(system_call_jumping_ends - system_call_jumping_starts) );
        current_ret_ptr = current_break;

        next_ret_ptr = current_break + size;

        
        // printf("%f", current_break);




        //Allocate left of the PageAllocation to Null
        // temp_allocation -> left = NULL;

        // //Set right of the Page Allocation to a SmallAllocation of given Size
        // struct SmallAllocation * first_allocation;
        // first_allocation -> current_size = size;
        // first_allocation -> left = NULL;
        // first_allocation -> size_sum_of_sibblings = size;
        // temp_allocation->right = (int *) first_allocation;

        // temp_allocation -> total_size = size;

        return current_ret_ptr;
    }
    else{
        if((next_ret_ptr + size - current_break) <= TWO_MB){
            
            // printf("Achieved : %ld", (next_ret_ptr + size - current_break));
            current_ret_ptr = next_ret_ptr;
            next_ret_ptr += size;
            return current_ret_ptr; 
        }
        else{
            do_alloc = 1;
            
            return HugeAlloc(size);
        }
    }
}


void * alloc(ssize_t size){
    if(current_AT_index != 0 || do_alloc == 0){
         if((next_ret_ptr + size - current_break) <= TWO_MB){
            
            // printf("Achieved : %ld", (next_ret_ptr + size - current_break));
            current_ret_ptr = next_ret_ptr;
            next_ret_ptr += size;
            return current_ret_ptr; 
        }
        else{
            do_alloc = 1;
            
            return HugeAlloc(size);
        }
    }
}


// int main(){


    // // struct PageAllocation *temp_huge;
    // // temp_huge->total_size = 0;

    // int malloc_size_i = 1000000;
    // int malloc_size_j = 700;

    // int brk_size_i = 1000000;
    // int brk_size_j = 700;

    // clock_t start_mlk = clock();

    // clock_t time_for_allocation_start_mlk = clock();

    // int * arr_malloc = malloc(malloc_size_i * sizeof(int));

    // clock_t time_for_allocation_end_mlk = clock();

    // printf("Time for allocation of mlk: %f \n", (double) (time_for_allocation_end_mlk - time_for_allocation_start_mlk));


    // for(int i=0; i < malloc_size_i; i++){
    //     // int * arr = HugeAlloc(1);
        
    //     int * temp_arr = malloc(malloc_size_j * sizeof(int));

    //     for(int j = 0; j < malloc_size_j; j++){
    //         temp_arr[j] = 1;
    //     }
    //     arr_malloc[i] = (int *) temp_arr;
    //     // arr[10000] = 2;
    // }

    // clock_t end_mlk = clock();

    // printf("time for mlk: %f \n", (double)(end_mlk - start_mlk));





    // // printf("%p", arr);

    // clock_t start_brk = clock();

    // clock_t time_for_allocation_start_brk = clock();

    // int * arr = HugeAlloc(brk_size_i * sizeof(int));

    // clock_t time_for_allocation_end_brk = clock();

    // printf("Time for allocation of brk: %ld \n", (time_for_allocation_end_brk - time_for_allocation_start_brk));


    // for(int i=0; i < brk_size_i; i++){

    //     int * temp_arr =  HugeAlloc(brk_size_j  * sizeof(int));

    //     for(int j = 0; j < brk_size_j; j++){
    //         temp_arr[j] = 1;
    //     }
    //     arr[i] = temp_arr;

    //     // int * arr = HugeAlloc(1);
    //     // arr[10000] = 2;
    // }

    // clock_t end_brk = clock();

    // printf("time for brk: %f \n", (double)(end_brk - start_brk));





    // // printf("%d", *arr);




    // printf("arr[0] = %d, arr[1] = %d\n", arr[0], arr[1]);

    // printf("Pid_Main: %d", getpid());
    // return 0;
// }
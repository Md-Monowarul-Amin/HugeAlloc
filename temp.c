#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include "HugeAlloc.c"






int main(){
    // int n = 10;
    // int *marks;

    // marks = (int *) malloc(n * sizeof(int));

    // for (int i=0; i<10; i++){
    //     marks[i] = i+1;
    // }

    // for(int i=0; i< 10; i++){
    //     printf("Marks %d", marks[i]);
    // }
    // printf("\n");
    
    // return 0;


        // struct PageAllocation *temp_huge;
    // temp_huge->total_size = 0;

    int malloc_size_i = 10000;
    int malloc_size_j = 700;

    int brk_size_i = 10000;
    int brk_size_j = 700;

    clock_t start_mlk = clock();

    clock_t time_for_allocation_start_mlk = clock();

    int * arr_malloc = malloc(malloc_size_i * sizeof(int));

    clock_t time_for_allocation_end_mlk = clock();

    printf("Time for allocation of mlk: %f \n", (double) (time_for_allocation_end_mlk - time_for_allocation_start_mlk));


    for(int i=0; i < malloc_size_i; i++){
        // int * arr = HugeAlloc(1);
        
        int * temp_arr = malloc(malloc_size_j * sizeof(int));

        for(int j = 0; j < malloc_size_j; j++){
            temp_arr[j] = 1;
        }
        arr_malloc[i] = (int *) temp_arr;
        // arr[10000] = 2;
    }

    clock_t end_mlk = clock();

    printf("time for mlk: %f \n", (double)(end_mlk - start_mlk));





    // printf("%p", arr);

    clock_t start_brk = clock();

    clock_t time_for_allocation_start_brk = clock();

    int * arr = HugeAlloc(brk_size_i * sizeof(int));

    clock_t time_for_allocation_end_brk = clock();

    printf("Time for allocation of brk: %ld \n", (time_for_allocation_end_brk - time_for_allocation_start_brk));


    for(int i=0; i < brk_size_i; i++){

        int * temp_arr =  HugeAlloc(brk_size_j  * sizeof(int));

        for(int j = 0; j < brk_size_j; j++){
            temp_arr[j] = 1;
        }
        arr[i] = temp_arr;

        // int * arr = HugeAlloc(1);
        // arr[10000] = 2;
    }

    clock_t end_brk = clock();

    printf("time for brk: %f \n", (double)(end_brk - start_brk));





    // printf("%d", *arr);




    printf("arr[0] = %d, arr[1] = %d\n", arr[0], arr[1]);

    printf("Pid_Main: %d", getpid());
    return 0;

}
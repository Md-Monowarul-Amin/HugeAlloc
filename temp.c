#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

struct temp
{
    /* data */
};



 
int main(){
    int n = 10;
    int *marks;

    // marks = (int *) malloc(n * sizeof(int));



    // for (int i=0; i<10; i++){
    //     marks[i] = i+1;
    // }

    // for(int i=0; i< 10; i++){
    //     printf("Marks %d", marks[i]);
    // }
    // printf("\n");

    void * curr_ = sbrk(400);

    int * curr = (int *) curr_;

    curr[0] = 4;

    printf("Curr[0] = %d", curr[0]);

    
    return 0;
}
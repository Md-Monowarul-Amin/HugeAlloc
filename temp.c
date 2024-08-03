#include<stdio.h>
#include <stdlib.h>


struct temp
{
    /* data */
};




int main(){
    int n = 10;
    int *marks;

    marks = (int *) malloc(n * sizeof(int));

    for (int i=0; i<10; i++){
        marks[i] = i+1;
    }

    for(int i=0; i< 10; i++){
        printf("Marks %d", marks[i]);
    }
    printf("\n");
    
    return 0;
}
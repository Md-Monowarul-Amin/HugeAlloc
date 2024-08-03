#include <stdio.h>
#include<unistd.h>

struct PageAllocation
{
    int total_size;
    int * start_index;
    int * left;
    int * right;
    /* data */
};


// int main(){
//     return 0;
// }
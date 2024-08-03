#include<stdio.h>
#include<unistd.h>

struct SmallAllocation
{
    int current_size;
    struct SmallAllocation *left;
    struct SmallAllocation *right;
    int size_sum_of_sibblings;    
    /* data */
};



// int main(){
//     return 0;
// }
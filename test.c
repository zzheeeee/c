#include<stdio.h>
#define SIZE 10

int main(void)
{
    int lst_a[SIZE] = {1,2,3,4,5,6,7,8,9,10};
    int temp[SIZE] = {0};
    int i;
    for (i = 0; i < 5; i++)
    {
        temp[i] = lst_a[9-i];
        lst_a[9-i] = lst_a[i];
        lst_a[i] = temp[i];
    }

    for(i=0; i<10; i++){
        printf("%d ", lst_a[i]);
    }
    printf("\n");

    return 0;
}
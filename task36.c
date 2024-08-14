/* 10부터 1까지의 수를 배열에 저장 */

#include <stdio.h>

int main(void)
{
    int i;
    int a[10];

    for(i=10; i>=1; i--)
    {
        a[10-i] = i;
    }  
    for(i=0; i<10; i++)
    {
        printf("%d ", a[i]);
    }
    
    printf("\n\n");

    return 0;
}
/* 10, 20, 30, ..., 100 저장하고, 거꾸로 출력하기 */

#include <stdio.h>

int main(void)
{
    int i;
    int a[10];
    
    for (i=1; i<=10; i++)
    {
        a[i-1] = i *10;
    }
    
    for (i=9; i>=0; i--)
    {
        printf("%d ",a[i]);
    }

    printf("\n\n");
 
    return 0;
}
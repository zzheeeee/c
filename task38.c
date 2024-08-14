/* 배열 요소를 오른쪽으로 한 칸씩 원형으로 이동하기 */

#include <stdio.h>

int main(void)
{
    int i, temp;
    int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    temp = a[9];
   
    for(i=9; i>0; i--)
    {
        a[i] = a[i-1];
    }
    
    a[0] = temp;

    for(i=0; i<10; i++)
    {
        printf("%d ", a[i]);
    }

    printf("\n\n");

    return 0;
}
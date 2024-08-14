/* 1, -2, 3, -4, ..., 99, -100의 합 구하기 */

#include <stdio.h>

int main(void)
{
    int sum = 0;

    for(int i=1; i<=100; i++)
    {
        if(i%2 == 0)
        {
            sum = sum - i;
        }
        else
        {
            sum = sum + i;
        }
    }

    printf("%d\n\n", sum);
 
    return 0;
}
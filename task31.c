/* 피보나치수열의 20번째 항까지의 합 */

#include <stdio.h>

int main(void)
{
    int a, b = 1;
    int c = 0;
    int sum = a+b;
    int cnt;
    
    for (cnt=2; cnt<21;)
    {
        c = a+b;
        sum = sum+c;
        cnt++;
        a = b;
        b = c;
    }

    printf("%d\n\n", sum);
    
    return 0;
}
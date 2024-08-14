/* 재귀함수를 이용해서 입력 받은 수에 대한 계승 */

#include <stdio.h>

int fact(int num);

int main(void)
{
    printf("%d\n\n", fact(10));
 
    return 0;
}

int fact(int num)
{
    if (num == 1)
    {
        return 1;
    }
    else
    {
        return num * fact(num-1);
    }
}
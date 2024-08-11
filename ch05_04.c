/* 1부터 20까지의 정수와 그들의 제곱값 */
#include <stdio.h>
int main(void)
{
    int num = 1;
    
    printf("  수   제곱\n");
    while (num < 21)
    {
        printf("%4d %6d\n", num, num * num);
        num = num + 1;
    }
    
    return 0;
}

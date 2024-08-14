/* 약수 구하기 */

#include <stdio.h>

int main(void)
{
    int num = 0;
    printf("정수 : ");
    scanf("\n%d", &num);

    for(int i=1; i<=num; i++)
    {
        if(num%i == 0)
        {
            printf("%d ", i);
        }
    }

    printf("\n\n");
 
    return 0;
}
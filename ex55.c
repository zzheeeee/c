/* 함수로 약수 구하기 */

#include <stdio.h>

void func(int a);

int main(void)
{
    for(int i=1; i<=10; i++)
    {
        printf("%d: ",i);
        func(i);
    }
    
    printf("\n");
    
    return 0;
}

void func(int a)
{
    for (int i=1; i<=a; i++)
    {
        if(a%i==0)
        {
            printf("%d ", i);
        }
    }
    printf("\n");
}
#include <stdio.h>

int primeNum(int);

int main()
{
    for (int i=2; i<10; i++)
        primeNum(i);
}

int primeNum(int n)
{
    for (int i=2;i<n; i++)
    {
        if(n%i==0)
        {
            printf("\n%d : 소수 아님 ", n);
            return 0;
        }
    }
    printf("\n%d : 소수임 ", n);
    return 0;
    
}
/* while 사용하여 1부터 100까지 수 중 짝수의 합 구하기 */

#include <stdio.h>

int main(void)
{
    int sum = 0;

    for(int i=1; i<=100; i++)
    {
        if(i%2==0)
        {
            sum = sum + i;
        }
    }

    printf("%d\n\n",sum);
 
    return 0;
}
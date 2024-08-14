/* 
    다음과 같은 형식으로 2단~9단 출력하기
    2*1= 2 3*1= 3 4*1= 4 5*1= 5 6*1= 6 7*1= 7 8*1= 8 9*1= 9  
*/

#include <stdio.h>

int main(void)
{
    for (int i=1; i<10; i++)
    {
        for (int j=2; j<10; j++)
        {
            printf("%d*%d=%-4d", j,i,j*i);
        }
        printf("\n");
    }
    
    printf("\n");
 
    return 0;
}
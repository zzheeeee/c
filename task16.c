/* 1부터 10까지 출력하기 */

#include <stdio.h>

int main(void)
{
    int i;
    
    for(i=0; i<10;)
    {   
        i = i + 1;
        printf("%d ",i);
    }
    
    printf("\n\n");

    return 0;
}
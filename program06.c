/* 삼각형 모양 출력하기 */

#include <stdio.h>

int main()
{
    int len;

    printf("정수 입력 : ");
    scanf("%d", &len);

    for(int i=0; i<len; i++)
    {
        for(int j=0; j<len-i; j++){
            printf(" ");
        }
        for(int k=0; k<(2*i)+1; k++)
        {
            printf("*");
        }
        printf("\n");
    }
    
    return 0;

}

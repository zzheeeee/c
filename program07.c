/* 빈 다이아몬드 모양 출력하기 */

#include <stdio.h>

int num ,row;


int main()
{    
    printf("높이를 입력하세요 :");
    scanf("%d", &num);
    row = num;
    
    for (int i = 1; i <= num; i++)
    {
        for (int j = row-i; j > 0; j--)
        {
            printf("X");
        }
        for (int k = 0; k < i; k++)
        {
            printf(" ");
        }
 
        for (int k = 0; k < i-1; k++)
        {
            printf(" ");
        }
        for (int j = (row - i); j > 0; j--)
        {
            printf("X");
        }
    
        printf("\n");
    }
 
    for (int i = 1; i <= num-1; i++)
    {
        for (int j = 0; j < i; j++)
        {
            printf("X");
        }
        for (int k = (row - i); k > 1; k--)
        {
            printf(" ");
        }
 
        for (int k = i; k < row; k++)
        {
            printf(" ");
        }
        for (int j = 0; j < i; j++)
        {
            printf("X");
        }
 
        printf("\n");
    } 
    return 0;
}

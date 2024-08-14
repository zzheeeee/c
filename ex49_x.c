/* 
    병합 정렬 알고리즘 
    1. 배열 2개에 임의의 요소를 저장한다.
       a[5], b[5], c[10], 범위:1~20
    2. a[0~4] vs b[0][1][2][3][4] -> c에 추가
    3. a 배열 확인 끝나면 b 확인 안한 값 c에 추가
    4. c 출력
*/

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int a[5], b[5], c[10];
    int i, j, k;

    /* 1. 배열 a, b에 임의의 수 저장하기 (범위: 1~20)*/
    printf("정렬 전");
    printf("\na: ");
    for(i=0; i<5; i++)
    {
        a[i] = rand() % 20 + 1;
        printf("%d ", a[i]);
    }
    printf("\nb: ");
    for(j=0; j<5; j++)
    {
        b[j] = rand() % 20 + 1;
        printf("%d ", a[j]);
    }
    
    i=0; j=0; k=0;

    while (i < 4 && j < 4)
    {
        if (a[i] < b[j])
        {
            c[k] = a[i];
            i = i + 1;
        }
        else
        {
            c[k] = b[j];
            j = j + 1;
        }
        k = k + 1;
    }

    if(i == 4)        // a 배열 탐색 끝이면 b 배열 탐색 안한 값 저장
    {
        while (j < 4)
        {
            c[k] = b[j];
            j = j + 1;
            k = k + 1;
        }
    }
    else              // b 배열 탐색 끝이면 a 배열 탐색 안한 값 저장
    {
        while(i < 4)
        {
            c[k] = a[i];
            i = i + 1;
            k = k + 1;            
        }
    }
    
    printf("\n정렬 후");
    for(k=0; k<8; k++)
    {
        printf("\n%d ", c[k]);
    }
    printf("\n\n");

    return 0;
}
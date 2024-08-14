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
#include <string.h>
#include <time.h>

int compare(const void *a, const void *b) {
    if(*(int*)a > *(int*)b) {
        return 1;
    }
    else if(*(int*)a < *(int*)b) {
        return -1;
    }
    else {
        return 0;
    }
}

int main(void)
{
    srand((unsigned)time(NULL));

    int a[5], b[5], c[10];
    int i, j, k, num;

    /* 1. 배열 a, b에 임의의 수 저장하기 (범위: 1~20)*/
    for(k=0; k<5; k++)
    {
        a[k] = rand()%20 + 1;

        for(j=0;j<=k;j++)
        {
            for (int j=0; j<i; j++)
            {
                if(a[k]==a[j])
                    k--;
            }                
        }          
    }
    for(k=0; k<5; k++)
    {
        b[k] = rand()%20 + 1;

        for(j=0;j<=k;j++)
        {
            for (int j=0; j<i; j++)
            {
                if(b[k]==b[j])
                    k--;
            }                
        }          
    }
    
    qsort(a, 5, sizeof(int), compare);
    qsort(b, 5, sizeof(int), compare);

    printf("정렬 전");    
    printf("\na: ");
    for(k=0; k<5; k++)
    {        
        printf("%d ", a[k]);
    }
    printf("\nb: ");
    for(k=0; k<5; k++)
    {        
        printf("%d ", b[k]);
    }

    /* 병합 정렬 수행 */
    i=0; j=0; k=0;

    while (i <= 4 && j <= 4)
    {
        printf("\na[%d] %d vs b[%d] %d ", i, a[i], i, b[i]);
        if (a[i] < b[j])
        {
            c[k] = a[i];
            printf("\nc[%d]에 a[%d]넣기(%d) / c:  %d  ",i, i, a[i], c[i]);
            i = i + 1;
            printf("i+=1 : %d",i);
        }
        else
        {
            c[k] = b[j];
            printf("\nc[%d]에 b[%d]넣기(%d) / c:  %d ",i, i, b[i], c[i]);
            j = j + 1;
            printf("j+=1 : %d",j);
        }
        k = k+1;
    }

    if(i == 5)        // a 배열 탐색 끝이면 b 배열 탐색 안한 값 저장
    {
        while (j < 5)
        {
            c[k] = b[j];
            j = j + 1;
            k = k + 1;
        }
    }
    else              // b 배열 탐색 끝이면 a 배열 탐색 안한 값 저장
    {
        while(i < 5)
        {
            c[k] = a[i];
            i = i + 1;
            k = k + 1;            
        }
    }
    
    printf("\n정렬 후\nc: ");
    for(k=0; k<8; k++)
    {
        printf("%d ", c[k]);
    }
    printf("\n\n");

    return 0;
}
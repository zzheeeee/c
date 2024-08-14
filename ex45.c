/* 
    선형 탐색 알고리즘 
    1. 배열[10] 랜덤 값(1~50) 저장하기 (+중복 제거하기)
    2. key 입력받기(1~50)
    3. 배열 인덱스 0~9까지의 값과 Key 값 비교하기
    4. 일치하면 index출력, 일치하지 않응면 "탐색실패"출력
*/

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int key, find;
    int a[10];

    /* 1. Key 값 입력받기*/
    for(int i=0; i<10; i++)
    {
        a[i] = rand() % 50 + 1; // 1부터 50까지 범위 중 임의의 수를 생성한다.
        
        for (int j=0; j<i; j++)
        {
            if(a[i]==a[j])
                i--;
        }        
    }

    for(int i=0; i<10; i++)
    {
        printf("%d ", a[i]);
    }

    /* 2. Key 값 입력받기*/
    printf("\n\n검색할 값을 입력하세요(1~50) : "); 
    scanf("%d", &key); 
    
    /* 3. 배열 인덱스 0~9와 Key의 값을 비교하기  */
    for(find=0; find<10; find++)
    {
        if (a[find] == key)
        {
            printf("%d에서 탐색 성공!\n\n", find);  
            break;
        }           
    }
    if(find==10)
    {
        printf("탐색 실패!\n\n");
    }    
 
    return 0;
}
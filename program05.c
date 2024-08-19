/* 사용자가 도형 모양을 숫자로 선택하고, 해당 번호 도형을 출력 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
 
void drawStar1(int, int);
void drawStar2(int, int);
void drawStar3(int, int);
 
int main()
{
    int len, shapeIdx;

    while(true)
    {
        printf("\nShape : 1 : ◣  2 : ◢  3 : ◤  4 : ◥   5 : ▲  6 : ▼  7 : ♦️");
        printf("\n출력할 도형 모양 : ");
        scanf("%d", &shapeIdx);

        while(getchar()!='\n');
        printf("길이 : ");
        scanf("%d", &len);
        
        if (1 <= shapeIdx && shapeIdx <= 4)
        {
            drawStar1(len, shapeIdx);
            //break;
            continue;
        }
        else if (5 <= shapeIdx && shapeIdx <= 6)
        {
            drawStar2(len, shapeIdx);
            //break;
            continue;
        }
        else if (shapeIdx == 7)
        {
            drawStar3(len, shapeIdx);
            //break;
            continue;
        }
        else
        {
            //printf("1에서 7사이의 숫자를 입력하세요.");
            //continue;
            break;
        }
    }
    return 0;
}
 
void drawStar1(int len, int shape)
{
    char arr[len][len];
    int l = len-1;
    
    /* 초기화 */
    for(int i=0; i<len; i++)
    {
        for(int j=0; j<len; j++)
        {
            arr[i][j] = ' ';
        }
    }
    
    /* 별찍기 */
    switch(shape)
    {
        case 1:
            for(int i=0; i<len; i++)
            {
                for(int j=0; j<i+1; j++)
                {
                    arr[i][j] = '*';
                }
            }
            break;
        case 2:
            for(int i=0; i<len; i++)
            {
                for(int j=abs(l-i); j<len; j++)
                {
                    arr[i][j] = '*';
                }
            }
            break;
        case 3:
            for(int i=0; i<len; i++)
            {
                for(int j=0; j<=abs(l-i); j++)
                {
                    arr[i][j] = '*';
                }
            }
            break;
        case 4:
            for(int i=0; i<len; i++)
            {
                for(int j=i; j<len; j++)
                {
                    arr[i][j] = '*';
                }
            }
            break;
    }
    
    /* 출력 */
    printf("\n");
    
    for(int i=0; i<len; i++)
    {
        for(int j=0; j<len; j++)
        {
            printf("%c",arr[i][j]);
        }
        printf("\n");
    }
}
 
void drawStar2(int len, int shape)
{
    int l = (len*2)-1;
    int start, end, cnt;
    char arr[len][l];
    
    /* 초기화 */
    for(int i=0; i<len; i++)
    {
        for(int j=0; j<l; j++)
        {
            arr[i][j] = ' ';
        }
    }
    
    /* 별찍기 */
    switch(shape)
    {
        case 5:
            for(int i=0; i<len; i++)
            {
                start = (l/2)-i;
                end = (l/2)+i;
                cnt = start;
                while(cnt<=end)
                {
                    arr[i][cnt] = '*';
                    cnt++;
                }
            }
            break;
        case 6:
            for(int i=0; i<len; i++)
            {
                start = i;
                end = l-i-1;
                cnt = start;
                while(cnt<=end)
                {
                    arr[i][cnt] = '*';
                    cnt++;
                }
            }
            break;
    }
    
    /* 출력 */
    printf("\n");
    
    for(int i=0; i<len; i++)
    {
        for(int j=0; j<l; j++)
        {
            printf("%c",arr[i][j]);

            
        }
        printf("\n");
    }
}
 
void drawStar3(int len, int shape)
{
    int l = (len*2)-1;
    int ll = (int)(l/2)/2;
    int offset = (len%2==0);
    int start, end, cnt;
    char arr[len][l];
    
    /* 초기화 */
    for(int i=0; i<len; i++)
    {
        for(int j=0; j<l; j++)
        {
            arr[i][j] = ' ';
        }
    }
    
    /* 별찍기 */
    switch(shape)
    {
        case 7:
            for(int i=0; i<len; i++)
            {
                if(i<(len/2)-offset)
                {
                    start = ll-i;
                    end = ll+i;
                }else if(i>(len/2)){
                    start = start+1;
                    end = end-1;
                }else{
                    start = ll-((len/2)-offset);
                    end = ll+((len/2)-offset);
                }
                cnt = start;
                while(cnt<=end)
                {
                    arr[i][cnt] = '*';
                    cnt++;
                }
            }
            break;
    }
    
    /* 출력 */
    printf("\n");
    
    for(int i=0; i<len; i++)
    {
        for(int j=0; j<l; j++)
        {
            printf("%c",arr[i][j]);
        }
        printf("\n");
    }
}

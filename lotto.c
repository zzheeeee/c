#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* 글씨 색상 */
#define COLOR_BLK "\x1B[0m" // Set all colors back to normal.
#define COLOR_MGT "\x1B[35m"  // Magenta
#define COLOR_RED "\x1B[31m"  // Red
 
#define DAY_OF_WEEK_SIZE 7
const static char *DAY_OF_WEEK[DAY_OF_WEEK_SIZE] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
 
// struct tm *localtime(const time_t *timeval);
 
// struct tm {
//         int tm_sec;   //초
//         int tm_min;   //분
//         int tm_hour;  //시
//         int tm_mday;  //일
//         int tm_mon;   //월(0부터 시작)
//         int tm_year;  //1900년부터 흐른 년
//         int tm_wday;  //요일(0부터 일요일)
//         int tm_yday;  //현재 년부터 흐른 일수
//         int tm_isdst;
// };

bool pay(int);
int game(int, int);
void getPrint(int);
int compare(const void *a, const void *b);

int main(void)
{
    srand((unsigned)time(NULL));

    int game[5][6] = {0};
    bool sale = false; 
    bool pass;
    int gameCnt, select, half, halfCnt, manual, manualCnt;
    
    printf("\n ─ ─ ─ ─ ─ Lotto 구매 ─ ─ ─ ─ ─");
    
    // 1. 구매하기
    while(true)
    {
        printf("\nSystem: 몇 개를 구매하시겠습니까? \n>> 입력 : ");
        scanf("%d", &gameCnt);

        if (1>gameCnt || gameCnt>5)
        {
            printf("\nSystem: 최소 1개에서 최대 5개까지 구매 가능합니다. 다시 입력해주세요.");
            continue;
        }
        else
        {
            printf("\nSystem: 금액은 %d원 입니다.",(gameCnt*1000));
            break;
        } 
    } 

    // 2. 결제
    sale = pay(gameCnt);

    // 3. 자동, 반자동, 수동 선택
    while(sale)
    {
        printf("\nSystem: 1. 자동   2. 반자동   3. 수동 \n>> 입력 : ");
        scanf("%d",&select);

        if(0<select<4)
        {
            switch (select)
            {
            case 1:
                //printf("자동선택\n");
                for(int i=0; i<gameCnt; i++)
                {
                    for(int j=0; j<6; j++)
                    {
                        game[i][j] = rand() % 45 + 1; 
                        for (int k=0; k<j; k++)
                        {
                            if(game[i][j]==game[i][k])
                                j--;
                        }       
                    }
                    int sortArray[6] = { game[i][0], game[i][1], game[i][2], game[i][3], game[i][4], game[i][5]};
                    qsort(sortArray, 6, sizeof(int), compare);
                    for(int srt=0; srt<6; srt++)
                    {
                        game[i][srt]=sortArray[srt];
                    }
                }
                break;
            case 2:
                //printf("반자동선택\n");
                printf("\n%sSystem: 1에서 45사이의 수를 입력하세요.%s",COLOR_RED,COLOR_BLK);
                printf("\n%sSystem: 0을 입력하면 나머지는 자동으로 입력됩니다.%s",COLOR_RED,COLOR_BLK);
                halfCnt = 0;
                for (int i=0; i<gameCnt; i++)
                {
                    printf("\n *** %d 게임 구매중 ***", i+1);
                    halfCnt=0;
                    while(halfCnt<6)
                    {
                        pass=true;
                        printf("\n%d번째 숫자 입력: ",halfCnt+1);
                        scanf("%d", &half);
                        
                        if(half == 0)
                        {
                            for(halfCnt; halfCnt<6; halfCnt++)
                            {
                                game[i][halfCnt] = rand() % 45 + 1; 
                                for (int k=0; k<halfCnt; k++)
                                {
                                    if(game[i][halfCnt]==game[i][k])
                                        halfCnt--;
                                        break;
                                }       
                            }
                        }
                        else if(half<0 || half>45)
                        {
                            printf("\n%sSystem: 1에서 45사이의 수를 입력하세요.%s",COLOR_RED,COLOR_BLK);
                            while(getchar()!='\n');
                            continue;
                        }
                        else 
                        {
                            for(int j=0; j<halfCnt; j++)
                            {
                                if(game[i][j]==half)
                                {
                                    pass = false;
                                    break;
                                }
                            }
                            if (pass)
                            {
                                game[i][halfCnt] = half;
                                halfCnt++;
                            }
                            else
                            {
                                printf("\n%sSystem: 중복값입니다.%s",COLOR_RED,COLOR_BLK);
                                continue;
                            }
                        }
                    }
                    int sortArray[6] = { game[i][0], game[i][1], game[i][2], game[i][3], game[i][4], game[i][5]};
                    qsort(sortArray, 6, sizeof(int), compare);
                    for(int srt=0; srt<6; srt++)
                    {
                        game[i][srt]=sortArray[srt];
                    }
                }
                break;
            case 3:
                //printf("수동선택\n");
                printf("\n%sSystem: 1에서 45사이의 수를 입력하세요.%s",COLOR_RED,COLOR_BLK);
                manual = 0;
                for (int i=0; i<gameCnt; i++)
                {
                    printf("\n *** %d 게임 구매중 ***", i+1);
                    manualCnt=0;
                    while(manualCnt<6)
                    {
                        pass=true;
                        printf("\n%d번째 숫자 입력: ",manualCnt+1);
                        scanf("%d", &manual);
                        
                        if(manual<0 || manual>45)
                        {
                            printf("\n%sSystem: 1에서 45사이의 수를 입력하세요.%s",COLOR_RED,COLOR_BLK);
                            while(getchar()!='\n');
                            continue;
                        }
                        else if(manual>0 && manual<46)
                        {
                            for(int j=0; j<manualCnt; j++)
                            {
                                if(game[i][j]==manual)
                                {
                                    pass = false;
                                    break;
                                }
                            }
                            if (pass)
                            {
                                game[i][manualCnt] = manual;
                                manualCnt++;
                            }
                            else
                            {
                                printf("\n%sSystem: 중복값입니다.%s",COLOR_RED,COLOR_BLK);
                                continue;
                            }
                        }
                    }
                    int sortArray[6] = { game[i][0], game[i][1], game[i][2], game[i][3], game[i][4], game[i][5]};
                    qsort(sortArray, 6, sizeof(int), compare);
                    for(int srt=0; srt<6; srt++)
                    {
                        game[i][srt]=sortArray[srt];
                    }
                }
                break;
            default:
                printf("\n%sSystem: 다시 입력하세요.%s",COLOR_RED,COLOR_BLK);
                while(getchar()!='\n');
                continue;
            }
            sale=false;
        }
        else
        {
            printf("\n%sSystem: 다시 입력하세요.%s",COLOR_RED,COLOR_BLK);
            while(getchar()!='\n');
            continue;
        }
    }
    
    /* 출력 */
    printf("\nSystem: 당첨 되시길 바랍니다! *^^* \n\n ");
    getPrint(0); // title
    getPrint(1); // time
    for(int i=0; i<gameCnt; i++)
    {
        
        printf("%s    | %s", COLOR_MGT, COLOR_BLK);
        for(int j=0; j<6; j++)
        {
            printf("%2d", game[i][j]);
            printf("%s | %s", COLOR_MGT, COLOR_BLK);
        }
        printf("\n");
    }
    
        getPrint((gameCnt*1000));
    
    return 0;
}

void getPrint(int section)
{
    /* 시간 */
    time_t current;
    time(&current);
    struct tm *t = localtime(&current);
    
    switch(section)
    {
        case 0:    
            printf("          💰 Lotto 6/45 💰 \n");
            break;
        case 1:
            printf(COLOR_MGT);
            printf("   발행일: %d/%02d/%02d(%s) %02d:%02d:%02d\n", t->tm_year + 1900,t->tm_mon + 1,t->tm_mday,DAY_OF_WEEK[t->tm_wday],t->tm_hour,t->tm_min,t->tm_sec);
            printf(COLOR_BLK);
            break;
        default:
            printf("\n   --------------------------------");
            printf("\n                             %d원",section);
        
    }
}

/* 로또 구입 */
bool pay(int cnt)
{   
    int money;
    
    while(true)
    { // 돈 받기 while 시작
        printf(" \n>> 돈 : ");
        scanf("%d",&money);

        if(money>=(cnt*1000))
        {
            if(money-(cnt*1000) == 0)
            {
                printf("\nSystem: 구매가 완료되었습니다.");
                return true;
            }
            else
            {
                printf("\nSystem: 잔돈은 %d원입니다. ",(money-(cnt*1000)));
                return true;
            }    
        }
        else 
        {
            printf("\nSystem: 다시 입력하세요.");
            while(getchar()!='\n');
            money = 0;
            continue;
        }
    } // 구매 종료
}

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



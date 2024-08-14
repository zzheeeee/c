#include <stdio.h>
#include <time.h>
#include <stdbool.h>

/* 글씨 색상 */
#define RESETTEXT "\x1B[0m" // Set all colors back to normal.
#define FOREMAG "\x1B[35m"  // Magenta
#define FORERED "\x1B[31m"  // Red
 
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
void getTime();

int main()
{
    bool sale = false;
    int gameCnt, select;

    printf("\n ─ ─ ─ ─ ─ Lotto 구매 ─ ─ ─ ─ ─");
    
    // 1. 구매하기
    while(true)
    {
        printf("\nSystem: 몇 개를 구매하시겠습니까? \n 입력 >> ");
        scanf("%d", &gameCnt);

        if (1>gameCnt || gameCnt>5)
        {
            printf("\nSystem: 최소 1개에서 최대 5개까지 구매 가능합니다. 다시 입력해주세요.");
            continue;
        }
        else
        {
            printf("판매 금액은 %d원 입니다.",(gameCnt*1000));
            break;
        } 
    } 

    // 2. 결제
    sale = pay(gameCnt);

    // 3. 자동, 반자동, 수동 선택
    while(sale)
    {
        printf("\nSystem: 1. 자동   2. 반자동   3. 수동\n 입력 >> ");
        scanf("%d",&select);

        if(0<select<4)
        {
            game(gameCnt,select);
            sale=false;
        }
        else
        {
            printf("\nSystem: 다시 입력하세요.");
            while(getchar()!='\n');
            continue;
        }
    }



    return 0;
}

void getTime()
{
    /* 시간 */
    time_t current;
    time(&current);
    struct tm *t = localtime(&current);
    
    /* 요일 */
    //printf("\n\n1.\n");
    switch (t->tm_wday)
    {
    case 0:
        printf("일요일 ");
        break;
    case 1:
        printf("월요일 ");
        break;
    case 2:
        //printf("%s", DAY_OF_WEEK[t->tm_wday]);
        break;
    case 3:
        printf("수요일 ");
        break;
    case 4:
        printf("목요일 ");
        break;
    case 5:
        printf("금요일 ");
        break;
    case 6:
        printf("토요일 ");
        break;
    }
    printf("\n        💰 Lotto 6/45 💰");
    
    /* 발행일 시작 */
    printf(FOREMAG); // 지금부터 마젠타 색으로 출력
    printf("\n");
    printf("발행일: ");
    /* 날짜 */
    printf("%d", t->tm_year + 1900);    printf("/");    printf("%02d", t->tm_mon + 1);
    printf("/");    printf("%02d", t->tm_mday);
    /* 요일 */
    printf("(");    printf("%s", DAY_OF_WEEK[t->tm_wday]);    printf(") ");
    /* 시간 */
    printf("%02d", t->tm_hour);    printf(":");    printf("%02d", t->tm_min);    printf(":");    printf("%02d", t->tm_sec);
    printf("\n");      // 발행일 끝
    printf(RESETTEXT); // 다시 일반 색으로 출력
}

/* 로또 구입 */
bool pay(int cnt)
{   
    int money;
    
    while(true)
    { // 돈 받기 while 시작
        printf("\n>> 돈 : ");
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
                printf("\nSystem: 잔돈은 %d원입니다.",(money-(cnt*1000)));
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

int game(int cnt, int num)
{
    int game[5][6];
    switch (num)
    {
    case 1:
        printf("자동선택\n");
        break;
    case 2:
        printf("반자동선택\n");
        break;
    case 3:
        printf("수동선택\n");
        break;
    }
}
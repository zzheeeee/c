/* 문자열을 읽고 정렬한다 */

#include <stdio.h>
#include <string.h>
#define SIZE 81        /* 문자열 길이 제한, \0을 포함한다 */
#define LIM 20         /* 읽을 최대 라인 개수 */
#define HALT ""        /* 입력을 멈추는 널 문자열 */
void stsrt(char *strings[], int num); /* 문자열 정렬 함수 */
char * s_gets(char * st, int n);

int main(void)
{
    char input[LIM][SIZE];     /* 입력을 저장할 배열 */
    char *ptstr[LIM];          /* 포인터 변수들의 배열 */
    int ct = 0;                /* 입력 카운트 */
    int k;                     /* 출력 카운트 */
    
    printf("최대 %d라인까지 입력하면 그것들을 정렬하겠습니다.\n",LIM);
    printf("입력을 멈추려면 라인 시작에서 Enter 키를 누르시오.\n");
    while (ct < LIM && s_gets(input[ct], SIZE) != NULL
           && input[ct][0] != '\0')
    {
        ptstr[ct] = input[ct];  /* 문자열들을 ptrs로 설정한다 */
        ct++;
    }
    stsrt(ptstr, ct);           /* 문자열 정렬자 */
    puts("\n문자열들이 정렬된 결과 : \n");
    for (k = 0; k < ct; k++)
        puts(ptstr[k]) ;       /* 정렬된 포인터들 */
    
    return 0;
}

/* 문자열-포인터-정렬 함수 */
void stsrt(char *strings[], int num)
{
    char *temp;
    int top, seek;
    
    for (top = 0; top < num-1; top++)
        for (seek = top + 1; seek < num; seek++)
            if (strcmp(strings[top],strings[seek]) > 0)
            {
                temp = strings[top];
                strings[top] = strings[seek];
                strings[seek] = temp;
            }
}

char * s_gets(char * st, int n)
{
    char * ret_val;
    int i = 0;
    
    ret_val = fgets(st, n, stdin);
    if (ret_val)
    {
        while (st[i] != '\n' && st[i] != '\0')
            i++;
        if (st[i] == '\n')
            st[i] = '\0';
        else // '\0'이어야 한다
            while (getchar() != '\n')
                continue;
    }
    return ret_val;
}


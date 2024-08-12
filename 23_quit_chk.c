/* 어떤 프로그램의 시작 부분 */

#include <stdio.h>
#include <string.h>

#define SIZE 80
#define LIM 10
#define STOP "quit"

char * s_gets(char * st, int n);

int main(void)
{
    char input[LIM][SIZE];
    int ct = 0;
    
    printf("%d라인까지 입력하십시오 (끝내려면 quit):\n", LIM);
    while (ct < LIM && s_gets(input[ct], SIZE) != NULL &&
           strcmp(input[ct],STOP) != 0)
    {
        ct++;
    }
    printf("%d개의 문자열이 입력되었습니다.\n", ct);
    
    return 0;
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
        else // 반드시 '\0'이어야 한다.
            while (getchar() != '\n')
                continue;
    }
    return ret_val;
}

/* 문자열을 포맷 지정한다 */

#include <stdio.h>
#define MAX 20

char * s_gets(char * st, int n);

int main(void)
{
    char first[MAX];
    char last[MAX];
    char formal[2 * MAX + 10];
    double prize;
    
    puts("이름을 입력하세요 : ");
    s_gets(first, MAX);
    puts("성을 입력하세요 : ");
    s_gets(last, MAX);
    puts("상금을 입력하세요 : ");
    scanf("%lf", &prize);
    sprintf(formal, "%s, %-19s: $%6.2f\n", last, first, prize);
    puts(formal);
    
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
        else // 반드시 '\0'이어야 한다
            while (getchar() != '\n')
                continue;
    }
    return ret_val;
}

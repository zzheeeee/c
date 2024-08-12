/* 20 수정 버전, 바르게 동작함! */

#include <stdio.h>
#include <string.h>   // strcmp()를 선언한다

#define ANSWER "Grant"
#define SIZE 40
char * s_gets(char * st, int n);

int main(void)
{
    char try[SIZE];
    
    puts("Grant의 무덤에 누가 잠들어 있습니까?");
    s_gets(try, SIZE);
    while (strcmp(try,ANSWER) != 0)
    {
        puts("틀렸습니다. 다시 말해 보세요.");
        s_gets(try, SIZE);
    }
    puts("정답입니다!");
        
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
        else // must have words[i] == '\0'
            while (getchar() != '\n')
                continue;
    }
    return ret_val;
}

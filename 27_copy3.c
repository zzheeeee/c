/* strncpy()를 사용한다 */

#include <stdio.h>
#include <string.h>  /* strncpy()를 선언한다 */

#define SIZE 40
#define TARGSIZE 7
#define LIM 5

char * s_gets(char * st, int n);

int main(void)
{
    char qwords[LIM][TARGSIZE];
    char temp[SIZE];
    int i = 0;
    
    printf("q로 시작하는 단어를 %d개 입력하세요 : \n", LIM);
    while (i < LIM && s_gets(temp, SIZE))
    {
        if (temp[0] != 'q')
            printf("%s : q로 시작하는 단어가 아닙니다!\n", temp);
        else
        {
            strncpy(qwords[i], temp, TARGSIZE - 1);
            qwords[i][TARGSIZE - 1] = '\0';
            i++;
        }
    }
    puts("받아들인 단어들은 다음과 같습니다 : ");
    for (i = 0; i < LIM; i++)
        puts(qwords[i]);
    
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
        else // '\0'이어야 한다
            while (getchar() != '\n')
                continue;
    }
    return ret_val;
}

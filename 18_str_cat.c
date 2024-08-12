/* strcat() -- 두 문자를 결합한다. */

#include <stdio.h>
#include <string.h>  /* strcat() 함수를 선언한다 */

#define SIZE 80

char * s_gets(char * st, int n);

int main(void)
{
    char flower[SIZE];
    char addon[] =  "s smell like heaven.";
    
    puts("네가 가장 좋아하는 꽃이 뭐야?");
    if (s_gets(flower, SIZE))
    {
        strcat(flower, addon);
        puts(flower);
        puts(addon);
    }
    else
        puts("파일 끝 EOF!");
    puts("빠잉-*");


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

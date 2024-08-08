/* 문자에 대응하는 코드값을 표시하는 프로그램 */

#include <stdio.h>

int main(void)
{
    char ch;

    printf("영문자 하나를 입력하세요.\n");
    scanf("%c", &ch);   /* 사용자가 문자를 입력한다. */
    printf("문자 %c에 대응하는 코드 값은 %d입니다.\n", ch, ch);
    
    return 0;
}

/*

이 프로그램에서 공부할 수 있는 것
1. [Line 13] 
2. [Line 13] 
3. [Line 13] 
4. [Line 17] 

*/


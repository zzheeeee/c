/* 이스케이프 문자를 사용한다 */
#include <stdio.h>
int main(void)
{
    float salary;
    
    printf("\a원하는 월급 액수를 입력하세요:");/* 1 */
    printf(" $_______\b\b\b\b\b\b\b");             /* 2 */
    scanf("%f", &salary);
    printf("\n\t월 $%.2f이면 연봉으로 $%.2f입니다.", salary,
           salary * 12.0);                         /* 3 */
    printf("\r우왕!\n");                            /* 4 */
    
    return 0;
}

/*

이 프로그램에서 공부할 수 있는 것
1. [Line 13] 
2. [Line 13] 
3. [Line 13] 
4. [Line 17] 

*/

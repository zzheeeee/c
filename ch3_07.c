/* 
    xxxxxxxxxxxxxxx
    float형 값을 두 가지 표기로 출력한다. 
*/

#include <stdio.h>

int main(void)
{
    float aboat = 32000.0;
    double abet = 2.14e9;
    long double dip = 5.32e-5;
    
    printf("%f은 %e로 표현할 수 있다.\n", aboat, aboat);
    
    // 다음 라인은 C99 이상의 표준을 따를 것을 요구한다.
    printf("그것은 16진수 2의 거듭제곱으로 %a이다.\n", aboat);
    printf("%f은 %e로 표현할 수 있다.\n", abet, abet);
    printf("%Lf은 %Le로 표현할 수 있다.\n", dip, dip);
    
    return 0;
}

/*

이 프로그램에서 공부할 수 있는 것
1. [Line 13] 
2. [Line 13] 
3. [Line 13] 
4. [Line 17] 

*/


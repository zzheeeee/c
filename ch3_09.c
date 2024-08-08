/* 잘못된 전달인자 사용의 예 */

#include <stdio.h>

int main(void)
{
    int n = 4;
    int m = 5;
    float f = 7.0f;
    float g = 8.0f;
    
    // 오답
    // printf("%d\n", n, m);    /* 전달인자가 너무 많다  */
    // printf("%d %d %d\n", n); /* 전달인자가 너무 적다 */
    // printf("%d %d\n", f, g); /* 데이터형이 일치하지 않는다. */

    // 정답
    printf("%d\n", n);   
    printf("%d\n", m);   
    printf("%d, %d\n", n, m);
    printf("%d %d %d\n", n, m, n+m);
    printf("%f %f\n", f, g);  
    
    return 0;
}

/*

이 프로그램에서 공부할 수 있는 것
1. [Line 13] 
2. [Line 13] 
3. [Line 13] 
4. [Line 17] 

*/

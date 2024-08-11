/* 1부터 20까지 정수 합을 구한다 */

#include <stdio.h>

int main(void)                
{
    int count, sum;             // 선언문
    
    count = 0;                  // 대입문
    sum = 0;                    // 대입문
    while (count++ < 20)        // while 
        sum = sum + count;      // 문장(while문)
    printf("sum = %d\n", sum);  // 함수문 
    
    return 0;                   // return문
}

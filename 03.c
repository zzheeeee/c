/* 100을 10진수, 8진수, 16진수 표기로 출력한다 */

#include <stdio.h>

int main(void)
{
    int x = 100;
    
    printf("10진수 = %d; 8진수 = %o; 16진수 = %x\n", x, x, x);
    printf("10진수 = %d; 8진수 = %#o; 16진수 = %#x\n", x, x, x);
    
    return 0;
}

/*

이 프로그램에서 공부할 수 있는 것
1. 동일한 값이 세가지 서로 다른 수 체계로 출력된다.
2. printf()함수가 이러한 변환을 처리한다.
3. 포맷 지정자의 일부로 #을 넣지 않으면 접두사 0과 0x가 표시되지 않는다.
4. 반대로 접두사 0과 0x를 표시하고 싶으면? 포맷 지정자의 일부로 #을 넣어주면된다는 뜻

*/


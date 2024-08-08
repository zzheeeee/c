/* print2. printf()의 추가적인 특성을 설명한다 */

#include <stdio.h>

int main(void)
{
    unsigned int un = 3000000000; // 32bit int형과
    short end = 200;              // 16비트 short형을 사용하는 시스템
    long big = 65537;
    long long verybig = 12345678908642;
    
    printf("un = %u and not %d\n", un, un);
    printf("end = %hd and %d\n", end, end);
    
    printf("big = %ld and not %ld\n", big, big); // 정답
    printf("verybig= %lld and not %lld\n", verybig, verybig); // 정답

    // printf("big = %ld and not %hd\n", big, big); // 오답
    // printf("verybig= %lld and not %ld\n", verybig, verybig); // 오답
    
    return 0;
}

/*

이 프로그램에서 공부할 수 있는 것
오류 1. [Line 14] long int에 %hd 지정자를 사용했더니 컴파일 오류가 발생한다.
오류 2. [Line 14] %hd는 int 지정자 // long int 지정자는 %ld 이다.
오류 3. [Line 15] long long int에 %ld 지정자를 사용했더니 컴파일 오류가 발생한다.
오류 4. [Line 15] %ld는 long int 지정자 // long long int 지정자는 %lld 이다.

정답 1. [Line 7, 12] 부호 없는 변수 un에 %d 지정자를 사용했더니 음수가 출력되었다.
정답 1. [Line 7, 12] 부호 있는 최대값보다 더 큰수를 처리할 때 발생한다.
정답 1. [Line 7, 12] 96과 같이 작은 양수의 경우에는 부호 있는 데이터형과 부호 없는 데이터형이 둘 다 동일하게 저장되고 출력된다.
정답 2. [Line 8, 13] short형(%hd 지정자)으로 지정하든 int형(%d 지정자)형으로 지정하든 간에 short형 변수 end는 동일하게 출력된다.
정답 2. [Line 8, 13] 이유는 c가 short형 값을 함수에 전달인자로 전달할 때 자동으로 int형으로 확장하기 때문이다.
정답 2. [Line 8, 13] 컴퓨터가 가장 효율적으로 처리할 수 있는 정수 크기가 int형이기 때문이다.
정답 2. [Line 8, 13] short형과 int형의 크기가 서로 다른 컴퓨터에서 int형으로 값을 전달하는 것이 더 빠를 수 있다.
정답 2. [Line 8, 13] 
*/


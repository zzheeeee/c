//* limits.h와 float.h에 정의되어 있는 기호 상수들을 사용한다. */
#include <stdio.h>
#include <limits.h>    // integer limits
#include <float.h>     // floating-point limits
int main(void)
{
    printf("이 시스템이 표현하는 수의 한계:\n");
    printf("int형 최댓값: %d\n", INT_MAX);
    printf("long long형 최솟값: %lld\n", LLONG_MIN);
    printf("이 시스템에서 1 byte는 %d bit이다.\n", CHAR_BIT);
    printf("double형 최댓값: %e\n", DBL_MAX);
    printf("float형 최솟값: %e\n", FLT_MIN);
    printf("float형 정밀도는 소수점 아래 %d자리까지\n", FLT_DIG);
    printf("float형 epsilon = %e\n", FLT_EPSILON);
    
    return 0;
}

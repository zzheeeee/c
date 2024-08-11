/* 사이즈 단위를 인치 단위로 변환하기 */

#include <stdio.h>
#define ADJUST 7.31              // 기호 상수의 한 종류

int main(void)
{
    const double SCALE = 0.333;  // 기호 상수의 다른 종류
    double shoe, foot;
    
    shoe = 9.0;
    foot = SCALE * shoe + ADJUST;
    printf("신발 사이즈(남성용)    발길이\n");
    printf("%10.1f %15.2f 인치\n", shoe, foot);
    
    return 0;
}

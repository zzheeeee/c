/* 여러 개의 신발 사이즈를 인치 단위로 계산한다 */

#include <stdio.h>
#define ADJUST 7.31              // 기호 상수의 다른 종류

int main(void)
{
    const double SCALE = 0.333;  // 기호 상수의 다른 종류
    double shoe, foot;
    
    printf("신발 사이즈(남성용)    발길이\n");
    shoe = 3.0;
    while (shoe < 18.5)      /* while 루프 시작 */
    {                        /* 블록의 시작     */
        foot = SCALE * shoe + ADJUST;
        printf("%10.1f %15.2f 인치\n", shoe, foot); 
        shoe = shoe + 1.0;
    }                        /* 블록의 끝       */
    printf("그 신발이 발에 맞는다면, 그것을 신으세요.\n");
    
    return 0;
}

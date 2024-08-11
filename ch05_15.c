/* 하나의 전달인자를 사용하는 함수 */

#include <stdio.h>

void pound(int n);   // 구현은 안해도 선언은 해줘

int main(void)
{
    int times = 5;
    char ch = '!';   // !의 아스키 코드 값은 33
    float f = 6.0f;
    
    // int형 전달인자
    pound(times);    // 이미 int형
    pound(ch);       // pound((int)ch); 형변환 되어 전달인자 입력
    pound(f);        // pound((int)f); 형변환 되어 전달인자 입력
    
    return 0;
}

void pound(int n)    // int형 전달인자를 한 개 사용하겠다!
{                    
    while (n-- > 0)
        printf("#");
    printf("\n");
}

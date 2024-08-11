/* 초 수로 주어지는 시간을 분과 초로 변경한다 */

#include <stdio.h>
#define SEC_PER_MIN 60            // 1분은 60초

int main(void)
{
    int sec, min, left;
    
    printf("초 단위 시간을 분과 초로 변환해줄게~~\n");
    printf("초 수를 입력하세요(0을 입력하면 종료):\n");
    scanf("%d", &sec);           

    while (sec > 0)
    {
        min = sec / SEC_PER_MIN;  // 나머지를 버린 분
        left = sec % SEC_PER_MIN; // 나머지 초 
        
        printf("%d초는 %d 분 %d 초 입니다.\n", sec, min, left);
        printf("다음 초 수를 입력하세요(0을 입력하면 종료):\n");
        scanf("%d", &sec);
    }
    printf("! 종료 !\n");
    
    return 0;
}

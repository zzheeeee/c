/* 지수적 증가 */

#include <stdio.h>
#define SQUARES 64             // 네모칸 수

int main(void)
{
    const double CROP = 2E16;  // 세계 밀 생산량
    double current, total;
    int count = 1;
    
    printf(" 네모칸     추가         누계     ");
    printf("세계 수확량과의 \n");
    printf(" 번  호    낱알 수      낱알 수      ");
    printf("상대 비율\n");
    total = current = 1.0; /* start with one grain   */
    printf("%4d %13.2e %12.2e %14.2e\n", count, current, total, total/CROP);

    while (count < SQUARES)
    {
        count = count + 1;
        current = 2.0 * current;
        /* double grains on next square */
        total = total + current;     /* update total */
        printf("%4d %13.2e %12.2e %14.2e\n", count, current, total, total/CROP);
    }
    printf("-끝-\n");
    
    return 0;
}

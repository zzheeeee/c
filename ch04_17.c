/* 입력에서 처음 2개의 정수를 건너뛴다 */

#include <stdio.h>

int main(void)
{
    int n;
    
    printf("3개의 정수를 입력하세요:\n");
    scanf("%*d %*d %d", &n);
    printf("마지막으로 입력한 정수는 %d입니다.\n", n);
    
    return 0;
}

/* 자동 데이터형 변환 */

#include <stdio.h>

int main(void)
{
    char ch;
    int i;
    float fl;
    
    fl = i = ch = 'C';
    printf("ch = %c, i = %d, fl = %2.2f\n", ch, i, fl);
    
    ch = ch + 1;
    i = fl + 2 * ch;
    fl = 2.0 * ch + i;
    printf("ch = %c, i = %d, fl = %2.2f\n", ch, i, fl);
    
    int N = 1107;
    ch = N;
    // ch = 1107;
    printf("Now ch = %c\n", ch);
    // printf("%d\n",(int)ch); // 출력 : 83
    
    ch = 80.89;
    printf("Now ch = %c\n", ch);
    
    return 0;
}

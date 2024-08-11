// sizeof 연산자를 사용한다
// C99의 %z 변경자를 사용한다 // 지원하지 않으면 %u %lu 사용하기

#include <stdio.h>

int main(void)
{
    int n = 0;
    size_t intsize;
    
    intsize = sizeof (int);
    printf("n = %d, n은 %zd바이트다; 모든 int형 값은 %zd바이트다.\n",
           n, sizeof n, intsize );
    
    return 0;
}

//* 데이터형들의 크기를 출력한다. */

#include <stdio.h>

int main(void)
{
    /* c99는 크기를 위해 %zd 포맷 지정자를 제공한다. */
    printf("int형의 크기: %zd bytes\n", sizeof(int));
    printf("char형의 크기: %zd bytes\n", sizeof(char));
    printf("long형의 크기: %zd bytes\n",sizeof(long));
    printf("long long형의 크기: %zd bytes\n",
           sizeof(long long));
    printf("double형의 크기: %zd bytes\n",
           sizeof(double));
    printf("long double형의 크기: %zd bytes\n",
           sizeof(long double));
    return 0;
}

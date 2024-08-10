/* 몇 가지 유형의 문자열들을 사용한다 */

#include <stdio.h>
#define PRAISE "넌 특별한 존재야."

int main(void)
{
    char name[40];
    
    printf("너 이름이 뭐니?\n");
    scanf("%s", name);
    printf("반가워, %s야. %s\n", name, PRAISE);
    
    return 0;
}

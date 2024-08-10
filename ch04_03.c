/* 사용하는 시스템이 %zd를 인식하지 못한다면 %u 또는 %lu를 사용한다. */

#include <stdio.h>
#include <string.h>      /* provides strlen() prototype */
#define PRAISE "넌 특별한 존재야."

int main(void)
{
    char name[40];
    
    printf("너 이름이 뭐니?\n");
    scanf("%s", name);
    printf("반가워, %s야. %s\n", name, PRAISE);
    printf("이름은 %zd글자인데, 메모리 셀 %zd개를 차지합니다.\n", strlen(name), sizeof name);
    printf("감탄문은 %zd글자인데, ", strlen(PRAISE));
    printf("메모리 셀 %zd개를 차지합니다. \n", sizeof PRAISE);
    
    return 0;
}
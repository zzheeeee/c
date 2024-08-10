/* &를 언제 사용하는가 */ 

#include <stdio.h>

int main(void)
{
    int age;             // 변수
    float assets;        // 변수
    char pet[30];        // 문자열
    
    printf("나이, 재산, 좋아하는 애완동물을 입력하세요.\n");
    scanf("%d %f", &age, &assets); // 여기에는 &를 사용한다.
    scanf("%s", pet);              // 문자 배열에는 &를 사용하지 않는다.
    printf("%d $%.2f %s\n", age, assets, pet);
    
    return 0;
}

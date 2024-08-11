/* 
    전위 모드 : 사용되기 전에 변경한다.
    후위 모드 : 사용된 후에 변경한다.
*/

#include <stdio.h>
int main(void)

{
    int a = 1, b = 1;
    int a_post, pre_b;
    
    a_post = a++;  // value of a++ during assignment phase
    pre_b = ++b;   // value of ++b during assignment phase
    printf("a  a_post   b   pre_b \n");
    printf("%1d %5d %5d %5d\n", a, a_post, b, pre_b);
    
    return 0;
}

/* strcmp()의 리턴값 */

#include <stdio.h>
#include <string.h>

int main(void)
{
    
    printf("strcmp(\"A\", \"A\")은 ");
    printf("%d\n", strcmp("A", "A"));
    
    printf("strcmp(\"A\", \"B\")은 ");
    printf("%d\n", strcmp("A", "B"));
    
    printf("strcmp(\"B\", \"A\")은 ");
    printf("%d\n", strcmp("B", "A"));
    
    printf("strcmp(\"C\", \"A\")은 ");
    printf("%d\n", strcmp("C", "A"));
    
    printf("strcmp(\"Z\", \"a\")은 ");
    printf("%d\n", strcmp("Z", "a"));
    
    printf("strcmp(\"apples\", \"apple\")은 ");
    printf("%d\n", strcmp("apples", "apple"));
    
    return 0;
}

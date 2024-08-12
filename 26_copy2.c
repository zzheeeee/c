/* strcpy()를 사용한다 */

#include <stdio.h>
#include <string.h>    // strcpy()를 선언한다

#define WORDS  "beast"
#define SIZE 40

int main(void)
{
    const char * orig = WORDS;
    char copy[SIZE] = "Be the best that you can be.";
    char * ps;
    
    puts(orig);
    puts(copy);
    ps = strcpy(copy + 7, orig);
    puts(copy);
    puts(ps);
    
    return 0;
}

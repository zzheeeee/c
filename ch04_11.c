/* 
       xxxxx
       잘못 짝지어진 정수형 변환 지정자 
*/

#include <stdio.h>
#define PAGES 336
#define WORDS 65618

int main(void)
{
    short num = PAGES;
    short mnum = -PAGES;
    
    printf("short형, unsigned short형 num:  %hd %hu\n", num, num);
    printf("-num as short and unsigned short: %hd %hu\n", mnum,
           mnum);
    printf("num as int and char: %d %c\n", num, num);
    printf("WORDS as int, short, and char: %d %hd %c\n",
           WORDS, WORDS, WORDS);
    return 0;
}

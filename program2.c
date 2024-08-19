/* 사용자가 사각형의 넓이, 원의 넓이, 삼각형의 넓이, 원의 둘레를 선택하고 자동 계산하기 */

#include<stdio.h>
#include <string.h>

char shp[10];
int width, height, rad, res, status;
double res1;

void square(void);
void circle(void);
void triangle(void);

int main(void)
{
    printf("******************도형의넓이*********************\n");
    printf("구하고 싶은 도형을 입력하시오(사각형, 삼각형, 원)\n");
    status = scanf("%s",shp);
    while (status == 1)
    {
        if(strcmp(shp, "사각형")==0)
        {
            square();
            printf("가로는 %d 세로는 %d 입니다.\n", width, height);
            printf("%s의 넓이는 %d㎠ 입니다.\n", shp, res);
            break;
        }
        else if(strcmp(shp, "원")==0)
        {
            circle();
            printf("반지름은 %d 입니다.\n", rad);
            printf("%s의 넓이는 %.3lf㎠ 입니다.\n", shp, res1);
            break;
        }

        else if(strcmp(shp, "삼각형")==0)
        {
            triangle();
            printf("밑변은 %d 높이는 %d 입니다.\n", width, height);
            printf("%s의 넓이는 %.2lf㎠입니다.\n", shp, res1);
            break;
        }
        else
        {
            printf("\n다시 입력하세요 : ");
            printf("구하고 싶은 도형을 입력하시오(사각형, 삼각형, 원)\n");
            status = scanf("%s",shp);        
        }
    }
    printf("*************************************************\n");

    return 0;
}

void square(void)
{
    printf("\n가로를 입력하세요 : ");
    scanf("%d",&width);
    printf("\n세로를 입력하세요 : ");
    scanf("%d",&height);

    res = width * height;
}

void circle(void)
{
    printf("\n반지름을 입력하세요 :");
    scanf("%d",&rad);
    
    res1 = rad * rad * 3.14;
}
void triangle(void)
{
    printf("\n밑변을 입력하세요 :");
    scanf("%d",&width);
    printf("\n높이를 입력하세요 :");
    scanf("%d",&height);

    res1 = (width * height) / 2;
}

/* 사용자에게 숫자 2개를 입력받고, 사칙연산 기호(+, -, *, /, %, //)를 입력받으면, 그 연산 결과를 출력 */

#include<stdio.h>

int f_add(int,int);
int f_sub(int,int);
int f_mul(int,int);
double f_div(double,double);
int f_rmn(int,int);
int f_quo(int,int);

int main(void)
{
    int i, ans1, ans2, ans3, num1, num2, num3;     

    for(i=0; ;i++){

        printf("첫 번째 수를 입력하세요. : ");
        ans1 = scanf("%d",&num1);
        
        while (getchar() != '\n');
            if (ans1 == 1)		
                break; 
    }

    for(i=0; ;i++){

        printf("두 번째 수를 입력하세요. : ");
        ans2 = scanf("%d",&num2);
        while (getchar() != '\n');
            if (ans2 == 1)		
                break; 
    }

    for(i=0; ;i++){

        printf("연산자를 선택하세요. \" + = 1, - = 2, * = 3, / = 4, %% = 5, // = 6 \" 를 입력하세요. : ");
        scanf("%d",&num3);
        if (1 > num3 || num3 > 6){
            printf("1 ~ 6 사이의 수를 입력해주세요.\n");
            while (getchar() != '\n');
            continue;
        }
        
        else break;
        }


    if(num3 == 1)
        f_add(num1,num2);
    if(num3 == 2)
        f_sub(num1,num2);
    if(num3 == 3)
        f_mul(num1,num2);
    if(num3 == 4)
        f_div(num1,num2);
    if(num3 == 5)
        f_rmn(num1,num2);
    if(num3 == 6)
        f_quo(num1,num2);
}

int f_add(int n1, int n2)
{
    int add = n1 + n2;
    printf("덧셈 결과는 * %d * 입니다. \n", add);
}

int f_sub(int n1, int n2)
{
    int sub = n1 - n2;
    printf("뺄셈 결과는 * %d * 입니다. \n", sub);
}

int f_mul(int n1, int n2)
{
    int mul = n1 * n2;
    printf("곱셈 결과는 * %d * 입니다. \n", mul);
}

double f_div(double n1, double n2)
{
    double div = n1 / n2;
    printf("나눗셈 결과는 * %0.2f * 입니다. \n", div);
}

int f_rmn(int n1, int n2)
{
    int rmn = n1 % n2;
    printf("나머지 결과는 * %d * 입니다. \n", rmn);
}

int f_quo(int n1, int n2)
{
    int quo = n1 / n2;
    printf("몫은 * %d * 입니다. \n", quo);
}

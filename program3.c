/* 사용자가 숫자를 원하는 개수만큼 입력한다.
     출력 결과는, 가장 큰 수와 가장 작은수, 홀수와 짝수인지, 양수, 0, 음수의 판결 결과를 출력 */

#include<stdio.h>

int main(void)
{
    int i, num;
    int a[2]={0};
    int kwak;
    
    printf("다음 정수를 입력하시오.");
    scanf("%d",&num);
    
    a[0]=num;
    a[1]=num;
    
    while(scanf("%d",&num))
    {
        printf("다음 정수를 입력하세요. (끝내려면 q)\n");
        
        if(a[1]<=num)
        {
            a[1]=num;
        }
        else if (a[0]>=num)
        {
            a[0]=num;
        }
        
        printf("최댓값 : %d\n최솟값 : %d\n",a[1],a[0]);
    }
    
    if(a[1]%2 ==0)
    {
    	printf("%d는 짝수입니다.\n",a[1]);
    }
    else
    {
    	printf("%d는 홀수입니다.\n",a[1]);
    }
    
    if(a[0]%2 ==0)
    {
        printf("%d는 짝수입니다.\n",a[0]);
    }
    else
    {
	printf("%d는 홀수입니다.\n",a[0]);
    }
    
    if(a[1]>0)
    {
        printf("%d는 양수입니다.\n",a[1]);
    }
    else if(a[1]==0)
    {
        printf("%d는 0입니다.\n",a[1]);
    }
    else
    {
    	printf("%d는 음수입니다.\n",a[1]);
    }
    
    if(a[0]>0)
    {
        printf("%d는 양수입니다.\n",a[1]);
    }
    else if(a[0]==0)
    {
        printf("%d는 0입니다.\n",a[0]);
    }
    else
    {
        printf("%d는 음수입니다.\n",a[0]);
    }
 
    return 0;
}

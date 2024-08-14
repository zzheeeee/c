/*  */

#include <stdio.h>

int main(void)
{
    int a[5][5];

    int i, j, k = 0;

    for(i=0; i<=4; i++)
    {
        for(j=0; j<=4; j++)
        {
            k = k + 1;
            a[i][j] = k;
            printf("%3d", a[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
    return 0;
}
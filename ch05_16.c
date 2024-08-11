/* 육상선수에게 유용한 프로그램 */

#include <stdio.h>
const int S_PER_M = 60;         // 1분의 초 수
const int S_PER_H = 3600;       // 1시간의 초 수
const double M_PER_K = 0.62137; // 1킬로미터의 마일 

int main(void)
{
    double distk, distm;  // 달린 거리 (각각 킬로미터 수와 마일 수)
    double rate;          // 평균속도(mph; 시간당 마일 수)
    int min, sec;         // 달린 시간(분, 초)
    int time;             // 달린 시간(초)
    double mtime;         // 1마일을 달린 시간(초)
    int mmin, msec;       // 1마일을 달린 시간(분, 초)
    
    printf("이 프로그램은 미터단위계로 측정한 달리기 기록을\n");
    printf("1마일을 달린 시간과, 평균속도(mph)로 변환한다.\n");
    printf("달린 거리를 킬로미터 수로 입력하세요.\n");
    scanf("%lf", &distk);  // %lf for type double
    printf("1. 달린 시간을 분 단위로 입력하세요.\n");
    scanf("%d", &min);
    printf("2. 달린 시간을 초 단위로 입력하세요\n");
    scanf("%d", &sec);

    // 달린 분+초를 초로 변환한다.
    time = S_PER_M * min + sec;
    // 킬로미터를 마일로 변환한다.
    distm = M_PER_K * distk;
    // 초당 마일 * 시간당 초 = mph
    rate = distm / time * S_PER_H;
    // 시간/거리 = 마일당 시간

    mtime = (double) time / distm;
    mmin = (int) mtime / S_PER_M; // 분을 구한다.
    msec = (int) mtime % S_PER_M; // 나머지 초를 구한다.
    printf("당신은 %1.2f킬로미터(%1.2f마일)를 %d분 %d초에 달렸습니다.\n",
              distk, distm, min, sec);
    printf("이 페이스는 1마일을 %d분 %d초에 달린 것에 해당합니다. %1.2f mph.\n",
              mmin, msec, rate);
    
    return 0;
}

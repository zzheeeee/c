#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#define ENCHANT_RATE 20 // 장비 강화 확률(20%)

/* 펫 */
typedef struct
{
    char *name;
    int skill;
} Pet;

/* 사용자  */
// 레벨, 체력, 공격력, 방어력, 골드, cnt[8], 히어로[3], pet, 장비 티어<무기, 갑옷, 신발, 장갑, 망토, 마스크>
typedef struct
{
    char *name;
    int level;      // min: 1, max: 10
    double health;  // 총 체력
    double hp;      // 체력
    float power;    // 공격력
    float depense;  // 방어력(데미지 감소)
    int gold;       // 골드
    int cnt[8];     // 빨포, 주포, 맑포, 고포, 마을, 텔포, 장비, 엘릭서
    int heroCnt[3]; // 히어로 주문서
    Pet pet;        // 펫
    int swordIdx;   // 장비-무기 현재 티어(배열 인덱스)
    int armorIdx;   // 장비-갑옷
    int shoesIdx;   // 장비-신발
    int glovesIdx;  // 장비-장갑
    int capeIdx;    // 장비-망토
    int maskIdx;    // 장비-마스크
} Player;

/* 사용자 스탯 계산 함수 */
void calcPlayer(Player *jyb, int playerHealth, float *equipEnchant)
{
    int powerStat[4] = {2, 5, 10, 20};
    int depenseStat[4] = {1, 4, 8, 13};
    int levelPlus = jyb->level * 10;
    int petPower = 0;
    int petDefense = 0;
    if (jyb->pet.name == "도베르만")
    {
        petPower = jyb->pet.skill;
    }
    else if (jyb->pet.name == "비숑")
    {
        petDefense = jyb->pet.skill;
    }
    jyb->power = powerStat[jyb->swordIdx] * equipEnchant[0] + petPower + levelPlus;
    jyb->depense = depenseStat[jyb->armorIdx] * equipEnchant[1] + depenseStat[jyb->shoesIdx] * equipEnchant[2] +
                   depenseStat[jyb->glovesIdx] * equipEnchant[3] + depenseStat[jyb->capeIdx] * equipEnchant[4] + depenseStat[jyb->maskIdx] * equipEnchant[5] +
                   petDefense + levelPlus;
    jyb->health = playerHealth + levelPlus;
    // printf("%s 공격력 : %d, 방어력 : %d, 체력 : %d\n", jyb->name, jyb->power, jyb->depense, jyb->health);
}

void printPlayerInfo(Player player, float *equipEnchant)
{
    printf("사용자 이름: %s\n", player.name);
    printf("레벨: %d\n", player.level);
    printf("체력: %lf\n", player.health);
    printf("공격력: %.2f\n", player.power);
    printf("방어력: %.2f\n", player.depense);
    printf("골드: %d\n", player.gold);
    printf("펫: %s, %d", player.pet.name, player.pet.skill);
    printf("장비 인첸트: %f / %f / %f / %f / %f / %f", equipEnchant[0], equipEnchant[1], equipEnchant[2], equipEnchant[3], equipEnchant[4], equipEnchant[5]);
    printf("\n");
}

/* 몬스터 */
typedef struct
{
    char *name;
    // 체력 범위, 체력
    int minHealth;
    int maxHealth;
    double health; // 총 체력
    double hp;     // 체력
    // 1회 공격 데미지 범위
    int minDamage;
    int maxDamage;
    // 골드 드롭 범위
    int minGold;
    int maxGold;
    // 마귀, 이동 주문서 드롭률
    float returnRate;
    float teleportRate;
    // 2티어 ~ 4티어 드롭률
    float tier2Rate;
    float tier3Rate;
    float tier4Rate;
    // 엘릭서 드롭률
    float elixirRate;
    // 처치 시 총 체력 상승률
    float healthRate;
} Monster;

/* 보스 */
typedef struct
{
    char *name;
    double health; // 총 체력
    double hp;     // 체력
    // 1회 공격 데미지 범위
    int minDamage;
    int maxDamage;
    // 골드 드롭 범위
    int minGold;
    int maxGold;
    // 마귀, 이동 주문서 드롭률
    float returnRate;
    float teleportRate;
    // 2티어 ~ 4티어 드롭률
    float tier2Rate;
    float tier3Rate;
    float tier4Rate;
    // 엘릭서 드롭률
    float elixirRate;
    // 처치 시 총 체력 상승률
    float healthRate;
} Boss;

/* 몬스터 > 생성 함수 */
Monster createMonster(char *name, int minHealth, int maxHealth, int minDamage, int maxDamage, int minGold, int maxGold,
                      float scrollReturnRate, float scrollTeleportRate, float tier2Rate, float tier3Rate, float tier4Rate, float elixirRate, float healthRate)
{
    // 이름, 체력min/max, 데미지min/max, 골드min/max, 마귀, 텔포, 2티어, 3티어, 4티어, 엘릭서, 처치시총체력
    Monster mon;
    mon.name = name;
    mon.minHealth = minHealth;
    mon.maxHealth = maxHealth;
    mon.health = (rand() % (maxHealth - minHealth + 1)) + minHealth;
    mon.hp = mon.health;
    mon.minDamage = minDamage;
    mon.maxDamage = maxDamage;
    mon.minGold = minGold;
    mon.maxGold = maxGold;
    mon.returnRate = scrollReturnRate;
    mon.teleportRate = scrollTeleportRate;
    mon.tier2Rate = tier2Rate;
    mon.tier3Rate = tier3Rate;
    mon.tier4Rate = tier4Rate;
    mon.elixirRate = elixirRate;
    mon.healthRate = healthRate;

    return mon;
}

/* 보스몬스터 > 생성 함수 */
Boss createBoss(char *name, double health, int minDamage, int maxDamage, int minGold, int maxGold,
                float scrollReturnRate, float scrollTeleportRate, float tier2Rate, float tier3Rate, float tier4Rate, float elixirRate, float healthRate)
{
    // 이름, 체력, 데미지min/max, 골드min/max, 마귀, 텔포, 2티어/3티어/4티어, 엘릭서 드롭률, 처치시총체력
    Boss boss;
    boss.name = name;
    boss.health = health;
    boss.hp = health;
    boss.minDamage = minDamage;
    boss.maxDamage = maxDamage;
    boss.minGold = minGold;
    boss.maxGold = maxGold;
    boss.returnRate = scrollReturnRate;
    boss.teleportRate = scrollTeleportRate;
    boss.tier2Rate = tier2Rate;
    boss.tier3Rate = tier3Rate;
    boss.tier4Rate = tier4Rate;
    boss.elixirRate = elixirRate;
    boss.healthRate = healthRate;

    return boss;
}

// 몬스터 정보 출력 함수
void printMonsterInfo(Monster monster)
{
    printf("몬스터: %s\n", monster.name);
    printf("체력: %lf\n", monster.health);
    printf("공격력: %d\n", rand() % (monster.maxDamage - monster.minDamage + 1) + monster.minDamage);
    printf("골드 드롭: %d\n", rand() % (monster.maxGold - monster.minGold + 1) + monster.minGold);
    printf("마을이동주문서 드롭률: %.2f%%\n", monster.returnRate * 100);
    printf("\n");
}

// 보스 정보 출력 함수
void printBossInfo(Boss boss)
{
    printf("보스: %s\n", boss.name);
    printf("체력: %lf\n", boss.health);
    printf("공격력: %d\n", rand() % (boss.maxDamage - boss.minDamage + 1) + boss.minDamage);
    printf("골드 드롭: %d\n", rand() % (boss.maxGold - boss.minGold + 1) + boss.minGold);
    printf("마을이동주문서 드롭률: %.2f%%\n", boss.returnRate * 100);
    printf("\n");
}

/*  아이템 >> 물약 >> 구조체 */
typedef struct
{
    char *name;
    int plus;
    int price;
    int count;
} Potion;

/* 아이템 > 물약 >> 생성 함수 */
Potion createPotion(char *name, int plus, int price)
{
    Potion pot;
    pot.name = name;
    pot.plus = plus;
    pot.price = price;

    return pot;
}

/* 아이템 > 물약 >> 사용 함수 */
void hpPlus(Potion *potion, Player *jyb, int use)
{
    for (int i = 0; i < use; i++)
    {
        potion->count--;
        jyb->hp += potion->plus;
    }
}

/* 아이템 > 장비 >> 구조체 */
typedef struct
{
    char *name;
    int enchantLV;
    float basicPerformance;
} Equipment;

/* 아이템 > 장비 >> 강화 후 성능 계산 함수 */
float calcPerformance(Equipment *equip)
{
    if (equip->enchantLV == 0)
    {
        // return equip->basicPerformance;
        return 1.0;
    }
    else // 110% : 1.1, 120% : 1.2, 130% : 1.3, 140% : 1.4, ...
    {
        // return equip->basicPerformance * (1.0 + (equip->enchantLV * 0.1));
        return 1.0 + equip->enchantLV * 0.1;
    }
}

/* 아이템 > 장비 >> 장비 강화 함수 */
int enchantEquipment(Equipment *equip, Player *player, int index, float *enchant, bool isElixir)
{
    int elixirCnt = player->cnt[7];
    int scrollCnt = player->cnt[6];
    if (scrollCnt <= 0)
    {
        printf("강화에 필요한 주문서가 부족합니다.\n");
        return 0;
    }
    if (equip->enchantLV >= 10)
    {
        printf("이미 최대 인첸트(+10)입니다.\n");
        return 0;
    }
    else
    {
        (scrollCnt)--; // 주문서 하나 차감

        // 강화 시도
        printf("%s 강화 시도 중 ...", equip->name, index);
        int succ = rand() % 100 + 1;

        if (isElixir == 1)
        {
            succ = 100;
            player->cnt[7]--;
        }

        if (succ <= ENCHANT_RATE) // 강화 성공
        {
            equip->enchantLV++;
            printf("강화 성공!! %s의 인첸트가 %d 증가했습니다.\n", equip->name, equip->enchantLV);
            printf("현재 성능은 %.2f%%입니다.\n", equip->basicPerformance * (1.0 + (equip->enchantLV * 0.1)));
            enchant[index] = calcPerformance(equip);
            return 0;
        }
        else // 강화 실패
        {
            if (equip->enchantLV == 0)
            {
                printf("강화 실패!! 최소 인챈트는 0입니다.\n");
                printf("현재 성능은 %.2f%%입니다.\n", equip->basicPerformance * (1.0 + (equip->enchantLV * 0.1)));
                enchant[index] = calcPerformance(equip);
                return 1; // 강화 실패
            }
            else
            {
                equip->enchantLV--;
                printf("강화 실패!! %s의 인첸트가 %d 감소했습니다.\n", equip->name, equip->enchantLV);
                printf("현재 성능은 %.2f%%입니다.\n", equip->basicPerformance * (1.0 + (equip->enchantLV * 0.1)));
                enchant[index] = calcPerformance(equip);
                return 1; // 강화 실패
            }
        }
        return 0; // 강화 성공
    }
}

int main()
{
    int playerHP = 1000;
    int floor;

    /* equipEnchant[] : 장비 강화 수치 배열 */
    // 장비 강화 시 업데이트
    // 티어 변경 시 업데이트 (0으로)
    // swordEnchant, armorEnchant, shoesEnchant, glovesEnchant, capeEnchant, maskEnchant
    float equipEnchant[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    /* names[] : 우리 반 친구들 이름 배열 */
    char *names[] = {"차수빈", "신정희", "이광영", "임민호", "곽성민", "신명호", "김민석", "최윤식", "김이삭", "김지섭", "김수현", "박성민", "정세현"};
    ////////////////////////////////////////////////

    /* 아이템 >> 포션 :: 이름, 증가량, 가격 */
    Potion red = createPotion("빨간포션", 30, 30);        // cnt[0]
    Potion orange = createPotion("주황포션", 50, 50);     // cnt[1]
    Potion clear = createPotion("맑은포션", 70, 70);      // cnt[2]
    Potion strong = createPotion("고농도포션", 150, 150); // cnt[3]
    red.count = 5;
    orange.count = 5;
    clear.count = 5;
    strong.count = 5;

    // cnt[4] 마을 이동 주문서
    int returnScroll = 0;
    // cnt[5] 순간 이동 주문서
    int teleportScroll = 0;
    // cnt[6] 장비 강화 주문서
    int enchantScroll = 10;
    // cnt[7] 엘릭서
    int elixir = 3;
    // heroCnt[공격, 방어, 회피]
    int heroAttack = 0;
    int heroDefense = 0;
    int heroAvoid = 0;
    /* 아이템 >> 장비 :: 이름, 인챈트 단계(0~10), 성능 */
    Equipment sword[4] = {{"전설의 검", 0, 2.0}, {"장검", 0, 5.0}, {"일본도", 0, 10.0}, {"싸울아비장검", 0, 20.0}};
    Equipment armor[4] = {{"기본 갑옷", 0, 1.0}, {"반팔 갑옷", 0, 4.0}, {"후드 갑옷", 0, 8.0}, {"용 갑옷", 0, 13.20}};
    Equipment shoes[4] = {{"기본 장화", 0, 1.0}, {"슬리퍼", 0, 4.0}, {"운동화", 0, 8.0}, {"에어조단", 0, 13.20}};
    Equipment gloves[4] = {{"기본 장갑", 0, 1.0}, {"고무 장갑", 0, 4.0}, {"면 장갑", 0, 8.0}, {"가죽 장갑", 0, 13.20}};
    Equipment cape[4] = {{"기본 망토", 0, 1.0}, {"면 망토", 0, 4.0}, {"비단 망토", 0, 8.0}, {"방탄 망토", 0, 13.20}};
    Equipment mask[4] = {{"기본 마스크", 0, 1.0}, {"k80 마스크", 0, 4.0}, {"k94 마스크", 0, 8.0}, {"타이거 마스크", 0, 13.20}};

    //////////////////////////////////////////////////////////////////////

    srand(time(NULL));

    /* 플레이어 생성 */
    //                    레벨, 체력, 공격력, 방어력, 골드, cnt[8], pet, 장비 티어<무기, 갑옷, 신발, 장갑, 망토, 마스크>
    Player jyb = {"전용복", 1, playerHP, playerHP, 0.0, 0.0, 300, {red.count, orange.count, clear.count, strong.count, returnScroll, teleportScroll, enchantScroll, elixir}, {heroAttack, heroDefense, heroAvoid}, {"없음", 0}, 0, 0, 0, 0, 0, 0};
    calcPlayer(&jyb, playerHP, equipEnchant);

    printPlayerInfo(jyb, equipEnchant);
    //{"없음", 0}
    /* 몬스터 생성 */
    // 이름, 체력min/max, 데미지min/max, 골드min/max, <드롭률>마귀/텔포줌/2티어/3티어/4티어/엘릭서, 체력상승률
    // 오크 전사 생성 (1층 y44 x22)
    Monster orcWarrior = createMonster("오크 전사", 50, 100, 10, 15, 5, 30, 0.2, 0.0, 0.0, 0.0, 0.0, 0.0, 1);
    printMonsterInfo(orcWarrior);
    // 좀비 생성 (2, 3, 4층 y44 x22)
    Monster zombie = createMonster("좀비", 50, 180, 17, 30, 5, 60, 0.2, 0.0, 0.0, 0.0, 0.0, 0.0, 2);
    printMonsterInfo(zombie);
    // 구울 생성 (3, 4, 5, 6층 y44 x22)
    Monster ghoul = createMonster("구울", 120, 280, 20, 45, 5, 100, 0.2, 0.0, 0.2, 0.0, 0.0, 0.0, 3);
    printMonsterInfo(ghoul);
    // 해골 생성 (4, 5, 6층 y44 x22)
    Monster Skeleton = createMonster("해골", 120, 280, 20, 45, 5, 100, 0.2, 0.0, 0.0, 0.0, 0.0, 0.0, 3);
    printMonsterInfo(Skeleton);
    // 스파토이 생성 (4, 5, 6층 y44 x22)
    Monster Spatoy = createMonster("스파토이", 260, 360, 32, 75, 5, 200, 0.2, 0.0, 0.2, 0.0, 0.0, 0.0, 5);
    printMonsterInfo(Spatoy);
    /* 보스 생성 */
    // 이름, 체력, 데미지min/max, 골드min/max, 마귀, 텔포, 2티어/3티어/4티어, 엘릭서 드롭률, 처치시총체력
    // 네임드 생성 (1, 2, 3, 4, 5, 6, 7층)
    Boss Friends[13];
    for (int i = 0; i < 13; i++)
    {
        Friends[i] = createBoss(names[i], jyb.health * 2, 100, 300, 5, 500, 0.2, 0.3, 0.2, 0.2, 0.0, 0.0, 20);
        printBossInfo(Friends[i]);
    }
    // 바포메트(5,6,7층)
    Boss BPMT = createBoss("바포메트", jyb.health * 5, 180, 450, 5, 700, 0.2, 0.3, 0.0, 0.2, 0.05, 0.0, 20);
    // 이동녀크(6층)
    Boss LDNK = createBoss("이동녀크", jyb.health * 7, 300, 550, 5, 1000, 0.0, 0.3, 0.0, 0.2, 0.1, 0.1, 60);
    // 최상달(7층)
    Boss CSD = createBoss("최상달", jyb.health * 10, 500, 1300, 5, 3000, 0.0, 0.3, 0.0, 0.3, 0.2, 0.2, 100);

    // for (int i = 1; i <= enchantScroll; i++)
    // {
    //     int enchantIdx;
    //     while (1)
    //     {
    //         printf("0:무기 1:갑옷 2:신발 3:장갑 4:망토 5:마스크\n입력: ");
    //         scanf("%d", &enchantIdx);

    //         if (enchantIdx == 0)
    //         {
    //             enchantEquipment(&sword[jyb.swordIdx], &jyb, enchantIdx, equipEnchant, false);
    //             break;
    //         }
    //         else if (enchantIdx == 1)
    //         {
    //             enchantEquipment(&armor[jyb.armorIdx], &jyb, enchantIdx, equipEnchant, false);
    //             break;
    //         }
    //         else if (enchantIdx == 2)
    //         {
    //             enchantEquipment(&shoes[jyb.shoesIdx], &jyb, enchantIdx, equipEnchant, false);
    //             break;
    //         }
    //         else if (enchantIdx == 3)
    //         {
    //             enchantEquipment(&gloves[jyb.glovesIdx], &jyb, enchantIdx, equipEnchant, false);
    //             break;
    //         }
    //         else if (enchantIdx == 4)
    //         {
    //             enchantEquipment(&cape[jyb.capeIdx], &jyb, enchantIdx, equipEnchant, false);
    //             break;
    //         }
    //         else if (enchantIdx == 5)
    //         {
    //             enchantEquipment(&mask[jyb.maskIdx], &jyb, enchantIdx, equipEnchant, false);
    //             break;
    //         }
    //         else
    //         {
    //             printf("다시 입력하세요.\n"); // 종료하려면 q를 입력하세요 추가하기
    //             continue;
    //         }
    //     }

    //     while (getchar() != '\n')
    //         ;
    //     calcPlayer(&jyb, playerHP, equipEnchant);
    //     printPlayerInfo(jyb, equipEnchant);
    // }

    while (1)
    {
        int potionCnt;
        jyb.hp = 150;
        printf("\n용복이 체력 : %.2lf", jyb.hp);
        while (red.count != 0)
        {
            printf("\n 빨간 포션 증가량 : %d, 포션 개수 : %d\n 몇 개 사용하시겠습니까? : ", red.plus, red.count);
            scanf("%d", &potionCnt);
            if (potionCnt > red.count)
            {
                printf("\n사용할 포션이 부족합니다. %d개만 사용됩니다.", red.count);
                potionCnt= red.count;
            }

            hpPlus(&red, &jyb, potionCnt);
            printf("\n용복이 체력 : %.2lf", jyb.hp);
        }

        printf("\n-----------------------------------------------------");
        jyb.hp = 150;
        printf("\n용복이 체력 : %.2lf", jyb.hp);
        while (orange.count != 0)
        {
            printf("\n주황 포션 증가량 : %d, 포션 개수 : %d\n 몇 개 사용하시겠습니까? : ", orange.plus, orange.count);
            scanf("%d", &potionCnt);
            if (potionCnt > orange.count)
            {
                printf("\n사용할 포션이 부족합니다. %d개만 사용됩니다.", orange.count);
                potionCnt= orange.count;
            }

            hpPlus(&orange, &jyb, potionCnt);
            printf("\n용복이 체력 : %.2lf", jyb.hp);
        }

        printf("\n-----------------------------------------------------");
        jyb.hp = 150;
        printf("\n용복이 체력 : %.2lf", jyb.hp);
        while (clear.count != 0)
        {
            printf("\n 맑은 포션 증가량 : %d, 포션 개수 : %d\n 몇 개 사용하시겠습니까? : ", clear.plus, clear.count);
            scanf("%d", &potionCnt);
            if (potionCnt > clear.count)
            {
                printf("\n사용할 포션이 부족합니다. %d개만 사용됩니다.", clear.count);
                potionCnt= clear.count;
            }
            hpPlus(&clear, &jyb, potionCnt);
            printf("\n용복이 체력 : %.2lf", jyb.hp);
        }

        printf("\n-----------------------------------------------------");
        jyb.hp = 150;
        printf("\n용복이 체력 : %.2lf", jyb.hp);
        while (strong.count != 0)
        {
            printf("\n 고농축 포션 증가량 : %d, 포션 개수 : %d\n 몇 개 사용하시겠습니까? : ", strong.plus, strong.count);
            scanf("%d", &potionCnt);
            if (potionCnt > strong.count)
            {
                printf("\n사용할 포션이 부족합니다. %d개만 사용됩니다.", strong.count);
                potionCnt= strong.count;
            }

            hpPlus(&strong, &jyb, potionCnt);
            printf("\n용복이 체력 : %.2lf", jyb.hp);
        }

        break;
    }

    return 0;
}
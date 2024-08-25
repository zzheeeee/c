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
    jyb->health = playerHealth;
    // printf("%s 공격력 : %d, 방어력 : %d, 체력 : %d\n", jyb->name, jyb->power, jyb->depense, jyb->health);
}

void printPlayerInfo(Player player, float *equipEnchant)
{
    printf("사용자 이름: %s\n", player.name);
    printf("레벨: %d\n", player.level);
    printf("체력: %.2lf\n", player.health);
    printf("공격력: %.2f\n", player.power);
    printf("방어력: %.2f\n", player.depense);
    printf("골드: %d\n", player.gold);
    printf("펫: %s, %d\n", player.pet.name, player.pet.skill);
    printf("장비 인첸트: %.2f / %.2f / %.2f / %.2f / %.2f / %.2f\n", equipEnchant[0], equipEnchant[1], equipEnchant[2], equipEnchant[3], equipEnchant[4], equipEnchant[5]);
}

/* 몬스터 */
typedef struct
{
    char *name;         // 이름
    int minHealth;      // 체력 범위 min
    int maxHealth;      // 체력 범위 max
    double health;      // 총 체력
    double hp;          // 체력
    int minDamage;      // 1회 공격 데미지 범위 min
    int maxDamage;      // 1회 공격 데미지 범위 max
    int minGold;        // 골드 드롭 범위 min
    int maxGold;        // 골드 드롭 범위 max
    float returnRate;   // 마귀 주문서 드롭률
    float teleportRate; // 이동 주문서 드롭률
    float tier2Rate;    // 2티어 드롭률
    float tier3Rate;    // 3티어 드롭률
    float tier4Rate;    // 4티어 드롭률
    float elixirRate;   // 엘릭서 드롭률
    float healthRate;   // 처치 시 총 체력 상승률
} Monster;

/* 보스 */
typedef struct
{
    char *name;         // 이름
    double health;      // 총 체력
    double hp;          // 체력
    int minDamage;      // 1회 공격 데미지 범위 min
    int maxDamage;      // 1회 공격 데미지 범위 max
    int minGold;        // 골드 드롭 범위 min
    int maxGold;        // 골드 드롭 범위 max
    float returnRate;   // 마귀 주문서 드롭률
    float teleportRate; // 이동 주문서 드롭률
    float tier2Rate;    // 2티어 드롭률
    float tier3Rate;    // 3티어 드롭률
    float tier4Rate;    // 4티어 드롭률
    float elixirRate;   // 엘릭서 드롭률
    float healthRate;   // 처치 시 총 체력 상승률
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
    printf("체력: %.2lf\n", monster.health);
    printf("공격력: %d\n", rand() % (monster.maxDamage - monster.minDamage + 1) + monster.minDamage);
    printf("골드 드롭: %d\n", rand() % (monster.maxGold - monster.minGold + 1) + monster.minGold);
    printf("마을이동주문서 드롭률: %.2f%%\n", monster.returnRate * 100);
    printf("\n");
}

// 보스 정보 출력 함수
void printBossInfo(Boss boss)
{
    printf("보스: %s\n", boss.name);
    printf("체력: %.2lf\n", boss.health);
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
        if(jyb->health < (jyb->hp)+(potion->plus))
        {
            jyb->hp = jyb->health;
        }else
        {
            jyb->hp += potion->plus;
        }
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

void bossClear(Boss *boss, Player *player, Equipment *sword, Equipment *armor, Equipment *shoes, Equipment *glove, Equipment *cape, Equipment *mask)
{
    int succ, equip;

    printf("%s를 물리쳤습니다.\n", boss->name);

    printf("//맵(좌표) 값 0으로 바꾸기//\n");

    // 골드 획득
    int gold = rand() % (boss->maxGold - boss->minGold + 1) + boss->minGold;
    printf("%d 골드를 획득했습니다.\n", gold);
    player->gold += gold;

    // 마을이동주문서 획득
    succ = rand() % 100 + 1;
    if (succ <= boss->returnRate * 100)
    {
        printf("마을이동주문서를 획득했습니다.\n");
        player->cnt[4] += 1;
    }

    // 이동 주문서 획득
    succ = rand() % 100 + 1;
    if (succ <= boss->teleportRate * 100)
    {
        printf("이동주문서를 획득했습니다.\n");
        player->cnt[5] += 1;
    }

    // 2티어 장비 획득
    succ = rand() % 100 + 1;
    equip = rand() % 6;
    if (boss->tier2Rate * 100)
    {
        switch (equip)
        {
        case 0:
            if (player->swordIdx < 1)
            {
                player->swordIdx = 1;
                printf("2티어 무기(%s) 획득!\n", sword[1].name);
            }
            break;
        case 1:
            if (player->armorIdx < 1)
            {
                player->armorIdx = 1;
                printf("2티어 갑옷(%s) 획득!\n", armor[1].name);
            }
            break;
        case 2:
            if (player->shoesIdx < 1)
            {
                player->shoesIdx = 1;
                printf("2티어 신발(%s) 획득!\n", shoes[1].name);
            }
            break;
        case 3:
            if (player->glovesIdx < 1)
            {
                player->glovesIdx = 1;
                printf("2티어 장갑(%s) 획득!\n", glove[1].name);
            }
            break;
        case 4:
            if (player->capeIdx < 1)
            {
                player->capeIdx = 1;
                printf("2티어 망토(%s) 획득!\n", cape[1].name);
            }
            break;
        case 5:
            if (player->maskIdx < 1)
            {
                player->maskIdx = 1;
                printf("2티어 마스크(%s) 획득!\n", mask[1].name);
            }
            break;
        }
    }

    // 3티어 장비 획득
    succ = rand() % 100 + 1;
    equip = rand() % 6;
    if (boss->tier3Rate * 100)
    {
        switch (equip)
        {
        case 0:
            if (player->swordIdx == 1)
            {
                player->swordIdx = 2;
                printf("3티어 무기(%s) 획득!\n", sword[2].name);
            }
            break;
        case 1:
            if (player->armorIdx == 1)
            {
                player->armorIdx = 2;
                printf("3티어 갑옷(%s) 획득!\n", armor[2].name);
            }
            break;
        case 2:
            if (player->shoesIdx == 1)
            {
                player->shoesIdx = 2;
                printf("3티어 신발(%s) 획득!\n", shoes[2].name);
            }
            break;
        case 3:
            if (player->glovesIdx == 1)
            {
                player->glovesIdx = 2;
                printf("3티어 장갑(%s) 획득!\n", glove[2].name);
            }
            break;
        case 4:
            if (player->capeIdx == 1)
            {
                player->capeIdx = 2;
                printf("3티어 망토(%s) 획득!\n", cape[2].name);
            }
            break;
        case 5:
            if (player->maskIdx == 1)
            {
                player->maskIdx = 2;
                printf("3티어 마스크(%s) 획득!\n", mask[2].name);
            }
            break;
        }
    }

    // 4 티어 장비 획득
    succ = rand() % 100 + 1;
    equip = rand() % 6;
    if (boss->tier4Rate * 100)
    {
        switch (equip)
        {
        case 0:
            if (player->swordIdx == 2)
            {
                player->swordIdx = 3;
                printf("4티어 무기(%s) 획득!\n", sword[3].name);
            }
            break;
        case 1:
            if (player->armorIdx == 2)
            {
                player->armorIdx = 3;
                printf("4티어 갑옷(%s) 획득!\n", armor[3].name);
            }
            break;
        case 2:
            if (player->shoesIdx == 2)
            {
                player->shoesIdx = 3;
                printf("4티어 신발(%s) 획득!\n", shoes[3].name);
            }
            break;
        case 3:
            if (player->glovesIdx == 2)
            {
                player->glovesIdx = 3;
                printf("4티어 장갑(%s) 획득!\n", glove[3].name);
            }
            break;
        case 4:
            if (player->capeIdx == 2)
            {
                player->capeIdx = 3;
                printf("4티어 망토(%s) 획득!\n", cape[3].name);
            }
            break;
        case 5:
            if (player->maskIdx == 2)
            {
                player->maskIdx = 3;
                printf("4티어 마스크(%s) 획득!\n", mask[3].name);
            }
            break;
        }
    }

    // 엘릭서 획득
    succ = rand() % 100 + 1;
    if (succ <= boss->elixirRate * 100)
    {
        printf("엘릭서를 획득했습니다.\n");
        player->cnt[7] += 1;
    }

    // 총 체력 증가
    player->health *= (1 + (0.01 * boss->healthRate));
    printf("총 체력이 증가하여 %.2lf가 되었습니다.(hp: %.2lf)", player->health, player->hp);
}

void bossExit(Boss *boss)
{
    // 보스 체력 리셋
    boss->hp = boss->health;
}

void monsterClear(Monster *monster, Player *player, Equipment *sword, Equipment *armor, Equipment *shoes, Equipment *glove, Equipment *cape, Equipment *mask)
{
    int succ, equip;

    printf("%s를 물리쳤습니다.\n", monster->name);

    printf("//맵(좌표) 값 0으로 바꾸기//\n");

    // 골드 획득
    int gold = rand() % (monster->maxGold - monster->minGold + 1) + monster->minGold;
    printf("%d 골드를 획득했습니다.\n", gold);
    player->gold += gold;

    // 마을이동주문서 획득
    succ = rand() % 100 + 1;
    if (succ <= monster->returnRate * 100)
    {
        printf("마을이동주문서를 획득했습니다.\n");
        player->cnt[4] += 1;
    }

    // 이동 주문서 획득
    succ = rand() % 100 + 1;
    if (succ <= monster->teleportRate * 100)
    {
        printf("이동주문서를 획득했습니다.\n");
        player->cnt[5] += 1;
    }

    // 2티어 장비 획득
    succ = rand() % 100 + 1;
    equip = rand() % 6;
    if (monster->tier2Rate * 100)
    {
        switch (equip)
        {
        case 0:
            if (player->swordIdx < 1)
            {
                player->swordIdx = 1;
                printf("2티어 무기(%s) 획득!\n", sword[1].name);
            }
            break;
        case 1:
            if (player->armorIdx < 1)
            {
                player->armorIdx = 1;
                printf("2티어 갑옷(%s) 획득!\n", armor[1].name);
            }
            break;
        case 2:
            if (player->shoesIdx < 1)
            {
                player->shoesIdx = 1;
                printf("2티어 신발(%s) 획득!\n", shoes[1].name);
            }
            break;
        case 3:
            if (player->glovesIdx < 1)
            {
                player->glovesIdx = 1;
                printf("2티어 장갑(%s) 획득!\n", glove[1].name);
            }
            break;
        case 4:
            if (player->capeIdx < 1)
            {
                player->capeIdx = 1;
                printf("2티어 망토(%s) 획득!\n", cape[1].name);
            }
            break;
        case 5:
            if (player->maskIdx < 1)
            {
                player->maskIdx = 1;
                printf("2티어 마스크(%s) 획득!\n", mask[1].name);
            }
            break;
        }
    }

    // 3티어 장비 획득
    succ = rand() % 100 + 1;
    equip = rand() % 6;
    if (monster->tier3Rate * 100)
    {
        switch (equip)
        {
        case 0:
            if (player->swordIdx == 1)
            {
                player->swordIdx = 2;
                printf("3티어 무기(%s) 획득!\n", sword[2].name);
            }
            break;
        case 1:
            if (player->armorIdx == 1)
            {
                player->armorIdx = 2;
                printf("3티어 갑옷(%s) 획득!\n", armor[2].name);
            }
            break;
        case 2:
            if (player->shoesIdx == 1)
            {
                player->shoesIdx = 2;
                printf("3티어 신발(%s) 획득!\n", shoes[2].name);
            }
            break;
        case 3:
            if (player->glovesIdx == 1)
            {
                player->glovesIdx = 2;
                printf("3티어 장갑(%s) 획득!\n", glove[2].name);
            }
            break;
        case 4:
            if (player->capeIdx == 1)
            {
                player->capeIdx = 2;
                printf("3티어 망토(%s) 획득!\n", cape[2].name);
            }
            break;
        case 5:
            if (player->maskIdx == 1)
            {
                player->maskIdx = 2;
                printf("3티어 마스크(%s) 획득!\n", mask[2].name);
            }
            break;
        }
    }

    // 4 티어 장비 획득
    succ = rand() % 100 + 1;
    equip = rand() % 6;
    if (monster->tier4Rate * 100)
    {
        switch (equip)
        {
        case 0:
            if (player->swordIdx == 2)
            {
                player->swordIdx = 3;
                printf("4티어 무기(%s) 획득!\n", sword[3].name);
            }
            break;
        case 1:
            if (player->armorIdx == 2)
            {
                player->armorIdx = 3;
                printf("4티어 갑옷(%s) 획득!\n", armor[3].name);
            }
            break;
        case 2:
            if (player->shoesIdx == 2)
            {
                player->shoesIdx = 3;
                printf("4티어 신발(%s) 획득!\n", shoes[3].name);
            }
            break;
        case 3:
            if (player->glovesIdx == 2)
            {
                player->glovesIdx = 3;
                printf("4티어 장갑(%s) 획득!\n", glove[3].name);
            }
            break;
        case 4:
            if (player->capeIdx == 2)
            {
                player->capeIdx = 3;
                printf("4티어 망토(%s) 획득!\n", cape[3].name);
            }
            break;
        case 5:
            if (player->maskIdx == 2)
            {
                player->maskIdx = 3;
                printf("4티어 마스크(%s) 획득!\n", mask[3].name);
            }
            break;
        }
    }

    // 엘릭서 획득
    succ = rand() % 100 + 1;
    if (succ <= monster->elixirRate * 100)
    {
        printf("엘릭서를 획득했습니다.\n");
        player->cnt[7] += 1;
    }

    // 총 체력 증가
    player->health *= (1 + (0.01 * monster->healthRate));
    printf("총 체력이 증가하여 %.2lf가 되었습니다.(hp: %.2lf)", player->health, player->hp);
}

void monsterExit(Monster *monster)
{
    // 몬스터 체력 리셋
    monster->hp = monster->health;
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
        printf("%s 강화 시도 중 ...", equip->name);
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
    int playerHP = 100;
    int floor;
    int tier2Price = 200;  // 2티어 장비 가격
    int scrollPrice = 300; // 주문서 가격
    char *petName[2] = {"도베르만", "비숑"};

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
    Equipment mask[4] = {{"기본 마스크", 0, 1.0}, {"KF80 마스크", 0, 4.0}, {"KF94 마스크", 0, 8.0}, {"타이거 마스크", 0, 13.20}};

    //////////////////////////////////////////////////////////////////////

    srand(time(NULL));

    /* 플레이어 생성 */
    //                    레벨, 체력, 공격력, 방어력, 골드, cnt[8], pet, 장비 티어<무기, 갑옷, 신발, 장갑, 망토, 마스크>
    Player jyb = {"전용복", 1, playerHP, playerHP, 0.0, 0.0, 300, {red.count, orange.count, clear.count, strong.count, returnScroll, teleportScroll, enchantScroll, elixir}, {heroAttack, heroDefense, heroAvoid}, {"없음", 0}, 0, 0, 0, 0, 0, 0};
    calcPlayer(&jyb, playerHP, equipEnchant);
    jyb.hp = jyb.health;
    printPlayerInfo(jyb, equipEnchant);

    /* 몬스터 생성 */
    Monster orcWarrior; // 오크 전사 
    Monster zombie;     // 좀비 
    Monster ghoul;      // 구울
    Monster Skeleton;   // 해골 
    Monster Spatoy;     // 스파토이

    /* 보스 생성 */
    Boss Friends[13]; // 우리반친구들
    for (int i = 0; i < 13; i++)
    {
        Friends[i] = createBoss(names[i], jyb.health * 2, 100, 300, 5, 500, 0.2, 0.3, 0.2, 0.2, 0.0, 0.0, 20);
        //printBossInfo(Friends[i]);
    }
    // 보스2. 바포메트
    Boss BPMT;
    // 보스3. 이동녀크
    Boss LDNK;
    // 보스4. 최상달
    Boss CSD;

    /* 전투 모드 */
    // printf("체력: %.2lf\n", monster.health);
    // printf("공격력: %d\n", rand() % (monster.maxDamage - monster.minDamage + 1) + monster.minDamage);
    // printf("골드 드롭: %d\n", rand() % (monster.maxGold - monster.minGold + 1) + monster.minGold);
    
    int input, damage, run, potionCnt;
    bool round2;
    bool bAttack = false, bDepense = false, bAvoid = false;
    Monster mon;
    Boss boss;
    for (int mark = 75; mark < 76; mark++)
    {
        switch (mark)
        {
        case 70: // printf("오크 나와!!\n");
            orcWarrior = createMonster("오크 전사", 50, 100, 10, 15, 5, 30, 0.2, 0.0, 0.0, 0.0, 0.0, 0.0, 1);
            // printMonsterInfo(orcWarrior);
            mon = orcWarrior;
            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", mon.name, mon.hp, mon.health);
            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
            printf(">> 1 : 공격 / 2 : 도망가기\n");
            printf(">> 3: 빨간 포션 / 4. 주황 포션 / 5. 맑은 포션 / 6. 고농축 포션\n");
            printf(">> 7. 공격 히어로 주문서(수량: %d) / 8. 방어 히어로 주문서(수량: %d) / 9. 회피 히어로 주문서(수량: %d)\n", jyb.heroCnt[0], jyb.heroCnt[1], jyb.heroCnt[2]);
            while (mon.hp > 0 && jyb.hp > 0)
            {
                printf(">> 입력 : ");
                scanf("%d", &input);
                printf("\n");
                if (input >= 0 && input < 10)
                {
                    switch (input)
                    {
                    case 1: // 공격
                        if (bAttack)
                        {
                            damage = jyb.power + (rand() % 10 + 1) + 10;
                            mon.hp -= damage;
                            printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                        }
                        damage = jyb.power + (rand() % 10 + 1) + 10;
                        mon.hp -= damage;
                        printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                        if (mon.hp > 0)
                        {
                            damage = rand() % (mon.maxDamage - mon.minDamage + 1) + mon.minDamage;
                            if (bDepense)
                            {
                                damage = 0;
                            }
                            if(damage-jyb.depense<=0)
                            {
                                damage = 1;
                            }
                            else{
                                damage -= jyb.depense;
                            }
                            if(jyb.hp-damage < 0){
                                jyb.hp = 0;
                            }else{
                                jyb.hp -= damage;
                            }
                            printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);
                        }
                        if (jyb.hp <= 0)
                        {
                            printf("플레이어 사망\n");
                        }
                        break;
                    case 2: // 도망 50%
                        run = rand() % 2;
                        if (run == 1)
                        {
                            printf("도망 성공, 위치 이동 시키기\n");
                        }
                        else
                        {
                            damage = rand() % (mon.maxDamage - mon.minDamage + 1) + mon.minDamage;
                            if (bDepense)
                            {
                                damage = 0;
                            }                            
                            if(damage-jyb.depense<=0)
                            {
                                damage = 1;
                            }
                            else{
                                damage -= jyb.depense;
                            }
                            if(jyb.hp-damage < 0){
                                jyb.hp = 0;
                            }else{
                                jyb.hp -= damage;
                            }
                            printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);

                            if (jyb.hp > 0)
                            {
                                if (bAttack)
                                {
                                    damage = jyb.power + (rand() % 10 + 1) + 10;
                                    mon.hp -= damage;
                                    printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                                }
                                damage = jyb.power + (rand() % 10 + 1) + 10;
                                mon.hp -= damage;
                                printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                            }
                            if (jyb.hp <= 0)
                            {
                                printf("플레이어 사망\n");
                            }
                        }
                        break;
                    case 3: // 빨간 포션
                        if (jyb.cnt[0] >= 1)
                        {
                            printf("빨간 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[0] -= 1;
                            hpPlus(&red, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 4: // 주황 포션
                        if (jyb.cnt[1] >= 1)
                        {
                            printf("주황 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[1] -= 1;
                            hpPlus(&orange, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 5: // 맑은 포션
                        if (jyb.cnt[2] >= 1)
                        {
                            printf("맑은 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[2] -= 1;
                            hpPlus(&clear, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 6: // 고농축 포션
                        if (jyb.cnt[3] >= 1)
                        {
                            printf("고농축 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[3] -= 1;
                            hpPlus(&strong, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 7: // 공격 히어로
                        if (jyb.heroCnt[0] >= 1)
                        {
                            bAttack = true;
                            printf("공격 히어로를 소환했습니다\n");
                            printf("공격력이 두 배로 증가합니다.\n");
                        }
                        break;
                    case 8: // 방어 히어로
                        if (jyb.heroCnt[1] >= 1)
                        {
                            bDepense = true;
                            printf("방어 히어로를 소환했습니다.\n");
                            printf("데미지를 입지 않습니다.\n");
                        }
                        break;
                    case 9: // 회피 히어로
                        if (jyb.heroCnt[2] >= 1)
                        {
                            printf("회피 히어로를 소환했습니다.\n");
                            printf("리바이 병장이 입체기동장치를 사용하여 나를 데리고 도망칩니다.\n");
                            printf("위치 이동 시키기\n");
                        }
                        break;
                    }
                }
                else
                {
                    printf("다시 입력하세요. \n");
                }
            }
            if (mon.hp <= 0) // 플레이어가 이겼을 때
            {
                monsterClear(&mon, &jyb, sword, armor, shoes, gloves, cape, mask);
            }
            else if (jyb.hp <= 0) // 플레이어가 졌을 때
            {
                printf("체력 얼마 남기고 마을로 이동\n");
            }
            monsterExit(&mon);
            bAttack = bDepense = bAvoid = false;
            break;
        case 71: // printf("좀비 나와!!\n");
            zombie = createMonster("좀비", 50, 180, 17, 30, 5, 60, 0.2, 0.0, 0.0, 0.0, 0.0, 0.0, 2);
            //printMonsterInfo(zombie);
            mon = zombie;
            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", mon.name, mon.hp, mon.health);
            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
            printf(">> 1 : 공격 / 2 : 도망가기\n");
            printf(">> 3: 빨간 포션 / 4. 주황 포션 / 5. 맑은 포션 / 6. 고농축 포션\n");
            printf(">> 7. 공격 히어로 주문서(수량: %d) / 8. 방어 히어로 주문서(수량: %d) / 9. 회피 히어로 주문서(수량: %d)\n", jyb.heroCnt[0], jyb.heroCnt[1], jyb.heroCnt[2]);
            while (mon.hp > 0 && jyb.hp > 0)
            {
                printf(">> 입력 : ");
                scanf("%d", &input);
                printf("\n");
                if (input >= 0 && input < 10)
                {
                    switch (input)
                    {
                    case 1: // 공격
                        if (bAttack)
                        {
                            damage = jyb.power + (rand() % 10 + 1) + 10;
                            mon.hp -= damage;
                            printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                        }
                        damage = jyb.power + (rand() % 10 + 1) + 10;
                        mon.hp -= damage;
                        printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                        if (mon.hp > 0)
                        {
                            damage = rand() % (mon.maxDamage - mon.minDamage + 1) + mon.minDamage;
                            // printf("*******damage min : %d max : %d, damage: %d\n", mon.maxDamage, mon.minDamage, damage);
                            if (bDepense)
                            {
                                damage = 0;
                            }                            
                            if(damage-jyb.depense<=0)
                            {
                                damage = 1;
                            }
                            else{
                                damage -= jyb.depense;
                            }
                            if(jyb.hp-damage < 0){
                                jyb.hp = 0;
                            }else{
                                jyb.hp -= damage;
                            }
                            printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);
                        }
                        if (jyb.hp <= 0)
                        {
                            printf("플레이어 사망\n");
                        }
                        break;
                    case 2: // 도망 50%
                        run = rand() % 2;
                        if (run == 1)
                        {
                            printf("도망 성공, 위치 이동 시키기\n");
                        }
                        else
                        {
                            damage = rand() % (mon.maxDamage - mon.minDamage + 1) + mon.minDamage;
                            if (bDepense)
                            {
                                damage = 0;
                            }                            
                            if(damage-jyb.depense<=0)
                            {
                                damage = 1;
                            }
                            else{
                                damage -= jyb.depense;
                            }
                            if(jyb.hp-damage < 0){
                                jyb.hp = 0;
                            }else{
                                jyb.hp -= damage;
                            }
                            printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);

                            if (jyb.hp > 0)
                            {
                                if (bAttack)
                                {
                                    damage = jyb.power + (rand() % 10 + 1) + 10;
                                    mon.hp -= damage;
                                    printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                                }
                                damage = jyb.power + (rand() % 10 + 1) + 10;
                                mon.hp -= damage;
                                printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                            }
                            if (jyb.hp <= 0)
                            {
                                printf("플레이어 사망\n");
                            }
                        }
                        break;
                    case 3: // 빨간 포션
                        if (jyb.cnt[0] >= 1)
                        {
                            printf("빨간 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[0] -= 1;
                            hpPlus(&red, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 4: // 주황 포션
                        if (jyb.cnt[1] >= 1)
                        {
                            printf("주황 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[1] -= 1;
                            hpPlus(&orange, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 5: // 맑은 포션
                        if (jyb.cnt[2] >= 1)
                        {
                            printf("맑은 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[2] -= 1;
                            hpPlus(&clear, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 6: // 고농축 포션
                        if (jyb.cnt[3] >= 1)
                        {
                            printf("고농축 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[3] -= 1;
                            hpPlus(&strong, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 7: // 공격 히어로
                        if (jyb.heroCnt[0] >= 1)
                        {
                            bAttack = true;
                            printf("공격 히어로를 소환했습니다\n");
                            printf("공격력이 두 배로 증가합니다.\n");
                        }
                        break;
                    case 8: // 방어 히어로
                        if (jyb.heroCnt[1] >= 1)
                        {
                            bDepense = true;
                            printf("방어 히어로를 소환했습니다.\n");
                            printf("데미지를 입지 않습니다.\n");
                        }
                        break;
                    case 9: // 회피 히어로
                        if (jyb.heroCnt[2] >= 1)
                        {
                            printf("회피 히어로를 소환했습니다.\n");
                            printf("리바이 병장이 입체기동장치를 사용하여 나를 데리고 도망칩니다.\n");
                            printf("위치 이동 시키기\n");
                        }
                        break;
                    }
                }
                else
                {
                    printf("다시 입력하세요. \n");
                }
            }
            if (mon.hp <= 0) // 플레이어가 이겼을 때
            {
                monsterClear(&mon, &jyb, sword, armor, shoes, gloves, cape, mask);
            }
            else if (jyb.hp <= 0) // 플레이어가 졌을 때
            {
                printf("체력 얼마 남기고 마을로 이동\n");
            }
            monsterExit(&mon);
            bAttack = bDepense = bAvoid = false;
            break;
        case 72: // printf("구울 나와!!\n");
            ghoul = createMonster("구울", 120, 280, 20, 45, 5, 100, 0.2, 0.0, 0.2, 0.0, 0.0, 0.0, 3);
            //printMonsterInfo(ghoul);
            mon = ghoul;
            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", mon.name, mon.hp, mon.health);
            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
            printf(">> 1 : 공격 / 2 : 도망가기\n");
            printf(">> 3: 빨간 포션 / 4. 주황 포션 / 5. 맑은 포션 / 6. 고농축 포션\n");
            printf(">> 7. 공격 히어로 주문서(수량: %d) / 8. 방어 히어로 주문서(수량: %d) / 9. 회피 히어로 주문서(수량: %d)\n", jyb.heroCnt[0], jyb.heroCnt[1], jyb.heroCnt[2]);
            while (mon.hp > 0 && jyb.hp > 0)
            {
                printf(">> 입력 : ");
                scanf("%d", &input);
                printf("\n");
                if (input >= 0 && input < 10)
                {
                    switch (input)
                    {
                    case 1: // 공격
                        if (bAttack)
                        {
                            damage = jyb.power + (rand() % 10 + 1) + 10;
                            mon.hp -= damage;
                            printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                        }
                        damage = jyb.power + (rand() % 10 + 1) + 10;
                        mon.hp -= damage;
                        printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                        if (mon.hp > 0)
                        {
                            damage = rand() % (mon.maxDamage - mon.minDamage + 1) + mon.minDamage;
                            // printf("*******damage min : %d max : %d, damage: %d\n", mon.maxDamage, mon.minDamage, damage);
                            if (bDepense)
                            {
                                damage = 0;
                            }
                            if(damage-jyb.depense<=0)
                            {
                                damage = 1;
                            }
                            else{
                                damage -= jyb.depense;
                            }
                            if(jyb.hp-damage < 0){
                                jyb.hp = 0;
                            }else{
                                jyb.hp -= damage;
                            }
                            printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);
                        }
                        if (jyb.hp <= 0)
                        {
                            printf("플레이어 사망\n");
                        }
                        break;
                    case 2: // 도망 50%
                        run = rand() % 2;
                        if (run == 1)
                        {
                            printf("도망 성공, 위치 이동 시키기\n");
                        }
                        else
                        {
                            damage = rand() % (mon.maxDamage - mon.minDamage + 1) + mon.minDamage;
                            if (bDepense)
                            {
                                damage = 0;
                            }                            
                            if(damage-jyb.depense<=0)
                            {
                                damage = 1;
                            }
                            else{
                                damage -= jyb.depense;
                            }
                            if(jyb.hp-damage < 0){
                                jyb.hp = 0;
                            }else{
                                jyb.hp -= damage;
                            }
                            printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);

                            if (jyb.hp > 0)
                            {
                                if (bAttack)
                                {
                                    damage = jyb.power + (rand() % 10 + 1) + 10;
                                    mon.hp -= damage;
                                    printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                                }
                                damage = jyb.power + (rand() % 10 + 1) + 10;
                                mon.hp -= damage;
                                printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                            }
                            if (jyb.hp <= 0)
                            {
                                printf("플레이어 사망\n");
                            }
                        } 
                        break;
                    case 3: // 빨간 포션
                        if (jyb.cnt[0] >= 1)
                        {
                            printf("빨간 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[0] -= 1;
                            hpPlus(&red, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 4: // 주황 포션
                        if (jyb.cnt[1] >= 1)
                        {
                            printf("주황 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[1] -= 1;
                            hpPlus(&orange, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 5: // 맑은 포션
                        if (jyb.cnt[2] >= 1)
                        {
                            printf("맑은 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[2] -= 1;
                            hpPlus(&clear, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 6: // 고농축 포션
                        if (jyb.cnt[3] >= 1)
                        {
                            printf("고농축 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[3] -= 1;
                            hpPlus(&strong, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 7: // 공격 히어로
                        if (jyb.heroCnt[0] >= 1)
                        {
                            bAttack = true;
                            printf("공격 히어로를 소환했습니다\n");
                            printf("공격력이 두 배로 증가합니다.\n");
                        }
                        break;
                    case 8: // 방어 히어로
                        if (jyb.heroCnt[1] >= 1)
                        {
                            bDepense = true;
                            printf("방어 히어로를 소환했습니다.\n");
                            printf("데미지를 입지 않습니다.\n");
                        }
                        break;
                    case 9: // 회피 히어로
                        if (jyb.heroCnt[2] >= 1)
                        {
                            printf("회피 히어로를 소환했습니다.\n");
                            printf("리바이 병장이 입체기동장치를 사용하여 나를 데리고 도망칩니다.\n");
                            printf("위치 이동 시키기\n");
                        }
                        break;
                    }
                }
                else
                {
                    printf("다시 입력하세요. \n");
                }
            }
            if (mon.hp <= 0) // 플레이어가 이겼을 때
            {
                monsterClear(&mon, &jyb, sword, armor, shoes, gloves, cape, mask);
            }
            else if (jyb.hp <= 0) // 플레이어가 졌을 때
            {
                printf("체력 얼마 남기고 마을로 이동\n");
            }
            monsterExit(&mon);
            bAttack = bDepense = bAvoid = false;
            break;
        case 73: // printf("해골 나와!!\n");
            Skeleton = createMonster("해골", 120, 280, 20, 45, 5, 100, 0.2, 0.0, 0.0, 0.0, 0.0, 0.0, 3);
            //printMonsterInfo(Skeleton);
            mon = Skeleton;
            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", mon.name, mon.hp, mon.health);
            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
            printf(">> 1 : 공격 / 2 : 도망가기\n");
            printf(">> 3: 빨간 포션 / 4. 주황 포션 / 5. 맑은 포션 / 6. 고농축 포션\n");
            printf(">> 7. 공격 히어로 주문서(수량: %d) / 8. 방어 히어로 주문서(수량: %d) / 9. 회피 히어로 주문서(수량: %d)\n", jyb.heroCnt[0], jyb.heroCnt[1], jyb.heroCnt[2]);
            while (mon.hp > 0 && jyb.hp > 0)
            {
                printf(">> 입력 : ");
                scanf("%d", &input);
                printf("\n");
                if (input >= 0 && input < 10)
                {
                    switch (input)
                    {
                    case 1: // 공격
                        if (bAttack)
                        {
                            damage = jyb.power + (rand() % 10 + 1) + 10;
                            mon.hp -= damage;
                            printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                        }
                        damage = jyb.power + (rand() % 10 + 1) + 10;
                        mon.hp -= damage;
                        printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                        if (mon.hp > 0)
                        {
                            damage = rand() % (mon.maxDamage - mon.minDamage + 1) + mon.minDamage;
                            // printf("*******damage min : %d max : %d, damage: %d\n", mon.maxDamage, mon.minDamage, damage);
                            if (bDepense)
                            {
                                damage = 0;
                            }
                            if(damage-jyb.depense<=0)
                            {
                                damage = 1;
                            }
                            else{
                                damage -= jyb.depense;
                            }
                            if(jyb.hp-damage < 0){
                                jyb.hp = 0;
                            }else{
                                jyb.hp -= damage;
                            }
                            printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);
                        }
                        if (jyb.hp <= 0)
                        {
                            printf("플레이어 사망\n");
                        }
                        break;
                    case 2: // 도망 50%
                        run = rand() % 2;
                        if (run == 1)
                        {
                            printf("도망 성공, 위치 이동 시키기\n");
                        }
                        else
                        {
                            damage = rand() % (mon.maxDamage - mon.minDamage + 1) + mon.minDamage;
                            if (bDepense)
                            {
                                damage = 0;
                            }
                            if(damage-jyb.depense<=0)
                            {
                                damage = 1;
                            }
                            else{
                                damage -= jyb.depense;
                            }
                            if(jyb.hp-damage < 0){
                                jyb.hp = 0;
                            }else{
                                jyb.hp -= damage;
                            }
                            printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);

                            if (jyb.hp > 0)
                            {
                                if (bAttack)
                                {
                                    damage = jyb.power + (rand() % 10 + 1) + 10;
                                    mon.hp -= damage;
                                    printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                                }
                                damage = jyb.power + (rand() % 10 + 1) + 10;
                                mon.hp -= damage;
                                printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                            }
                            if (jyb.hp <= 0)
                            {
                                printf("플레이어 사망\n");
                            }
                        }
                        break;
                    case 3: // 빨간 포션
                        if (jyb.cnt[0] >= 1)
                        {
                            printf("빨간 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[0] -= 1;
                            hpPlus(&red, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 4: // 주황 포션
                        if (jyb.cnt[1] >= 1)
                        {
                            printf("주황 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[1] -= 1;
                            hpPlus(&orange, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 5: // 맑은 포션
                        if (jyb.cnt[2] >= 1)
                        {
                            printf("맑은 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[2] -= 1;
                            hpPlus(&clear, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 6: // 고농축 포션
                        if (jyb.cnt[3] >= 1)
                        {
                            printf("고농축 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[3] -= 1;
                            hpPlus(&strong, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 7: // 공격 히어로
                        if (jyb.heroCnt[0] >= 1)
                        {
                            bAttack = true;
                            printf("공격 히어로를 소환했습니다\n");
                            printf("공격력이 두 배로 증가합니다.\n");
                        }
                        break;
                    case 8: // 방어 히어로
                        if (jyb.heroCnt[1] >= 1)
                        {
                            bDepense = true;
                            printf("방어 히어로를 소환했습니다.\n");
                            printf("데미지를 입지 않습니다.\n");
                        }
                        break;
                    case 9: // 회피 히어로
                        if (jyb.heroCnt[2] >= 1)
                        {
                            printf("회피 히어로를 소환했습니다.\n");
                            printf("리바이 병장이 입체기동장치를 사용하여 나를 데리고 도망칩니다.\n");
                            printf("위치 이동 시키기\n");
                        }
                        break;
                    }
                }
                else
                {
                    printf("다시 입력하세요. \n");
                }
            }
            if (mon.hp <= 0) // 플레이어가 이겼을 때
            {
                monsterClear(&mon, &jyb, sword, armor, shoes, gloves, cape, mask);
            }
            else if (jyb.hp <= 0) // 플레이어가 졌을 때
            {
                printf("체력 얼마 남기고 마을로 이동\n");
            }
            monsterExit(&mon);
            bAttack = bDepense = bAvoid = false;
            break;            
        case 74: // printf("스파토이 나와!!\n");
            Spatoy = createMonster("스파토이", 260, 360, 32, 75, 5, 200, 0.2, 0.0, 0.2, 0.0, 0.0, 0.0, 5);
            //printMonsterInfo(Spatoy);
            mon = Spatoy;
            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", mon.name, mon.hp, mon.health);
            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
            printf(">> 1 : 공격 / 2 : 도망가기\n");
            printf(">> 3: 빨간 포션 / 4. 주황 포션 / 5. 맑은 포션 / 6. 고농축 포션\n");
            printf(">> 7. 공격 히어로 주문서(수량: %d) / 8. 방어 히어로 주문서(수량: %d) / 9. 회피 히어로 주문서(수량: %d)\n", jyb.heroCnt[0], jyb.heroCnt[1], jyb.heroCnt[2]);
            while (mon.hp > 0 && jyb.hp > 0)
            {
                printf(">> 입력 : ");
                scanf("%d", &input);
                printf("\n");
                if (input >= 0 && input < 10)
                {
                    switch (input)
                    {
                    case 1: // 공격
                        if (bAttack)
                        {
                            damage = jyb.power + (rand() % 10 + 1) + 10;
                            mon.hp -= damage;
                            printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                        }
                        damage = jyb.power + (rand() % 10 + 1) + 10;
                        mon.hp -= damage;
                        printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                        if (mon.hp > 0)
                        {
                            damage = rand() % (mon.maxDamage - mon.minDamage + 1) + mon.minDamage;
                            // printf("*******damage min : %d max : %d, damage: %d\n", mon.maxDamage, mon.minDamage, damage);
                            if (bDepense)
                            {
                                damage = 0;
                            }
                            if(damage-jyb.depense<=0)
                            {
                                damage = 1;
                            }
                            else{
                                damage -= jyb.depense;
                            }
                            if(jyb.hp-damage < 0){
                                jyb.hp = 0;
                            }else{
                                jyb.hp -= damage;
                            }
                            printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);
                        }
                        if (jyb.hp <= 0)
                        {
                            printf("플레이어 사망\n");
                        }
                        break;
                    case 2: // 도망 50%
                        run = rand() % 2;
                        if (run == 1)
                        {
                            printf("도망 성공, 위치 이동 시키기\n");
                        }
                        else
                        {
                            damage = rand() % (mon.maxDamage - mon.minDamage + 1) + mon.minDamage;
                            if (bDepense)
                            {
                                damage = 0;
                            }
                            if(damage-jyb.depense<=0)
                            {
                                damage = 1;
                            }
                            else{
                                damage -= jyb.depense;
                            }
                            if(jyb.hp-damage < 0){
                                jyb.hp = 0;
                            }else{
                                jyb.hp -= damage;
                            }
                            printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);

                            if (jyb.hp > 0)
                            {
                                if (bAttack)
                                {
                                    damage = jyb.power + (rand() % 10 + 1) + 10;
                                    mon.hp -= damage;
                                    printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                                }
                                damage = jyb.power + (rand() % 10 + 1) + 10;
                                mon.hp -= damage;
                                printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                            }
                            if (jyb.hp <= 0)
                            {
                                printf("플레이어 사망\n");
                            }
                        }
                        break;
                    case 3: // 빨간 포션
                        if (jyb.cnt[0] >= 1)
                        {
                            printf("빨간 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[0] -= 1;
                            hpPlus(&red, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 4: // 주황 포션
                        if (jyb.cnt[1] >= 1)
                        {
                            printf("주황 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[1] -= 1;
                            hpPlus(&orange, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 5: // 맑은 포션
                        if (jyb.cnt[2] >= 1)
                        {
                            printf("맑은 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[2] -= 1;
                            hpPlus(&clear, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 6: // 고농축 포션
                        if (jyb.cnt[3] >= 1)
                        {
                            printf("고농축 포션을 사용합니다.\n");
                            printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                            jyb.cnt[3] -= 1;
                            hpPlus(&strong, &jyb, 1);
                        }
                        printf("///체력 리셋///\n");
                        break;
                    case 7: // 공격 히어로
                        if (jyb.heroCnt[0] >= 1)
                        {
                            bAttack = true;
                            printf("공격 히어로를 소환했습니다\n");
                            printf("공격력이 두 배로 증가합니다.\n");
                        }
                        break;
                    case 8: // 방어 히어로
                        if (jyb.heroCnt[1] >= 1)
                        {
                            bDepense = true;
                            printf("방어 히어로를 소환했습니다.\n");
                            printf("데미지를 입지 않습니다.\n");
                        }
                        break;
                    case 9: // 회피 히어로
                        if (jyb.heroCnt[2] >= 1)
                        {
                            printf("회피 히어로를 소환했습니다.\n");
                            printf("리바이 병장이 입체기동장치를 사용하여 나를 데리고 도망칩니다.\n");
                            printf("위치 이동 시키기\n");
                        }
                        break;
                    }
                }
                else
                {
                    printf("다시 입력하세요. \n");
                }
            }
            if (mon.hp <= 0) // 플레이어가 이겼을 때
            {
                monsterClear(&mon, &jyb, sword, armor, shoes, gloves, cape, mask);
            }
            else if (jyb.hp <= 0) // 플레이어가 졌을 때
            {
                printf("체력 얼마 남기고 마을로 이동\n");
            }
            monsterExit(&mon);
            bAttack = bDepense = bAvoid = false;
            break;

        /* 보스 */
        case 75: // printf("바포매트 나와!!\n");
            BPMT = createBoss("바포매트", jyb.health * 5, 180, 450, 5, 700, 0.2, 0.3, 0.0, 0.2, 0.05, 0.0, 20);
            //printBossInfo(BPMT);
            boss = BPMT;
            round2 = false;
            while(1)
            {
                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", boss.name, boss.hp, boss.health);
                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                printf(">> 1 : 공격 / 2 : 도망가기\n");
                printf(">> 3: 빨간 포션 / 4. 주황 포션 / 5. 맑은 포션 / 6. 고농축 포션\n");
                printf(">> 7. 공격 히어로 주문서(수량: %d) / 8. 방어 히어로 주문서(수량: %d) / 9. 회피 히어로 주문서(수량: %d)\n", jyb.heroCnt[0], jyb.heroCnt[1], jyb.heroCnt[2]);
                while (boss.hp > 0 && jyb.hp > 0)
                {
                    printf(">> 입력 : ");
                    scanf("%d", &input);
                    printf("\n");
                    if (input >= 0 && input < 10)
                    {
                        switch (input)
                        {
                        case 1: // 공격
                            if (bAttack)
                            {
                                damage = jyb.power + (rand() % 10 + 1) + 10;
                                boss.hp -= damage;
                                printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                            }
                            damage = jyb.power + (rand() % 10 + 1) + 10;
                            boss.hp -= damage;
                            printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, boss.hp);
                            if (boss.hp > 0)
                            {
                                damage = rand() % (boss.maxDamage - boss.minDamage + 1) + boss.minDamage;
                                if (bDepense)
                                {
                                    damage = 0;
                                }
                                if(damage-jyb.depense<=0)
                                {
                                    damage = 1;
                                }
                                else{
                                    damage -= jyb.depense;
                                }
                                if(jyb.hp-damage < 0){
                                    jyb.hp = 0;
                                }else{
                                    jyb.hp -= damage;
                                }
                                printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);
                            }
                            if (jyb.hp <= 0)
                            {
                                printf("플레이어 사망\n");
                            }
                            break;
                        case 2: // 도망 50%
                            run = rand() % 2;
                            if (run == 1)
                            {
                                printf("도망 성공, 위치 이동 시키기\n");
                            }
                            else
                            {
                                damage = rand() % (boss.maxDamage - boss.minDamage + 1) + boss.minDamage;
                                if (bDepense)
                                {
                                    damage = 0;
                                }                            
                                if(damage-jyb.depense<=0)
                                {
                                    damage = 1;
                                }
                                else{
                                    damage -= jyb.depense;
                                }
                                if(jyb.hp-damage < 0){
                                    jyb.hp = 0;
                                }else{
                                    jyb.hp -= damage;
                                }
                                printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);

                                if (jyb.hp > 0)
                                {
                                    if (bAttack)
                                    {
                                        damage = jyb.power + (rand() % 10 + 1) + 10;
                                        boss.hp -= damage;
                                        printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, boss.hp);
                                    }
                                    damage = jyb.power + (rand() % 10 + 1) + 10;
                                    boss.hp -= damage;
                                    printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, boss.hp);
                                }
                                if (jyb.hp <= 0)
                                {
                                    printf("플레이어 사망\n");
                                }
                            }
                            break;
                        case 3: // 빨간 포션
                            if (jyb.cnt[0] >= 1)
                            {
                                printf("빨간 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[0] -= 1;
                                hpPlus(&red, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 4: // 주황 포션
                            if (jyb.cnt[1] >= 1)
                            {
                                printf("주황 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[1] -= 1;
                                hpPlus(&orange, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 5: // 맑은 포션
                            if (jyb.cnt[2] >= 1)
                            {
                                printf("맑은 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[2] -= 1;
                                hpPlus(&clear, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 6: // 고농축 포션
                            if (jyb.cnt[3] >= 1)
                            {
                                printf("고농축 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[3] -= 1;
                                hpPlus(&strong, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 7: // 공격 히어로
                            if (jyb.heroCnt[0] >= 1)
                            {
                                bAttack = true;
                                printf("공격 히어로를 소환했습니다\n");
                                printf("공격력이 두 배로 증가합니다.\n");
                            }
                            break;
                        case 8: // 방어 히어로
                            if (jyb.heroCnt[1] >= 1)
                            {
                                bDepense = true;
                                printf("방어 히어로를 소환했습니다.\n");
                                printf("데미지를 입지 않습니다.\n");
                            }
                            break;
                        case 9: // 회피 히어로
                            if (jyb.heroCnt[2] >= 1)
                            {
                                printf("회피 히어로를 소환했습니다.\n");
                                printf("리바이 병장이 입체기동장치를 사용하여 나를 데리고 도망칩니다.\n");
                                printf("위치 이동 시키기\n");
                            }
                            break;
                        }
                    }
                    else
                    {
                        printf("다시 입력하세요. \n");
                    }
                }
                if (boss.hp <= 0) // 플레이어가 이겼을 때
                {
                    bossClear(&boss, &jyb, sword, armor, shoes, gloves, cape, mask);
                }
                else if (jyb.hp <= 0) // 플레이어가 졌을 때
                {
                    printf("체력 얼마 남기고 마을로 이동\n");
                }
                bossExit(&boss);
                bAttack = bDepense = bAvoid = false;    
                if (rand()*100 +1 <= 0.05 * 100 && !round2)
                {                           
                    round2 = true;
                    int myFriend = rand() % 13;
                    Friends[myFriend].health = jyb.health*2;
                    Friends[myFriend].hp = Friends[myFriend].health;
                    boss = Friends[myFriend];
                    printf("5%%확률로 %s가 등장했습니다.\n", Friends[myFriend].name);
                    continue;
                }
                else
                {
                    break;
                }
            }
            break;
        case 76: // printf("이동녀크 나와!!\n");
            LDNK = createBoss("이동녀크", jyb.health * 7, 300, 550, 5, 1000, 0.0, 0.3, 0.0, 0.2, 0.1, 0.1, 60);
            //printBossInfo(LDNK);
            boss = LDNK;
            round2 = false;
            while(1)
            {
                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", boss.name, boss.hp, boss.health);
                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                printf(">> 1 : 공격 / 2 : 도망가기\n");
                printf(">> 3: 빨간 포션 / 4. 주황 포션 / 5. 맑은 포션 / 6. 고농축 포션\n");
                printf(">> 7. 공격 히어로 주문서(수량: %d) / 8. 방어 히어로 주문서(수량: %d) / 9. 회피 히어로 주문서(수량: %d)\n", jyb.heroCnt[0], jyb.heroCnt[1], jyb.heroCnt[2]);
                while (boss.hp > 0 && jyb.hp > 0)
                {
                    printf(">> 입력 : ");
                    scanf("%d", &input);
                    printf("\n");
                    if (input >= 0 && input < 10)
                    {
                        switch (input)
                        {
                        case 1: // 공격
                            if (bAttack)
                            {
                                damage = jyb.power + (rand() % 10 + 1) + 10;
                                boss.hp -= damage;
                                printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                            }
                            damage = jyb.power + (rand() % 10 + 1) + 10;
                            boss.hp -= damage;
                            printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, boss.hp);
                            if (boss.hp > 0)
                            {
                                damage = rand() % (boss.maxDamage - boss.minDamage + 1) + boss.minDamage;
                                if (bDepense)
                                {
                                    damage = 0;
                                }
                                if(damage-jyb.depense<=0)
                                {
                                    damage = 1;
                                }
                                else{
                                    damage -= jyb.depense;
                                }
                                if(jyb.hp-damage < 0){
                                    jyb.hp = 0;
                                }else{
                                    jyb.hp -= damage;
                                }
                                printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);
                            }
                            if (jyb.hp <= 0)
                            {
                                printf("플레이어 사망\n");
                            }
                            break;
                        case 2: // 도망 50%
                            run = rand() % 2;
                            if (run == 1)
                            {
                                printf("도망 성공, 위치 이동 시키기\n");
                            }
                            else
                            {
                                damage = rand() % (boss.maxDamage - boss.minDamage + 1) + boss.minDamage;
                                if (bDepense)
                                {
                                    damage = 0;
                                }                            
                                if(damage-jyb.depense<=0)
                                {
                                    damage = 1;
                                }
                                else{
                                    damage -= jyb.depense;
                                }
                                if(jyb.hp-damage < 0){
                                    jyb.hp = 0;
                                }else{
                                    jyb.hp -= damage;
                                }
                                printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);

                                if (jyb.hp > 0)
                                {
                                    if (bAttack)
                                    {
                                        damage = jyb.power + (rand() % 10 + 1) + 10;
                                        boss.hp -= damage;
                                        printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, boss.hp);
                                    }
                                    damage = jyb.power + (rand() % 10 + 1) + 10;
                                    boss.hp -= damage;
                                    printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, boss.hp);
                                }
                                if (jyb.hp <= 0)
                                {
                                    printf("플레이어 사망\n");
                                }
                            }
                            break;
                        case 3: // 빨간 포션
                            if (jyb.cnt[0] >= 1)
                            {
                                printf("빨간 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[0] -= 1;
                                hpPlus(&red, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 4: // 주황 포션
                            if (jyb.cnt[1] >= 1)
                            {
                                printf("주황 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[1] -= 1;
                                hpPlus(&orange, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 5: // 맑은 포션
                            if (jyb.cnt[2] >= 1)
                            {
                                printf("맑은 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[2] -= 1;
                                hpPlus(&clear, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 6: // 고농축 포션
                            if (jyb.cnt[3] >= 1)
                            {
                                printf("고농축 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[3] -= 1;
                                hpPlus(&strong, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 7: // 공격 히어로
                            if (jyb.heroCnt[0] >= 1)
                            {
                                bAttack = true;
                                printf("공격 히어로를 소환했습니다\n");
                                printf("공격력이 두 배로 증가합니다.\n");
                            }
                            break;
                        case 8: // 방어 히어로
                            if (jyb.heroCnt[1] >= 1)
                            {
                                bDepense = true;
                                printf("방어 히어로를 소환했습니다.\n");
                                printf("데미지를 입지 않습니다.\n");
                            }
                            break;
                        case 9: // 회피 히어로
                            if (jyb.heroCnt[2] >= 1)
                            {
                                printf("회피 히어로를 소환했습니다.\n");
                                printf("리바이 병장이 입체기동장치를 사용하여 나를 데리고 도망칩니다.\n");
                                printf("위치 이동 시키기\n");
                            }
                            break;
                        }
                    }
                    else
                    {
                        printf("다시 입력하세요. \n");
                    }
                }
                if (boss.hp <= 0) // 플레이어가 이겼을 때
                {
                    bossClear(&boss, &jyb, sword, armor, shoes, gloves, cape, mask);
                }
                else if (jyb.hp <= 0) // 플레이어가 졌을 때
                {
                    printf("체력 얼마 남기고 마을로 이동\n");
                }
                bossExit(&boss);
                bAttack = bDepense = bAvoid = false;    
                if (rand()*100 +1 <= 0.05 * 100 && !round2)
                {                           
                    round2 = true;
                    int myFriend = rand() % 13;
                    Friends[myFriend].health = jyb.health*2;
                    Friends[myFriend].hp = Friends[myFriend].health;
                    boss = Friends[myFriend];
                    printf("5%%확률로 %s가 등장했습니다.\n", Friends[myFriend].name);
                    continue;
                }
                else
                {
                    break;
                }
            }
            break;
        case 77: // printf("최상달 나와!!\n");
            CSD = createBoss("최상달", jyb.health * 10, 500, 1300, 5, 3000, 0.0, 0.3, 0.0, 0.3, 0.2, 0.2, 100);
            //printBossInfo(CSD);
            boss = CSD;
            round2 = false;
            while(1)
            {
                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", boss.name, boss.hp, boss.health);
                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                printf(">> 1 : 공격 / 2 : 도망가기\n");
                printf(">> 3: 빨간 포션 / 4. 주황 포션 / 5. 맑은 포션 / 6. 고농축 포션\n");
                printf(">> 7. 공격 히어로 주문서(수량: %d) / 8. 방어 히어로 주문서(수량: %d) / 9. 회피 히어로 주문서(수량: %d)\n", jyb.heroCnt[0], jyb.heroCnt[1], jyb.heroCnt[2]);
                while (boss.hp > 0 && jyb.hp > 0)
                {
                    printf(">> 입력 : ");
                    scanf("%d", &input);
                    printf("\n");
                    if (input >= 0 && input < 10)
                    {
                        switch (input)
                        {
                        case 1: // 공격
                            if (bAttack)
                            {
                                damage = jyb.power + (rand() % 10 + 1) + 10;
                                boss.hp -= damage;
                                printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                            }
                            damage = jyb.power + (rand() % 10 + 1) + 10;
                            boss.hp -= damage;
                            printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, boss.hp);
                            if (boss.hp > 0)
                            {
                                damage = rand() % (boss.maxDamage - boss.minDamage + 1) + boss.minDamage;
                                if (bDepense)
                                {
                                    damage = 0;
                                }
                                if(damage-jyb.depense<=0)
                                {
                                    damage = 1;
                                }
                                else{
                                    damage -= jyb.depense;
                                }
                                if(jyb.hp-damage < 0){
                                    jyb.hp = 0;
                                }else{
                                    jyb.hp -= damage;
                                }
                                printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);
                            }
                            if (jyb.hp <= 0)
                            {
                                printf("플레이어 사망\n");
                            }
                            break;
                        case 2: // 도망 50%
                            run = rand() % 2;
                            if (run == 1)
                            {
                                printf("도망 성공, 위치 이동 시키기\n");
                            }
                            else
                            {
                                damage = rand() % (boss.maxDamage - boss.minDamage + 1) + boss.minDamage;
                                if (bDepense)
                                {
                                    damage = 0;
                                }                            
                                if(damage-jyb.depense<=0)
                                {
                                    damage = 1;
                                }
                                else{
                                    damage -= jyb.depense;
                                }
                                if(jyb.hp-damage < 0){
                                    jyb.hp = 0;
                                }else{
                                    jyb.hp -= damage;
                                }
                                printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);

                                if (jyb.hp > 0)
                                {
                                    if (bAttack)
                                    {
                                        damage = jyb.power + (rand() % 10 + 1) + 10;
                                        boss.hp -= damage;
                                        printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, boss.hp);
                                    }
                                    damage = jyb.power + (rand() % 10 + 1) + 10;
                                    boss.hp -= damage;
                                    printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, boss.hp);
                                }
                                if (jyb.hp <= 0)
                                {
                                    printf("플레이어 사망\n");
                                }
                            }
                            break;
                        case 3: // 빨간 포션
                            if (jyb.cnt[0] >= 1)
                            {
                                printf("빨간 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[0] -= 1;
                                hpPlus(&red, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 4: // 주황 포션
                            if (jyb.cnt[1] >= 1)
                            {
                                printf("주황 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[1] -= 1;
                                hpPlus(&orange, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 5: // 맑은 포션
                            if (jyb.cnt[2] >= 1)
                            {
                                printf("맑은 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[2] -= 1;
                                hpPlus(&clear, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 6: // 고농축 포션
                            if (jyb.cnt[3] >= 1)
                            {
                                printf("고농축 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[3] -= 1;
                                hpPlus(&strong, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 7: // 공격 히어로
                            if (jyb.heroCnt[0] >= 1)
                            {
                                bAttack = true;
                                printf("공격 히어로를 소환했습니다\n");
                                printf("공격력이 두 배로 증가합니다.\n");
                            }
                            break;
                        case 8: // 방어 히어로
                            if (jyb.heroCnt[1] >= 1)
                            {
                                bDepense = true;
                                printf("방어 히어로를 소환했습니다.\n");
                                printf("데미지를 입지 않습니다.\n");
                            }
                            break;
                        case 9: // 회피 히어로
                            if (jyb.heroCnt[2] >= 1)
                            {
                                printf("회피 히어로를 소환했습니다.\n");
                                printf("리바이 병장이 입체기동장치를 사용하여 나를 데리고 도망칩니다.\n");
                                printf("위치 이동 시키기\n");
                            }
                            break;
                        }
                    }
                    else
                    {
                        printf("다시 입력하세요. \n");
                    }
                }
                if (boss.hp <= 0) // 플레이어가 이겼을 때
                {
                    bossClear(&boss, &jyb, sword, armor, shoes, gloves, cape, mask);
                }
                else if (jyb.hp <= 0) // 플레이어가 졌을 때
                {
                    printf("체력 얼마 남기고 마을로 이동\n");
                }
                bossExit(&boss);
                bAttack = bDepense = bAvoid = false;    
                if (rand()*100 +1 <= 0.05 * 100 && !round2)
                {                           
                    round2 = true;
                    int myFriend = rand() % 13;
                    Friends[myFriend].health = jyb.health*2;
                    Friends[myFriend].hp = Friends[myFriend].health;
                    boss = Friends[myFriend];
                    printf("5%%확률로 %s가 등장했습니다.\n", Friends[myFriend].name);
                    continue;
                }
                else
                {
                    break;
                }
            }
            break;
        case 78:
            int myFriend = rand() % 13;
            Friends[myFriend].health = jyb.health*2;
            Friends[myFriend].hp = Friends[myFriend].health;
            boss = Friends[myFriend];
            printf("%s 나와!!\n", Friends[myFriend].name);
            round2 = false;
            while(1)
            {
                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", boss.name, boss.hp, boss.health);
                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                printf(">> 1 : 공격 / 2 : 도망가기\n");
                printf(">> 3: 빨간 포션 / 4. 주황 포션 / 5. 맑은 포션 / 6. 고농축 포션\n");
                printf(">> 7. 공격 히어로 주문서(수량: %d) / 8. 방어 히어로 주문서(수량: %d) / 9. 회피 히어로 주문서(수량: %d)\n", jyb.heroCnt[0], jyb.heroCnt[1], jyb.heroCnt[2]);
                while (boss.hp > 0 && jyb.hp > 0)
                {
                    printf(">> 입력 : ");
                    scanf("%d", &input);
                    printf("\n");
                    if (input >= 0 && input < 10)
                    {
                        switch (input)
                        {
                        case 1: // 공격
                            if (bAttack)
                            {
                                damage = jyb.power + (rand() % 10 + 1) + 10;
                                boss.hp -= damage;
                                printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, mon.hp);
                            }
                            damage = jyb.power + (rand() % 10 + 1) + 10;
                            boss.hp -= damage;
                            printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, boss.hp);
                            if (boss.hp > 0)
                            {
                                damage = rand() % (boss.maxDamage - boss.minDamage + 1) + boss.minDamage;
                                if (bDepense)
                                {
                                    damage = 0;
                                }
                                if(damage-jyb.depense<=0)
                                {
                                    damage = 1;
                                }
                                else{
                                    damage -= jyb.depense;
                                }
                                if(jyb.hp-damage < 0){
                                    jyb.hp = 0;
                                }else{
                                    jyb.hp -= damage;
                                }
                                printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);
                            }
                            if (jyb.hp <= 0)
                            {
                                printf("플레이어 사망\n");
                            }
                            break;
                        case 2: // 도망 50%
                            run = rand() % 2;
                            if (run == 1)
                            {
                                printf("도망 성공, 위치 이동 시키기\n");
                            }
                            else
                            {
                                damage = rand() % (boss.maxDamage - boss.minDamage + 1) + boss.minDamage;
                                if (bDepense)
                                {
                                    damage = 0;
                                }                            
                                if(damage-jyb.depense<=0)
                                {
                                    damage = 1;
                                }
                                else{
                                    damage -= jyb.depense;
                                }
                                if(jyb.hp-damage < 0){
                                    jyb.hp = 0;
                                }else{
                                    jyb.hp -= damage;
                                }
                                printf("몬스터에게 데미지 %d를 입었습니다. (남은 체력: %.2lf)\n", damage, jyb.hp);

                                if (jyb.hp > 0)
                                {
                                    if (bAttack)
                                    {
                                        damage = jyb.power + (rand() % 10 + 1) + 10;
                                        boss.hp -= damage;
                                        printf("공격형 히어로가 몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, boss.hp);
                                    }
                                    damage = jyb.power + (rand() % 10 + 1) + 10;
                                    boss.hp -= damage;
                                    printf("몬스터에게 데미지 %d를 입혔습니다. (남은 체력: %.2lf)\n", damage, boss.hp);
                                }
                                if (jyb.hp <= 0)
                                {
                                    printf("플레이어 사망\n");
                                }
                            }
                            break;
                        case 3: // 빨간 포션
                            if (jyb.cnt[0] >= 1)
                            {
                                printf("빨간 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[0] -= 1;
                                hpPlus(&red, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 4: // 주황 포션
                            if (jyb.cnt[1] >= 1)
                            {
                                printf("주황 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[1] -= 1;
                                hpPlus(&orange, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 5: // 맑은 포션
                            if (jyb.cnt[2] >= 1)
                            {
                                printf("맑은 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[2] -= 1;
                                hpPlus(&clear, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 6: // 고농축 포션
                            if (jyb.cnt[3] >= 1)
                            {
                                printf("고농축 포션을 사용합니다.\n");
                                printf("[ %s ]\n체력 : %.2lf(/%.2lf)\n", jyb.name, jyb.hp, jyb.health);
                                jyb.cnt[3] -= 1;
                                hpPlus(&strong, &jyb, 1);
                            }
                            printf("///체력 리셋///\n");
                            break;
                        case 7: // 공격 히어로
                            if (jyb.heroCnt[0] >= 1)
                            {
                                bAttack = true;
                                printf("공격 히어로를 소환했습니다\n");
                                printf("공격력이 두 배로 증가합니다.\n");
                            }
                            break;
                        case 8: // 방어 히어로
                            if (jyb.heroCnt[1] >= 1)
                            {
                                bDepense = true;
                                printf("방어 히어로를 소환했습니다.\n");
                                printf("데미지를 입지 않습니다.\n");
                            }
                            break;
                        case 9: // 회피 히어로
                            if (jyb.heroCnt[2] >= 1)
                            {
                                printf("회피 히어로를 소환했습니다.\n");
                                printf("리바이 병장이 입체기동장치를 사용하여 나를 데리고 도망칩니다.\n");
                                printf("위치 이동 시키기\n");
                            }
                            break;
                        }
                    }
                    else
                    {
                        printf("다시 입력하세요. \n");
                    }
                }
                if (boss.hp <= 0) // 플레이어가 이겼을 때
                {
                    bossClear(&boss, &jyb, sword, armor, shoes, gloves, cape, mask);
                }
                else if (jyb.hp <= 0) // 플레이어가 졌을 때
                {
                    printf("체력 얼마 남기고 마을로 이동\n");
                }
                bossExit(&boss);
                bAttack = bDepense = bAvoid = false;    
                if (rand()*100 +1 <= 0.05 * 100 && !round2)
                {                           
                    round2 = true;
                    int myFriend = rand() % 13;
                    Friends[myFriend].health = jyb.health*2;
                    Friends[myFriend].hp = Friends[myFriend].health;
                    boss = Friends[myFriend];
                    printf("5%%확률로 %s가 등장했습니다.\n", Friends[myFriend].name);
                    continue;
                }
                else
                {
                    break;
                }
            }
            break;
        default:
            break;
        }
        while (getchar() != '\n')
            ;
        calcPlayer(&jyb, playerHP, equipEnchant);
        printPlayerInfo(jyb, equipEnchant);
    }

    // printf("////드워프의 제련소////\n");
    // while(1)
    // {
    //     int buyCnt;
    //     while(1)
    //     {
    //         printf("장비 강화 주문서를 몇 개 구입하시겠습니까? : ");
    //         scanf("%d", &buyCnt);
    //         if (buyCnt < 0)
    //         {
    //             printf("\n다시 입력하세요.\n");
    //             continue;
    //         }
    //         else if (buyCnt * scrollPrice > jyb.gold)
    //         {
    //             printf("%d골드가 부족합니다.\n",buyCnt*scrollPrice);
    //             continue;
    //         }
    //         else if ( buyCnt > 0 && buyCnt * scrollPrice <= jyb.gold)
    //         {
    //             printf("구매가 완료되었습니다.\n");
    //             jyb.cnt[6] = jyb.cnt[6] + buyCnt;
    //             jyb.gold = jyb.gold - buyCnt * scrollPrice;
    //             printf("<구매 후> 수량: %d, 골드 : %d\n", jyb.cnt[6], jyb.gold);
    //             break;
    //         }
    //         else
    //         {
    //             break;
    //         }
    //     }
    // }

    // 강화 테스트
    // while(1){
    //     for (int i = 1; i <= enchantScroll; i++)
    //     {
    //         int enchantIdx;
    //         while (1)
    //         {
    //             printf("0:무기 1:갑옷 2:신발 3:장갑 4:망토 5:마스크\n입력: ");
    //             scanf("%d", &enchantIdx);
    //             if (enchantIdx == 0)
    //             {
    //                 enchantEquipment(&sword[jyb.swordIdx], &jyb, enchantIdx, equipEnchant, false);
    //                 break;
    //             }
    //             else if (enchantIdx == 1)
    //             {
    //                 enchantEquipment(&armor[jyb.armorIdx], &jyb, enchantIdx, equipEnchant, false);
    //                 break;
    //             }
    //             else if (enchantIdx == 2)
    //             {
    //                 enchantEquipment(&shoes[jyb.shoesIdx], &jyb, enchantIdx, equipEnchant, false);
    //                 break;
    //             }
    //             else if (enchantIdx == 3)
    //             {
    //                 enchantEquipment(&gloves[jyb.glovesIdx], &jyb, enchantIdx, equipEnchant, false);
    //                 break;
    //             }
    //             else if (enchantIdx == 4)
    //             {
    //                 enchantEquipment(&cape[jyb.capeIdx], &jyb, enchantIdx, equipEnchant, false);
    //                 break;
    //             }
    //             else if (enchantIdx == 5)
    //             {
    //                 enchantEquipment(&mask[jyb.maskIdx], &jyb, enchantIdx, equipEnchant, false);
    //                 break;
    //             }
    //             else
    //             {
    //                 printf("다시 입력하세요.\n"); // 종료하려면 q를 입력하세요 추가하기
    //                 continue;
    //             }
    //         }
    //         while (getchar() != '\n')
    //             ;
    //         calcPlayer(&jyb, playerHP, equipEnchant);
    //         printPlayerInfo(jyb, equipEnchant);
    //     }
    // }

    // 물약 복용 테스트
    // while (1)
    // {
    //     int potionCnt;
    //     jyb.hp = 150;
    //     printf("\n용복이 체력 : %.2lf", jyb.hp);
    //     while (red.count != 0)
    //     {
    //         printf("\n 빨간 포션 증가량 : %d, 포션 개수 : %d\n 몇 개 사용하시겠습니까? : ", red.plus, red.count);
    //         scanf("%d", &potionCnt);
    //         if (potionCnt > red.count)
    //         {
    //             printf("\n사용할 포션이 부족합니다. %d개만 사용됩니다.", red.count);
    //             potionCnt = red.count;
    //         }
    //         hpPlus(&red, &jyb, potionCnt);
    //         printf("\n용복이 체력 : %.2lf", jyb.hp);
    //     }
    //     printf("\n-----------------------------------------------------");
    //     jyb.hp = 150;
    //     printf("\n용복이 체력 : %.2lf", jyb.hp);
    //     while (orange.count != 0)
    //     {
    //         printf("\n주황 포션 증가량 : %d, 포션 개수 : %d\n 몇 개 사용하시겠습니까? : ", orange.plus, orange.count);
    //         scanf("%d", &potionCnt);
    //         if (potionCnt > orange.count)
    //         {
    //             printf("\n사용할 포션이 부족합니다. %d개만 사용됩니다.", orange.count);
    //             potionCnt = orange.count;
    //         }
    //         hpPlus(&orange, &jyb, potionCnt);
    //         printf("\n용복이 체력 : %.2lf", jyb.hp);
    //     }
    //     printf("\n-----------------------------------------------------");
    //     jyb.hp = 150;
    //     printf("\n용복이 체력 : %.2lf", jyb.hp);
    //     while (clear.count != 0)
    //     {
    //         printf("\n 맑은 포션 증가량 : %d, 포션 개수 : %d\n 몇 개 사용하시겠습니까? : ", clear.plus, clear.count);
    //         scanf("%d", &potionCnt);
    //         if (potionCnt > clear.count)
    //         {
    //             printf("\n사용할 포션이 부족합니다. %d개만 사용됩니다.", clear.count);
    //             potionCnt = clear.count;
    //         }
    //         hpPlus(&clear, &jyb, potionCnt);
    //         printf("\n용복이 체력 : %.2lf", jyb.hp);
    //     }
    //     printf("\n-----------------------------------------------------");
    //     jyb.hp = 150;
    //     printf("\n용복이 체력 : %.2lf", jyb.hp);
    //     while (strong.count != 0)
    //     {
    //         printf("\n 고농축 포션 증가량 : %d, 포션 개수 : %d\n 몇 개 사용하시겠습니까? : ", strong.plus, strong.count);
    //         scanf("%d", &potionCnt);
    //         if (potionCnt > strong.count)
    //         {
    //             printf("\n사용할 포션이 부족합니다. %d개만 사용됩니다.", strong.count);
    //             potionCnt = strong.count;
    //         }
    //         hpPlus(&strong, &jyb, potionCnt);
    //         printf("\n용복이 체력 : %.2lf", jyb.hp);
    //     }
    //     break;
    // }

    // printf("////판도라의 상점////\n");
    // while (1)
    // {
    //     printf(" 1  %s (%d골드)\n", red.name, red.price);
    //     printf(" 2  %s (%d골드)\n", orange.name, orange.price);
    //     printf(" 3  %s (%d골드)\n", clear.name, clear.price);
    //     printf(" 4  %s (%d골드)\n", strong.name, strong.price);
    //     printf(" 5  %s (%d골드)\n", sword[1].name, tier2Price);
    //     printf(" 6  %s (%d골드)\n", armor[1].name, tier2Price);
    //     printf(" 7  %s (%d골드)\n", shoes[1].name, tier2Price);
    //     printf(" 8  %s (%d골드)\n", gloves[1].name, tier2Price);
    //     printf(" 9  %s (%d골드)\n", cape[1].name, tier2Price);
    //     printf(" 10 %s (%d골드)\n", mask[1].name, tier2Price);
    //     printf(" 11 마을 귀환 주문서 (%d골드)\n", scrollPrice);
    //     printf("------------------------------\n");
    //     int buyIdx, buyCnt;
    //     int potionPrice[4] = {red.price, orange.price, clear.price, strong.price};
    //     printf("구매할 아이템의 번호를 입력하세요 (종료하려면 q를 입력하세요.) : ");
    //     scanf("%d", &buyIdx);
    //     if (0 < buyIdx && buyIdx < 5)
    //     {
    //         while (1)
    //         {
    //             printf("<구매 전> 수량: %d, 골드 : %d\n", jyb.cnt[buyIdx], jyb.gold);
    //             printf("구매할 수량을 입력하세요 : ");
    //             scanf("%d", &buyCnt);
    //             if (buyCnt < 0)
    //             {
    //                 printf("\n다시 입력하세요.\n");
    //                 continue;
    //             }
    //             else if (buyCnt * potionPrice[buyIdx - 1] > jyb.gold)
    //             {
    //                 printf("골드가 부족합니다.\n");
    //                 continue;
    //             }
    //             else if (buyCnt * potionPrice[buyIdx - 1] <= jyb.gold && buyCnt > 0)
    //             {
    //                 printf("구매가 완료되었습니다.\n");
    //                 jyb.cnt[buyIdx] = jyb.cnt[buyIdx] + buyCnt;
    //                 jyb.gold = jyb.gold - buyCnt * potionPrice[buyIdx - 1];
    //                 printf("<구매 후> 수량: %d, 골드 : %d\n", jyb.cnt[buyIdx], jyb.gold);
    //                 break;
    //             }
    //             else
    //             {
    //                 break;
    //             }
    //         }
    //     }
    //     else if (buyIdx == 5) // 무기 구매
    //     {
    //         if (jyb.swordIdx > 0)
    //         {
    //             printf("현재 착용하고 있는 장비 티어가 구매하려는 장비보다 같거나 높습니다.\n구매를 종료합니다.");
    //             break;
    //         }
    //         else
    //         {
    //             if (tier2Price > jyb.gold)
    //             {
    //                 printf("골드가 부족합니다.\n");
    //                 break;
    //             }
    //             else
    //             {
    //                 calcPlayer(&jyb, playerHP, equipEnchant);
    //                 printPlayerInfo(jyb, equipEnchant);
    //                 printf("<구매 전> 티어: %d, 골드 : %d\n", jyb.swordIdx, jyb.gold);
    //                 printf("구매가 완료되었습니다.\n");
    //                 // 장비 교체 Player Idx 변경, equipEnchant 초기화
    //                 jyb.swordIdx += 1;
    //                 equipEnchant[0] = 1.0;
    //                 jyb.gold = jyb.gold - tier2Price;
    //                 printf("<구매 후> 티어: %d, 골드 : %d\n", jyb.swordIdx, jyb.gold);
    //                 calcPlayer(&jyb, playerHP, equipEnchant);
    //                 break;
    //             }
    //         }
    //     }
    //     else if (buyIdx == 6) // 갑옷 구매
    //     {
    //         if (jyb.armorIdx > 0)
    //         {
    //             printf("현재 착용하고 있는 장비 티어가 구매하려는 장비보다 같거나 높습니다.\n구매를 종료합니다.");
    //             break;
    //         }
    //         else
    //         {
    //             if (tier2Price > jyb.gold)
    //             {
    //                 printf("골드가 부족합니다.\n");
    //                 break;
    //             }
    //             else
    //             {
    //                 calcPlayer(&jyb, playerHP, equipEnchant);
    //                 printPlayerInfo(jyb, equipEnchant);
    //                 printf("<구매 전> 티어: %d, 골드 : %d\n", jyb.armorIdx, jyb.gold);
    //                 printf("구매가 완료되었습니다.\n");
    //                 // 장비 교체 Player Idx 변경, equipEnchant 초기화
    //                 jyb.armorIdx += 1;
    //                 equipEnchant[1] = 1.0;
    //                 jyb.gold = jyb.gold - tier2Price;
    //                 printf("<구매 후> 티어: %d, 골드 : %d\n", jyb.armorIdx, jyb.gold);
    //                 calcPlayer(&jyb, playerHP, equipEnchant);
    //                 break;
    //             }
    //         }
    //     }
    //     else if (buyIdx == 7) // 신발 구매
    //     {
    //         if (jyb.shoesIdx > 0)
    //         {
    //             printf("현재 착용하고 있는 장비 티어가 구매하려는 장비보다 같거나 높습니다.\n구매를 종료합니다.");
    //             break;
    //         }
    //         else
    //         {
    //             if (tier2Price > jyb.gold)
    //             {
    //                 printf("골드가 부족합니다.\n");
    //                 break;
    //             }
    //             else
    //             {
    //                 calcPlayer(&jyb, playerHP, equipEnchant);
    //                 printPlayerInfo(jyb, equipEnchant);
    //                 printf("<구매 전> 티어: %d, 골드 : %d\n", jyb.shoesIdx, jyb.gold);
    //                 printf("구매가 완료되었습니다.\n");
    //                 // 장비 교체 Player Idx 변경, equipEnchant 초기화
    //                 jyb.shoesIdx += 1;
    //                 equipEnchant[2] = 1.0;
    //                 jyb.gold = jyb.gold - tier2Price;
    //                 printf("<구매 후> 티어: %d, 골드 : %d\n", jyb.shoesIdx, jyb.gold);
    //                 calcPlayer(&jyb, playerHP, equipEnchant);
    //                 break;
    //             }
    //         }
    //     }
    //     else if (buyIdx == 8) // 장갑 구매
    //     {
    //         if (jyb.glovesIdx > 0)
    //         {
    //             printf("현재 착용하고 있는 장비 티어가 구매하려는 장비보다 같거나 높습니다.\n구매를 종료합니다.");
    //             break;
    //         }
    //         else
    //         {
    //             if (tier2Price > jyb.gold)
    //             {
    //                 printf("골드가 부족합니다.\n");
    //                 break;
    //             }
    //             else
    //             {
    //                 calcPlayer(&jyb, playerHP, equipEnchant);
    //                 printPlayerInfo(jyb, equipEnchant);
    //                 printf("<구매 전> 티어: %d, 골드 : %d\n", jyb.glovesIdx, jyb.gold);
    //                 printf("구매가 완료되었습니다.\n");
    //                 // 장비 교체 Player Idx 변경, equipEnchant 초기화
    //                 jyb.swordIdx += 1;
    //                 equipEnchant[0] = 1.0;
    //                 jyb.gold = jyb.gold - tier2Price;
    //                 printf("<구매 후> 티어: %d, 골드 : %d\n", jyb.glovesIdx, jyb.gold);
    //                 calcPlayer(&jyb, playerHP, equipEnchant);
    //                 break;
    //             }
    //         }
    //     }
    //     else if (buyIdx == 9) // 망토 구매
    //     {
    //         if (jyb.capeIdx > 0)
    //         {
    //             printf("현재 착용하고 있는 장비 티어가 구매하려는 장비보다 같거나 높습니다.\n구매를 종료합니다.");
    //             break;
    //         }
    //         else
    //         {
    //             if (tier2Price > jyb.gold)
    //             {
    //                 printf("골드가 부족합니다.\n");
    //                 break;
    //             }
    //             else
    //             {
    //                 calcPlayer(&jyb, playerHP, equipEnchant);
    //                 printPlayerInfo(jyb, equipEnchant);
    //                 printf("<구매 전> 티어: %d, 골드 : %d\n", jyb.capeIdx, jyb.gold);
    //                 printf("구매가 완료되었습니다.\n");
    //                 // 장비 교체 Player Idx 변경, equipEnchant 초기화
    //                 jyb.capeIdx += 1;
    //                 equipEnchant[0] = 1.0;
    //                 jyb.gold = jyb.gold - tier2Price;
    //                 printf("<구매 후> 티어: %d, 골드 : %d\n", jyb.capeIdx, jyb.gold);
    //                 calcPlayer(&jyb, playerHP, equipEnchant);
    //                 break;
    //             }
    //         }
    //     }
    //     else if (buyIdx == 10) // 마스크 구매
    //     {
    //         if (jyb.swordIdx > 0)
    //         {
    //             printf("현재 착용하고 있는 장비 티어가 구매하려는 장비보다 같거나 높습니다.\n구매를 종료합니다.");
    //             break;
    //         }
    //         else
    //         {
    //             if (tier2Price > jyb.gold)
    //             {
    //                 printf("골드가 부족합니다.\n");
    //                 break;
    //             }
    //             else
    //             {
    //                 calcPlayer(&jyb, playerHP, equipEnchant);
    //                 printPlayerInfo(jyb, equipEnchant);
    //                 printf("<구매 전> 티어: %d, 골드 : %d\n", jyb.swordIdx, jyb.gold);
    //                 printf("구매가 완료되었습니다.\n");
    //                 // 장비 교체 Player Idx 변경, equipEnchant 초기화
    //                 jyb.swordIdx += 1;
    //                 equipEnchant[0] = 1.0;
    //                 jyb.gold = jyb.gold - tier2Price;
    //                 printf("<구매 후> 티어: %d, 골드 : %d\n", jyb.swordIdx, jyb.gold);
    //                 calcPlayer(&jyb, playerHP, equipEnchant);
    //                 printPlayerInfo(jyb, equipEnchant);
    //                 break;
    //             }
    //         }
    //     }
    //     else if (buyIdx == 11) // 주문서
    //     {
    //         printf("구매 전 : 마을 귀환 주문서를 %d개 가지고 있습니다.\n", jyb.cnt[4]);
    //         if (scrollPrice > jyb.gold)
    //         {
    //             printf("골드가 부족합니다.\n");
    //             break;
    //         }
    //         else
    //         {
    //             jyb.cnt[4] += 1;
    //             printf("구매가 완료되었습니다. 마을 귀환 주문서를 %d개 가지고 있습니다.\n", jyb.cnt[4]);
    //         }
    //     }
    //     else
    //     {
    //         printf("\n다시 입력하세요.\n");
    //         continue;
    //     }
    //     break;
    // }

    // 펫상점
    // while (1)
    // {
    //     Pet p;
    //     int petIdx;
    //     printf("------------------------------\n");
    //     printf("1. %-4s, 능력: 공격력+2  \n", petName[0]);
    //     printf("2. %-4s, 능력: 데미지감소-2  \n", petName[1]);
    //     printf("------------------------------\n");
    //     printf("펫을 선택하세요 : ");
    //     scanf("%d", &petIdx);
    //     printf("\n------------------------------");
    //     if (petIdx == 1 || petIdx == 2)
    //     {
    //         jyb.pet.name = petName[petIdx - 1];
    //         jyb.pet.skill = 2;
    //         printf("\n");
    //         calcPlayer(&jyb, playerHP, equipEnchant);
    //         printPlayerInfo(jyb, equipEnchant);
    //         break;
    //     }
    //     else
    //     {
    //         printf("다시 입력하세요.\n");
    //         continue;
    //     }
    // }

    // 야매레벨법사 테스트
    // while (1)
    // {
    //     printf("\n1에서 10사이로 레벨이 설정됩니다. 진행하시겠습니까?");
    //     int lv;
    //     printf(" (현재 레벨: %d)\n---------------------------------------------------------------\n", jyb.level);
    //     lv = rand() % 10 + 1;
    //     printf("우하하하 이제부터 너의 레벨은 %d!!!!\n---------------------------------------------------------------\n", lv);
    //     jyb.level = lv;
    //     printf("현재 레벨: %d\n", jyb.level);
    //     break;
    // }

    return 0;
}
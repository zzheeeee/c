import random

start = 0
while True:
    print()

    if(start == 0):
        userWin = 0
        userLose = 0

    while start == 0 :
        chance = 0
        level = input("난이도를 선택하세요(1:상, 2:중, 3:하) : ")
        if level.isdigit() and 0 < int(level) < 4:
            if int(level) == 1:
                chance = 10
            elif int(level) == 2:
                chance = 15
            else:
                chance = 20
            start = 1
        else:
            print("다시 입력하세요")

    while userWin != 0 or userLose != 0:
        retry = input("계속하려면 0을 입력하세요.")
        if (int(retry) != 0):
            continue
        else:
            break

    hitScore = []
    while True:
        r = random.randint(0, 9)
        if len(hitScore) == 3:
            break
        else:
            if r not in hitScore:
                hitScore.append(r)

    print("{:^25}".format(f"{hitScore}"))
    print('-' * 25)
    userScore = [-1, -1, -1]
    round = 1
    last = chance
    while 0<last:
        print('중도 포기하려면 gg를 입력하세요')
        # print(f"입력 기회 :", last)
        print('-' * 25)
        print("{:^25}".format(f"[ {round} Round ]"))
        print("{:^25}".format(f"** 입력 기회:{last} **"))
        print(' ' * 25)

        userCnt = 0

        userScore = [-1, -1, -1]
        strike = 0
        ball = 0
        out = 0

        while True:
            user = input(' Enter number 0 ~ 9 :  ')
            if user == "gg":
                print('중도 포기하셨습니다')
                last = 0
                break
            elif int(user) in userScore:
                print('중복 값은 입력할 수 없습니다.')
                continue
            elif not user.isdigit() or not (-1 < int(user) < 10):
                print('입력 오류 입니다.')
                continue
            else:
                userScore[userCnt] = int(user)
                if userCnt + 1 < 3:
                    userCnt = userCnt + 1
                    continue
                else:
                    break

        for ss in range(0, 3):
            if userScore[ss] == hitScore[ss]:
                strike = strike + 1
            elif userScore[ss] in hitScore:
                ball = ball + 1

        out = 3 - strike - ball

        print(' ' * 25)
        if out == 3:
            print("{:^25}".format(f"[ Out ]"))
        else:
            print("{:^25}".format(f"[ Strike: {strike}, Ball: {ball} ]"))
        print('-' * 25)

        if strike == 3:
            print('=' * 9, " win ", '=' * 9)
            print('승리하셨습니다.')
            userWin+=1
            break

        if round < chance:
            round = round + 1
            if(0 >= last-1):
                print('패배하셨습니다.')
                userLose += 1
                break
            else:
                last -= 1
            continue
        else:
            break

    print(f"승리: {userWin}, 패배: {userLose}")

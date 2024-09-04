#ifndef INFO_H
#define INFO_H

#include "my.h"

#define MAX_USER_NUM    5
#define MAX_NICK_LEN    30
#define    MAX_MESSAGE_LEN    1024

#define SERV_IP    "127.0.0.1"
#define SERV_PORT    10200

typedef struct _user{
    int usd;
    int unum;
    int rsd;
    char unick[MAX_NICK_LEN];
}User;

typedef struct _room{
    int rsd;
    int usd[5];
    int ucnt;
}Room;

#endif
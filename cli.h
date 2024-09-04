#ifndef CLI_H
#define CLI_H

#include "info.h"

User list[5];

int sd;
int rsd;
int flag = 0;
int rcnt = 0;
int usernum = 0;
pthread_mutex_t usermutex;

void SigExit(int signo);
int SockSetting(char *ip, int port);
void JoinChat(int ssd);
void *RecvMsg(void *user);
void *SendMsg(void *user);
void *thrdmain(void *us);

#endif
/*************************************************************************
	> File Name: common.h
	> Author: 
	> Mail: 
	> Created Time: Sat 11 Sep 2021 02:04:40 PM CST
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H
int make_nonblock(int fd);
int make_block(int fd);
int socket_create(int port);
int socket_connect(const char *ip, int port);
#ifdef _D
#define DBG(fmt, args...); printf(fmt, ##args);
#else
#define DBG(fmt, args...)
#endif
#endif

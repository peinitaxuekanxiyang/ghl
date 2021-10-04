/*************************************************************************
	> File Name: pihealth.h
	> Author: 
	> Mail: 
	> Created Time: Fri 10 Sep 2021 09:07:03 PM CST
 ************************************************************************/

#ifndef _PIHEALTH_H
#define _PIHEALTH_H

#define PI_HEART 0X01
#define PI_ACK 0X02
#define PI_FIN 0X04

struct pihealth_msg_ds{
    int type;
};


struct client_ds{
    int sockfd;
    int isonline;
    struct sockaddr_in addr;
};

void *work_on_reactor(void *arg);
void heart_beat(int signum);
void *do_login(void *arg);
#endif

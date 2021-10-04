/*************************************************************************
	> File Name: server_fun.c
	> Author: 
	> Mail: 
	> Created Time: Fri 10 Sep 2021 08:36:24 PM CST
 ************************************************************************/
#include"head.h"
#define MAXEVENTS 5

extern char token[100];
extern struct client_ds *clients;
extern int epollfd, max;

void *work_on_reactor(void *arg){
    struct epoll_event ev, events[MAXEVENTS];
    for(;;){
        int nfds = epoll_wait(epollfd, events, MAXEVENTS, -1);
        if(nfds <= 0){
            perror("epoll_wait");
            exit(1);
        }
        for(int i = 0; i < nfds; i++){
            struct pihealth_msg_ds msg;
            bzero(&msg, sizeof(msg));
            int sockfd = events[i].data.fd;
            int ret = recv(events[i].data.fd, (void *)&msg, sizeof(msg), 0);
            if(ret < 0){
                epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                close(sockfd);
                clients[sockfd].isonline = -1;
                continue;
            }
            if(msg.type & PI_ACK){
                //DBG(GREEN"<ack> "NONE" ack recved from %s!\n",inet_ntoa(clients[sockfd].addr.sin_addr);
                clients[sockfd].isonline = 10;
            }
        }
    }
}

void heart_beat(int signum){
    struct pihealth_msg_ds msg;
    msg.type = PI_HEART;
    for(int i = 0; i < max; i++){
        if(clients[i].isonline){
            send(clients[i].sockfd, (void *)&msg, sizeof(msg), 0);
            //不符合的剔除
            if(--clients[i].isonline == 0){
                epoll_ctl(epollfd, EPOLL_CTL_DEL, clients[i].sockfd, NULL);
                close(clients[i].sockfd);  
                DBG(PINK"<Heartfailed> : "NONE" %s is removed from list!\n", inet_ntoa(clients[i].addr.sin_addr));
            }
        }
    }
}

int check_online(struct sockaddr_in *addr){
    int flag = 0;
    for(int i = 0; i < max; i++){
        if(addr->sin_addr.s_addr == clients[i].addr.sin_addr.s_addr){
            flag == 1;
            break;
        }
    }
    return flag;
}

void *do_login(void *arg){
    int server_listen, sockfd;
    server_listen = *(int *)arg;
    while(1){
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        sockfd = accept(server_listen, (struct sockaddr *)&client, &len);
        if(sockfd < 0){
            perror("accept");
            exit(1);
        }
        char token_tmp[100] = {0};

        //客户端如果连上后不说话，就拜拜
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 10000;

        if((select(sockfd + 1, &rfds, NULL, NULL, &tv)) <= 0){
            close(sockfd);
            DBG(RED"<Error> : "NONE"time out from token!\n");
            continue;
        }


        if(recv(sockfd, token_tmp, sizeof(token_tmp), 0) < 0){
            close(sockfd);
            continue;
        }
        if(strcmp(token_tmp, token)){
            close(sockfd);
            DBG(RED"<Error> : "NONE" TOKEN Error!\n");
            continue;
        }

        //判断是否已经在线
        int ack = 1;  //ack=0是在线，ack=1不在线
        if(check_online(&client)){
            ack = 0;
        }

        //客户端发来token后，服务端回一下ack=1
        if(send(sockfd, (void *)&ack, sizeof(int), 0) < 0){
            close(sockfd);
            DBG(RED"<Error> : "NONE"send ack error!\n");
            continue;
        }

        if(ack == 0){
            close(sockfd);
            DBG(YELLOW"<waring> : "NONE"%s already login!\n", inet_ntoa(client.s_addr));
            continue;
        }

        //已经连接上来的客户端存放到clients数组
        clients[sockfd].sockfd = sockfd;
        clients[sockfd].isonline = 10;
        clients[sockfd].addr = client;

        //注册到epoll
        struct epoll_event ev;
        ev.data.fd = sockfd;
        ev.events = EPOLLIN;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);
        DBG(GREEN"<Reactor> : "NONE"add client to reactor!\n");
        //sockfd其实就是客户
    }
}





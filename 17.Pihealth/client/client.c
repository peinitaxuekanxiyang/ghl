/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: Sat 11 Sep 2021 04:00:58 PM CST
 ************************************************************************/
#include "head.h"
char server_ip[20];
int server_port;
char token[100];
char *config = "./pihealthd.conf";

int main(int argc, char **argv){
    int sockfd, opt;
    while((opt = getopt(argc, argv, "s:p:t:")) != -1){
        switch(opt){
            case 'p':
                server_port = atoi(optarg);
                break;
            case 's':
                strcpy(server_ip, optarg);
                break;
            case 't':
                strcpy(token, optarg);
                break;
            default:
                fprintf(stderr, "Usage : -p port!\n", argv[0]);
                exit(1);
        }
    }

    if(!server_port) server_port = atoi(get_conf_value(config, "SERVER_PORT"));
    if(!strlen(server_ip)) strcpy(server_ip, get_conf_value(config, "SERVER_IP"));
    if(!strlen(token)) strcpy(token, get_conf_value(config, "TOKEN"));

    //connect
    //send
    if((sockfd = socket_connect(server_ip, server_port)) < 0){
        perror("socket_connet");
        exit(1);
    }

    if(send(sockfd, token, strlen(token), 0) < 0){
        close(sockfd);
        perror("send token");
        exit(1);
    }

    struct timaval = tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);
    if(select(sockfd + 1, &rfds, NULL, NULL, &tv) <= 0){
        fprintf(stderr, RED"server do not response for token!\n"NONE);
        close(sockfd);
        exit(1);
    }

    //客户端发过去token之后，得知道自己的是不是发送到了
    int ack = 0;
    if(recv(sockfd, &ack, sizeof(ack), 0) <= 0){
        close(sockfd);
        perror("recv ack");
        exit(1);
    }

    //ack = 1服务端响应成功，已经收到客户端的token了
    if(ack != 1){
        fprintf(stderr, RED"server response error!\n"NONE);
        exit(1);
    }
    if(sizeof_log_dir != 0) do_with_file();
    //新建一个检测线程，用以进行系统资源的获取do_work
    //也可以新建进程来检测资源

    //接收服务端心跳信息，并回复一个msg.type = ACK
    while(1){
        struct pihealthd_msg_ds msg;
        bzero(&msg, sizeof(msg));
        recv(sockfd, (void *)&msg, sizeof(msg), 0);
        tmp_for_relogin = 10; //每收到服务端的信息就重新设置一下
        if(msg.type & PI_HEART){
            DBG(BLUE"<xin>\n"NONE);
            msg.type = PI_ACK;
            send(sockfd, (void *)&msg, sizeof(msg), 0);
        }
    }
    sleep(1000);
    return 0;
}


void *heart_beat_from_client(void *arg){
    while(1){
        pthread_cond_wait(&cond, &mutex);
        //do_login,将自己重新加入到服务端的client列表中,也就是重新登录一下
        do_with_file();
    }
}


void *do_work(void *arg){
    while(1){
        //check for cpu (popen)
        //每check一次，变量tmp_for_relogin--
        if(--tmp_for_relogin == 0){
            pthread_cond_signal(&cond);
        }
        //数据如何发送
        //将数据压入消息队列；任务队列
    }
}

void *do_msg_queue(){
    while(1){
        //msgrecv()  从消息队列取数据
        //检查数据的可靠性,没问题在发送
        //json发送的话,构造成json对象
        //序列化,发送
        //如果发送不成功,或者服务端不在线
        //数据持久化到文件
    }
}

void do_with_file(){
    //read from File
    //send
    //delete
}

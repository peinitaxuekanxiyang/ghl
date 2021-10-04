/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: Fri 10 Sep 2021 08:10:41 PM CST
 ************************************************************************/
#include"head.h"

static int port;
char *config = "./pihealthd.conf";
char token[100];
struct client_ds *clients;
int epollfd, max;

int main(int argc, char **argv){
    int opt;
    //守护进程
    int server_listen, sockfd;
    pthread_t login_tid, reactor_tid;

    while((opt = getopt(argc, argv, "p:t:m:")) != -1){
        switch(opt){
            case 'p':
                port = atoi(optarg);
                break;
            case 't':
                strcpy(token, optarg);
                break;
            case 'm':
                max = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage : %s -p port!\n", argv[0]);
                exit(1);
        }
    }
    if(!port) port = atoi(get_conf_value(config, "PORT"));
    if(!max) max = atoi(get_conf_value(config, "MAXCLIENT"));
    if(!strlen(token)) strcpy(token, get_conf_value(config, "TOKEN"));

    signal(SIGALRM, heart_beat);

    clients = (struct client_ds *)calloc(max, sizeof(struct client_ds));

    if((server_listen = socket_create(port)) < 0){
        perror("socket_create");
        exit(1);
    }

    if((epollfd = epoll_create(1)) < 0){
        perror("epoll_create");
        exit(1);
    }
    //每隔一秒钟产生一个时钟信号
    struct itimerval itimer;
    itimer.it_interval.tv_sec = 1;
    itimer.it_interval.tv_usec = 0;
    itimer.it_value.tv_sec = 10;
    itimer.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL, SI_TIMER, NULL);

    pthread_create(&login_tid, NULL, do_login, (void *)&server_listen);
    pthread_create(&reactor_tid, NULL, work_on_reactor, NULL);

    while(1){
        sleep(10);
    }
    return 0;
}

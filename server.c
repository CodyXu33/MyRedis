#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


static void do_something(int confd){
    char rbuf[64];
    ssize_t n = read(confd,rbuf,sizeof(rbuf) - 1);
    if (n < 0)
    {
        perror("read() error");
        return;
    }
    printf("客户端：%s\n",rbuf);

    char wbuf[] = "world";
    write(confd,wbuf,strlen(wbuf));
    
}

int main(void){

    //获取socket句柄
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if (fd == 0)
    {
        perror("获取socket句柄失败\n");
        exit(EXIT_FAILURE);

    }

    //设置
    int opt = 1;
    setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    

    //绑定监听端口
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(0);

    int rv = bind(fd,(struct sockaddr*)&addr,sizeof(addr));
    if(rv < 0){
        perror("端口绑定失败\n");
        exit(EXIT_FAILURE);

    }

    int is_listen = listen(fd,SOMAXCONN);

    if (is_listen < 0)
    {
        perror("监听端口失败\n");
        exit(EXIT_FAILURE);
    }
    


    while (1)
    {
        struct sockaddr_in  client_addr = {};
        socklen_t addr_len = sizeof(client_addr);
        int confd = accept(fd,(struct sockaddr *)&client_addr,&addr_len);
        if (confd < 0)
        {
            continue;
        }
        do_something(confd);
        close(confd);
        

    }
    
    
    


    return 0;
}
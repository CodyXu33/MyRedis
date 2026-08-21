#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(void){
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if (fd < 0)
    {
        perror("获取socket句柄失败\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);
    int rv  = connect(fd,(struct sockaddr *)&addr,sizeof(addr));
    if (rv)
    {
        perror("连接失败\n");
        exit(EXIT_FAILURE);
    }
    
    char msg[] = "hello";
    write(fd,msg,strlen(msg));


    char rbuf[64] = {};
    ssize_t n  = read(fd,rbuf,sizeof(rbuf) - 1);
    if (n < 0)
    {
        perror("读取服务器返回内容失败\n");
        exit(EXIT_FAILURE);
    }
    printf("服务器返回消息：%s\n",rbuf);

    close(fd);
    
    
    return 0;
}
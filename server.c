#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>


const size_t k_max_msg = 4096;
static int32_t read_full(int fd,char *buf,size_t n){
    while (n > 0)
    {
       ssize_t rv = read(fd,buf,n);
       if (rv <= 0)
       {
        return -1;
       }
       assert((size_t)rv <= n);
       n = n - (size_t)rv;
       buf = buf + rv;
       
    }
    
    return 0;
}


static int32_t write_full(int fd,char *buf,size_t n){
    while (n > 0)
    {
       ssize_t rv = write(fd,buf,n);
       if (rv == -1)
       {
        return -1;
       }
       assert((size_t)rv <= n);
       n = n - (size_t)rv;
       buf = buf + rv;
       
    }
    
    return 0;
}


static int32_t on_request(int confd){
    char rbuf[4+k_max_msg] = {};

    int32_t error = read_full(confd,rbuf,4);
    if (error)
    {
        return error;
    }

    //检查请求体长度
    int len;
    memcpy(&len,rbuf,4);
    if (len > k_max_msg)
    {
        perror("payload is too long\n");
        return -1;
    }
    
    error =  read_full(confd,&rbuf[4],len);
    if(error){
        perror("2.read_full error\n");
        return -1;
    }
    
    printf("客户端：%s\n",&rbuf[4]);

    const char reply[] = "world";
    char wbuf[4 + sizeof(reply)];
    len = (uint32_t)strlen(reply);
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], reply, len);
    return write_full(confd, wbuf, 4 + len);
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

        while (1)
        {
            int32_t error = on_request(confd);
            if (error)
            {
               break;
            }
            
        }
        
        close(confd);
        

    }
    
    
    


    return 0;
}
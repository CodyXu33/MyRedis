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
static int32_t query(int fd, const char *text) {

    int len = 0;
    len = strlen(text);

    if (len > k_max_msg)
    {
        perror("payload is too long\n");
        return -1;
    }
    
    char wbuf[4+k_max_msg];
    memcpy(wbuf,&len,4);
    memcpy(&wbuf[4],text,len);

    int error = write_full(fd,wbuf,4+len);
    if (error)
    {
        perror("write_full() error\n");
        return -1;
    }

    char rbuf[4+k_max_msg];

    error = read_full(fd,rbuf,4);

    if (error)
    {
        perror("read_full() error\n");
        return -1;
    }

    
    memcpy(&len,rbuf,4);


    if (len > k_max_msg)
    {
       perror("read_full() paload is too long\n");
       return -1;
    }

    error = read_full(fd,&rbuf[4],len);
    if (error)
    {
      perror("read_full() fail\n");
      return -1;
    }

    printf("服务端: %s\n",&rbuf[4]);

    return 0;
}

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
    
    int32_t err = query(fd, "hello21");
    if (err) {
        goto L_DONE;
    }
    err = query(fd, "hello2211111111");
    if (err) {
        goto L_DONE;
    }
    L_DONE:
    close(fd);
    
    
    return 0;
}
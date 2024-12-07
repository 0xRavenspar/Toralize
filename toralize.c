#include "toralize.h"

Req *request(const char *dstip, const int dstport){
    Req *req;

    req = malloc(reqsize);
    req->vn = 4;
    req->cd = 1;
    req->dstport = htons(dstport);
    req->dstip = inet_addr(dstip);
    strncpy(req->userid, USERNAME,8);

    return req;
}


int main(int argc, char *argv[]){
    char *host;
    int port, sock_fd;
    Req *req;
    Res *res;
    char buf[ressize];
    struct sockaddr_in sock;
    int success;

    if (argc < 3){
        fprintf(stderr, "Usage: %s <ip_addr> <port_num>\n", argv[0]);
        return 1;
    }

    host = argv[1];
    port = atoi(argv[2]);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0 ){
        perror("socker");
        return 1;
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(PROXYPORT);
    sock.sin_addr.s_addr= inet_addr(PROXY);

    if (connect(sock_fd, (struct sockaddr *)&sock, sizeof(sock))){
        perror("connect");
        return 1;
    }

    printf("Connection established\n");
    req = request(host, port);
    write(sock_fd, req,reqsize);

    //set buffer to zero
    memset(buf,0,ressize);
    if(read(sock_fd,buf,ressize) < 1){
        perror("read");
        free(req);
        close(sock_fd);

        return -1;
    }

    res = (Res *) buf;
    success = (res->cd == 90);
    if(!success){
        fprintf(stderr, "Unable to traverse the proxy,"
            "error code: %d\n", res->cd);
        
        close(sock_fd);
        free(req);

        return -1;
    }
    printf("Succesfully connected through the proxy to %s:%d\n",host, port);

    close(sock_fd);
    free(req);
    return 0;

}
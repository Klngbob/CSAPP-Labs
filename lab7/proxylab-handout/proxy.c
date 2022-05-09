#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define MAX_HOSTNAME_SIZE 128
#define MAX_PORT_SIZE 64
#define MAX_HEADER_SIZE 32
#define MAX_KEY_SIZE 64
#define MAX_VALUE_SIZE 128

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

typedef struct {
    char host[MAX_HOSTNAME_SIZE];
    char port[MAX_PORT_SIZE];   /* URL中可能带端口 */
    char path[MAXLINE];
} RequestLine;

typedef struct {
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
} RequestHeader;

void* thread(void* vargp);  /* 线程例程 */
void doit(int connfd);
void parse_uri(char* uri, RequestLine* request_line);
void read_requesthdrs(rio_t* rio, char* buf, RequestHeader* request_headers, int* hdrs_nums);
void set_requesthdrs(RequestLine* request_line, RequestHeader* request_headers, int* hdrs_nums);
void sendto_server(int serverfd, rio_t* server_rio, RequestLine* request_line, RequestHeader* request_headers, int* hdrs_nums);
RequestHeader parse_headers(char* buf);

int main(int argc, char* argv[])
{
    int listenfd, *connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    pthread_t tid;
    struct sockaddr_storage clientaddr;

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    while (1)
    {
        clientlen = sizeof(clientaddr);
        connfd = Malloc(sizeof(int));   /* 在线程里释放这段内存 */
        *connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);
        Getnameinfo((SA*)&clientaddr, clientlen, hostname, MAX_HOSTNAME_SIZE, port, MAX_PORT_SIZE, 0);
        printf("Accepted connection from (%s , %s)\n", hostname, port);
        //Pthread_create(&tid, NULL, thread, connfd);
        doit(*connfd);
        Close(*connfd);
    }

    Close(listenfd);
    return 0;
}

void* therad(void* vargp) {
    int connfd = *((int*) vargp);
    Pthread_detach(pthread_self());
    Free(vargp);
    doit(connfd);
    Close(connfd);
    return NULL;
}

void doit(int connfd) {
    int serverfd, hdrs_nums = 0;
    size_t n;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    RequestLine request_line;
    RequestHeader request_headers[MAX_HEADER_SIZE];
    rio_t proxy_rio, server_rio;

    /* 读取请求行 */
    Rio_readinitb(&proxy_rio, connfd);
    Rio_readlineb(&proxy_rio, buf, MAXLINE);
    printf("Request headers:\n");
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);
    if(strcasecmp(method, "GET")) {
        printf("Not implemented this method!");
        return;
    }
    parse_uri(uri, &request_line);
    read_requesthdrs(&proxy_rio, buf, request_headers, &hdrs_nums);
    set_requesthdrs(&request_line, request_headers, &hdrs_nums);

    serverfd = Open_clientfd(request_line.host, request_line.port);
    sendto_server(serverfd, &server_rio, &request_line, request_headers, &hdrs_nums);

    while ((n = Rio_readlineb(&server_rio, buf, MAXLINE)) != 0)
    {
        printf("Proxy received %d bytes\n, sending to client", n);
        Rio_writen(connfd, buf, n);
    }
    Close(serverfd);    /* 在这里关闭连接服务器描述符 */
}

void parse_uri(char* uri, RequestLine* request_line) {
    char* c;
    if(strstr(uri, "http://") != uri) {
        fprintf(stderr, "URL error: Invalid URI!\n");
        exit(0);
    }
    uri += strlen("http://");
    if((c = strstr(uri, ":")) == NULL) {
        c = strstr(uri, "/");
        *c = '\0';
        strcpy(request_line->host, uri);
        strcpy(request_line->port, "80");
    }
    else {
        *c = '\0';
        strcpy(request_line->host, uri);
        uri = c + 1;
        c = strstr(uri, "/");
        *c = '\0';
        strcpy(request_line->port, uri);
    }
    *c = '/';   /* c此时指向文件路径 */
    strcpy(request_line->path, c);
}

void read_requesthdrs(rio_t* rio, char* buf, RequestHeader* request_headers, int* hdrs_nums) {
    *hdrs_nums = 0;
    Rio_readlineb(&rio, buf, MAXLINE);
    while (strcmp(buf, "\r\n"))
    {
        request_headers[(*hdrs_nums)++] = parse_headers(buf);
        Rio_readlineb(rio, buf, MAXLINE);
    }
}

RequestHeader parse_headers(char* buf) {
    RequestHeader header;
    char* c = strstr(buf, ":");
    if(c == NULL) {
        fprintf(stderr, "Request Headers Error: Invalid Header!\n");
        exit(0);
    }
    *c = '\0';
    strcpy(header.key, buf);
    strcpy(header.value, c + 2);
    return header;
}

void set_requesthdrs(RequestLine* request_line, RequestHeader* request_headers, int* hdrs_nums) {
    int i, host_flag = -1, usr_flag = -1, conn_flag = -1, proxy_flag = -1;
    for(i = 0; i < (*hdrs_nums); ++i) {
        if(!strcasecmp(request_headers[i].key, "Host")) host_flag = i;
        else if(!strcasecmp(request_headers[i].key, "User-Agent")) usr_flag = i;
        else if (!strcasecmp(request_headers[i].key, "Connection")) conn_flag = i;
        else if (!strcasecmp(request_headers[i].key, "Proxy-Connection")) proxy_flag = i;
    }
    if(host_flag == -1) {
        i = (*hdrs_nums)++;
        strcpy(request_headers[i].key, "Host");
        strcpy(request_headers[i].value, request_line->host);
    }
    if(usr_flag != -1) {
        strcpy(request_headers[usr_flag].value, user_agent_hdr);
    }
    else {
        i = (*hdrs_nums)++;
        strcpy(request_headers[i].key, "User-Agent");
        strcpy(request_headers[i].value, user_agent_hdr);
    }
    if(conn_flag != -1) {
        strcpy(request_headers[conn_flag].value, "close");
    }
    else {
        i = (*hdrs_nums)++;
        strcpy(request_headers[i].key, "Connection");
        strcpy(request_headers[i].value, "close");
    }
    if(proxy_flag != -1) {
        strcpy(request_headers[proxy_flag].value, "close")
    }
    else {
        i = (*hdrs_nums)++;
        strcpy(request_headers[i].key, "Proxy-Connection");
        strcpy(request_headers[i].value, "close");
    }
}

void sendto_server(int serverfd, rio_t* server_rio, RequestLine* request_line, RequestHeader* request_headers, int* hdrs_nums) {
    int i;
    char buf[MAXLINE], *buf_start;
    buf_start = buf;
    Rio_readinitb(server_rio, serverfd);
    sprintf(buf, "GET %s HTTP/1.0", request_line->path);    /* 强制使用HTTP/1.0 */
    buf_start = buf + strlen(buf);
    for(i = 0; i < (*hdrs_nums); ++i) {
        sprintf(buf_start, "%s: %s", request_headers[i].key, request_headers[i].value);
        buf_start = buf +strlen(buf);
    }
    sprintf(buf_start, "\r\n");
    Rio_writen
}
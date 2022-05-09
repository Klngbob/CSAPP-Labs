#include <stdio.h>
#include "csapp.h"
#include "sbuf.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define MAX_HOSTNAME_SIZE 128
#define MAX_PORT_SIZE 64
#define MAX_HEADER_SIZE 32
#define MAX_KEY_SIZE 64
#define MAX_VALUE_SIZE 128
#define NTHREADS 10
#define SBUFSIZE 32

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

/*
*   请求行结构体
*/
typedef struct {
    char host[MAX_HOSTNAME_SIZE];
    char port[MAX_PORT_SIZE];   /* URL中可能带端口 */
    char path[MAXLINE];
} RequestLine;

/*
*   请求报头结构体
*/
typedef struct {
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
} RequestHeader;

/*
*   cache行结构体
*/
typedef struct {
    char* object;
    char* uri;
    int ts; /* LRU时间戳 */
    int is_valid;   /* 有效位 */

    int readcnt;
    sem_t mutex, w; /* mutex表示readcnt的互斥信号量，w表示表示读写互斥信号量 */
} CacheLine;

/*
*   cache模拟器结构体
*/
typedef struct {
    int n;
    CacheLine cache_line[MAX_CACHE_SIZE];
} Cache;

void init_cache();
void update_cache();
void write_cache(char* uri, char* buf);
int in_cache(char* uri);
int cache_evction();
void* thread(void* vargp);  /* 线程例程 */
void doit(int connfd);
void parse_uri(char* uri, RequestLine* request_line);
void read_requesthdrs(rio_t* rio, RequestHeader* request_headers, int* hdrs_nums);
void set_requesthdrs(RequestLine* request_line, RequestHeader* request_headers, int* hdrs_nums);
void sendto_server(int serverfd, rio_t* server_rio, RequestLine* request_line, RequestHeader* request_headers, int* hdrs_nums);
RequestHeader parse_headers(char* buf);

sbuf_t sbuf;    /* 套接字连接描述符缓冲区 */
Cache cache;

int main(int argc, char* argv[])
{
    int i, listenfd, *connfd;
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

    sbuf_init(&sbuf, SBUFSIZE);
    init_cache();
    for(i = 0; i < NTHREADS; ++i) {
        Pthread_create(&tid, NULL, thread, NULL);   /* 线程池 */
    }

    while (1)
    {
        clientlen = sizeof(clientaddr);
        connfd = Malloc(sizeof(int));   /* 在线程里释放这段内存 */
        *connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);
        Getnameinfo((SA*)&clientaddr, clientlen, hostname, MAX_HOSTNAME_SIZE, port, MAX_PORT_SIZE, 0);
        sbuf_insert(&sbuf, *connfd);
        Free(connfd);
        printf("Accepted connection from (%s , %s)\n", hostname, port);
    }

    Close(listenfd);
    return 0;
}

void* thread(void* vargp) {
    Pthread_detach(pthread_self());
    while (1)
    {
        int connfd = sbuf_remove(&sbuf);
        doit(connfd);
        Close(connfd);
    }
    return NULL;
}

void doit(int connfd) {
    int index, serverfd, hdrs_nums = 0;
    size_t n;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char cache_uri[MAXLINE];    /* 用于保存在cache中 */
    RequestLine request_line;
    RequestHeader request_headers[MAX_HEADER_SIZE];
    rio_t proxy_rio, server_rio;

    /* 读取请求行 */
    Rio_readinitb(&proxy_rio, connfd);
    Rio_readlineb(&proxy_rio, buf, MAXLINE);
    printf("Request headers:\n");
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);
    strcpy(cache_uri, uri);
    if(strcasecmp(method, "GET")) {
        printf("Not implemented this method!");
        return;
    }
    parse_uri(uri, &request_line);  /* 解析请求行 */

    if((index = in_cache(uri)) != -1) { /* 所请求的对象在cache中 */
        P(&cache.cache_line[index].w);  /* 要更新时间戳，所以要阻塞 */
        cache.cache_line[index].ts = 1;
        Rio_writen(connfd, cache.cache_line[index].object, strlen(cache.cache_line[index].object));
        V(&cache.cache_line[index].w);
        update_cache();
        return;
    }

    read_requesthdrs(&proxy_rio, request_headers, &hdrs_nums); /* 读取请求报头到相应结构体里 */
    set_requesthdrs(&request_line, request_headers, &hdrs_nums);    /* 按照writeup里设置若干个请求报头信息 */

    serverfd = Open_clientfd(request_line.host, request_line.port);
    sendto_server(serverfd, &server_rio, &request_line, request_headers, &hdrs_nums);   /* 将客户端请求转发给服务器 */

    int buf_size = 0;
    char cache_buf[MAX_OBJECT_SIZE];
    while ((n = Rio_readlineb(&server_rio, buf, MAXLINE)) != 0)
    {
        buf_size += n;
        if(buf_size < MAX_OBJECT_SIZE)
            strcat(cache_buf, buf);
        printf("Proxy received %ld bytes, sending to client\n", n);
        Rio_writen(connfd, buf, n); /* 把服务器回应内容转发给客户端 */

    }
    Close(serverfd);    /* 在这里关闭连接服务器描述符 */

    if(buf_size < MAX_OBJECT_SIZE)
        write_cache(cache_uri, cache_buf);    /* 如果不在cache里把HTML对象写入cache */
    update_cache();
}

void parse_uri(char* uri, RequestLine* request_line) {
    char* c;
    if((c = strstr(uri, "//")) == NULL) {
        fprintf(stderr, "URL error: Invalid URI!\n");
        exit(0);
    }
    uri = c + 2;
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

void read_requesthdrs(rio_t* rio, RequestHeader* request_headers, int* hdrs_nums) {
    char buf[MAXLINE];
    *hdrs_nums = 0;
    Rio_readlineb(rio, buf, MAXLINE);
    while (strcmp(buf, "\r\n"))
    {
        int tmp = *hdrs_nums;
        request_headers[(*hdrs_nums)++] = parse_headers(buf);
        Rio_readlineb(rio, buf, MAXLINE);
        
    }
}

RequestHeader parse_headers(char* buf) {
    RequestHeader header;
    char* c = strstr(buf, ": ");
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
        strcpy(request_headers[i].value, "close\r\n");  /* 千万别忘了加\r\n */
    }
    if(proxy_flag != -1) {
        strcpy(request_headers[proxy_flag].value, "close\r\n");
    }
    else {
        i = (*hdrs_nums)++;
        strcpy(request_headers[i].key, "Proxy-Connection");
        strcpy(request_headers[i].value, "close\r\n");
    }
}

void sendto_server(int serverfd, rio_t* server_rio, RequestLine* request_line, RequestHeader* request_headers, int* hdrs_nums) {
    int i;
    char buf[MAXLINE], *buf_start;
    buf_start = buf;
    Rio_readinitb(server_rio, serverfd);
    sprintf(buf, "GET %s HTTP/1.0\r\n", request_line->path);    /* 强制使用HTTP/1.0 */
    buf_start = buf + strlen(buf);
    for(i = 0; i < (*hdrs_nums); ++i) { /* 逐行写入请求报头 */
        sprintf(buf_start, "%s: %s", request_headers[i].key, request_headers[i].value);
        buf_start = buf +strlen(buf);
    }
    sprintf(buf_start, "\r\n");
    Rio_writen(serverfd, buf, MAXLINE);

}

/**********************
 * Cache函数部分
 **********************/
void init_cache() {
    int i;
    cache.n = 0;
    for(i = 0; i < MAX_CACHE_SIZE; ++i) {
        sem_init(&cache.cache_line[i].mutex, 0, 1);
        sem_init(&cache.cache_line[i].w, 0, 1);
        cache.cache_line[i].readcnt = 0;
        cache.cache_line[i].is_valid = 0;
        cache.cache_line[i].ts = 0;
    }
}

int in_cache(char* uri) {
    int i, index = -1;
    for(i = 0; i < cache.n; ++i) {
        P(&cache.cache_line[i].mutex);
        cache.cache_line[i].readcnt++;
        if(cache.cache_line[i].readcnt == 1)
            P(&cache.cache_line[i].w);
        V(&cache.cache_line[i].mutex);
        if(cache.cache_line[i].is_valid && !strcmp(cache.cache_line[i].uri, uri)) {
            index = i;
            break;
        }
        P(&cache.cache_line[i].mutex);
        cache.cache_line[i].readcnt--;
        if(cache.cache_line[i].readcnt == 0)
            V(&cache.cache_line[i].w);
        V(&cache.cache_line[i].mutex);
    }
    return index;
}

void update_cache() {
    int i;
    for(i = 0; i < cache.n; ++i) {
        P(&cache.cache_line[i].w);
        cache.cache_line[i].ts++;
        V(&cache.cache_line[i].w);
    }
}

/* 返回应替换cache块的索引 */
int cache_evction() {
    int i, res;
    int max_ts = 0;
    if(cache.n < MAX_CACHE_SIZE) {
        res = cache.n++;
    }
    else {
        for(i = 0; i < MAX_CACHE_SIZE; ++i) {
            P(&cache.cache_line[i].mutex);
            cache.cache_line[i].readcnt++;
            if(cache.cache_line[i].readcnt == 1)
                P(&cache.cache_line[i].w);
            V(&cache.cache_line[i].mutex);
            if(cache.cache_line[i].ts > max_ts) {   /* 替换ts最大的cache块 */
                res = i;
            }
            P(&cache.cache_line[i].mutex);
            cache.cache_line[i].readcnt--;
            if(cache.cache_line[i].readcnt == 0)
                V(&cache.cache_line[i].w);
            V(&cache.cache_line[i].mutex);
        }
    }
    return res;
}

void write_cache(char* uri, char* buf) {
    int i = cache_evction();

    P(&cache.cache_line[i].w);
    cache.cache_line[i].is_valid = 1;
    cache.cache_line[i].ts = 1;
    cache.cache_line[i].object = Malloc(sizeof(char) * strlen(buf));
    strcpy(cache.cache_line[i].object, buf);
    cache.cache_line[i].uri = Malloc(sizeof(char) * strlen(uri));
    strcpy(cache.cache_line[i].uri, uri);
    V(&cache.cache_line[i].w);
}
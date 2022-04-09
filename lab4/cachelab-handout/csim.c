#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include "cachelab.h"

static char *data_process_type[3] = {"hit", "miss", "miss eviction"};
int hits = 0, misses = 0, evictions = 0, verbose = 0;

struct cache_line
{
    unsigned long long tags; // tag标记
    int timestamp;           //时间戳，unsigned int 和 int 比较大小会强制类型转换。。。。。慎用unsigned。。。
    short is_valid;          //有效位
};

typedef struct cache_line cache_line;

struct cache_simulator
{
    int set_lines;   // cache组数
    int group_lines; //组内行数
    int block_sizes; //内存块大小
    cache_line *cache_lines;
};

typedef struct cache_simulator cache_simulator;

// 打印 helper 内容的函数，-h 命令使用，内容可自定义
void printUsage()
{
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n"
           "Options:\n"
           "  -h         Print this help message.\n"
           "  -v         Optional verbose flag.\n"
           "  -s <num>   Number of set index bits.\n"
           "  -E <num>   Number of lines per set.\n"
           "  -b <num>   Number of block offset bits.\n"
           "  -t <file>  Trace file.\n\n"
           "Examples:\n"
           "  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n"
           "  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

int visitCache(cache_simulator *cs, unsigned long long addr)
{
    unsigned long long tags;
    int i, group_index, empty_line_index = -1, eviction_index = 0, group_offset;
    group_index = (addr >> cs->block_sizes) % (1 << cs->set_lines);
    tags = addr >> (cs->block_sizes + cs->set_lines);
    group_offset=group_index * cs->group_lines;
    // printf("group_index=%d, tags=%llX\n",group_index,tags);
    //默认每次读取的数据字节数+该块起始地址不会超过下一块地址

    for (i = 0; i < cs->group_lines; ++i)
    {
        if (cs->cache_lines[group_offset + i].is_valid)
        {
            cs->cache_lines[group_offset + i].timestamp++; // LRU每次只需要在组内进行时间戳的修改。。。
            if (cs->cache_lines[group_offset + i].tags == tags)
            {
                cs->cache_lines[group_offset + i].timestamp = 1;
                hits++;
                return 0;
            }

            if (cs->cache_lines[group_offset + i].timestamp >= cs->cache_lines[group_offset + eviction_index].timestamp)
            {                       //找到ts最大的行
                eviction_index = i; //替换该行
            }
        }
        else
        {
            empty_line_index = i; // cache组中有空行，isFull返回空行的行号
        }
    }
    misses++;
    if (empty_line_index != -1)
    {
        // Load data miss, 需要调块至cache中
        cs->cache_lines[group_offset + empty_line_index].is_valid = 1;  //置有效位为1
        cs->cache_lines[group_offset + empty_line_index].timestamp = 1; //最近访问过，ts置1
        cs->cache_lines[group_offset + empty_line_index].tags = tags;
        return 1; // miss
    }
    else
    {
        // Load data miss, 且cache满，需要访存调内存块
        cs->cache_lines[group_offset + eviction_index].is_valid = 1;
        cs->cache_lines[group_offset + eviction_index].tags = tags;
        cs->cache_lines[group_offset + eviction_index].timestamp = 1;
        evictions++;
        return 2; // miss eviction
    }
}

int dataProcess(cache_simulator *cs, char opt, unsigned long long addr, int bytes)
{
    switch (opt)
    {
    case 'L': //读和写相同,hit, miss, miss eviction
        return visitCache(cs, addr);
    case 'S': // write-back and write-allocate
        return visitCache(cs, addr);
    case 'M': // read and write
        hits++;
        return 3 + visitCache(cs, addr);
    default:
        return -1;
    }
}

int main(int argc, char *argv[])
{
    int i, j;
    int s, E, b, bytes;
    unsigned long long addr;
    char c, opt;
    char *filepath;
    char buf[100];
    FILE *file;
    opterr = 0; //不希望getopt()打印错误信息，置opterr为0
    while ((c = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    { //解析命令行
        switch (c)
        {
        case 'h':
            printUsage();
            break;
        case 'v':
            verbose = 1;
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            filepath = optarg;
            break;
        case '?':
        default:
            printf("./csim failed to parse its options.\n");
            exit(1);
        }
    }
    cache_simulator cs;
    if (!(file = fopen(filepath, "r+")))
    {
        printf("trace file not exit\n");
        return -1;
    }
    //init_cache
    cs.set_lines = s;
    cs.group_lines = E;
    cs.block_sizes = b;
    cs.cache_lines = (cache_line *)malloc(sizeof(cache_line) * ((1 << s) * E));
    for (i = 0; i < s; ++i)
    {
        for (j = 0; j < E; ++j)
        {
            cs.cache_lines[i * E + j].is_valid = 0;
            cs.cache_lines[i * E + j].timestamp = 0;
        }
    }
    while (fgets(buf, sizeof(buf), file) != NULL)
    {
        if (buf[0] == 'I')
        {
            continue;
        }
        sscanf(buf, " %c %llX,%d", &opt, &addr, &bytes);
        // printf(" %c %llX,%d\n", opt, addr, bytes);
        i = dataProcess(&cs, opt, addr, bytes);
        if (verbose)
        {
            switch (i)
            {
            case 0: // hit
                printf("%c %llx,%d %s\n", opt, addr, bytes, data_process_type[i]);
                break;
            case 1: // miss
                printf("%c %llx,%d %s\n", opt, addr, bytes, data_process_type[i]);
                break;
            case 2: // miss eviction
                printf("%c %llx,%d %s\n", opt, addr, bytes, data_process_type[i]);
                break;
            default: // modify operation
                printf("%c %llx,%d %s hit\n", opt, addr, bytes, data_process_type[i - 3]);
                break;
            }
        }
    }
    printSummary(hits, misses, evictions);
    return 0;
}

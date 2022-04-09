# CSAPP：第五次实验 Malloc Lab



## 一、实验介绍

​		本实验需要用c语言实现一个动态的存储分配器，也就是你自己版本的malloc，free，realloc函数。

![image-20220126181722573](E:\ustc\CSAPP\labs\lab5\malloclab.assets\image-20220126181722573.png)

![image-20220126181748948](E:\ustc\CSAPP\labs\lab5\malloclab.assets\image-20220126181748948.png)

![image-20220126181757286](E:\ustc\CSAPP\labs\lab5\malloclab.assets\image-20220126181757286.png)

```c
//在memory.c中定义如下函数，大家可以调用

void *mem_sbrk(int incr)//: Expands the heap by incr bytes, where incr is a positive non-zero integer and returns a generic pointer to the first byte of the newly allocated heap area. 
void *mem_heap_lo(void)//:Returns a generic pointer to the first byte in the heap. 
void *mem_heap_hi(void)//: Returns a generic pointer to the last byte in the heap. 
size_t mem_heapsize(void)//:Returns the current size of the heap in bytes. 
size_t mem_pagesize(void)//: Returns the system’s page size in bytes (4K onLinux systems). 

```

**编程规则：**

- 不能改变`mm.c`中函数接口

- 不能直接调用任何内存管理的库函数和系统函数`malloc, calloc, free, realloc, sbrk, brk`

- 不能定义任何全局或者静态复合数据结构如`arrays, structs, trees`，允许使用`integers, floats, and pointers`等简单数据类型

- 返回的指针需要8字节对齐

  

## 二、实验内容



### 2.1 mm_init函数

​		空闲块的组织用`Segregated free list`方法 ，`segregated free list `中大小类的分类方法如下，并且将该链表放在堆的头部，通过序言块将它与数据隔离。在每一个大小类中，空闲块按照`size`由大到小排序。

```c
/*
 * mm_init - initialize the malloc package.
 *The return value should be -1 if there was a problem in performing the initialization, 0 otherwise
 */
int mm_init(void)
{
    int listnumber;
    char *heap;

    /* 初始化分离空闲链表 */
    for (listnumber = 0; listnumber < LISTMAX; listnumber++)
    {
        segregated_free_lists[listnumber] = NULL;
    }
    /* 初始化堆 */
    if ((long)(heap = mem_sbrk(4 * WSIZE)) == -1)
        return -1;
    PUT(heap, 0);
    PUT(heap + (1 * WSIZE), PACK(DSIZE, 1));
    PUT(heap + (2 * WSIZE), PACK(DSIZE, 1));
    PUT(heap + (3 * WSIZE), PACK(0, 1));
    /* 扩展堆 */
    if (extend_heap(INITCHUNKSIZE) == NULL)
        return -1;
    return 0;
}
```



### 2.2 find_fit函数

​		在本实验中，为了使用`best fit`，我们选择采用将链表按照从小到大的顺序排序，然后从头开始遍历链表，当遇到第一个满足要求大小的块，这个块就一定是最适合我们的。因此，我们每次插入某一个块的时候，别忘了要遍历链表，然后将该块放到正确的位置上，以维护链表的单调性。在寻找能放得下`size`个字节的最小的块的时候，我们有两种处理策略。笔者采用的方法如下：遍历当前的链表，寻找是否有满足要求的块，有的话就返回。另一种方法是只遍历`size`所在的链表，如果没有，直接返回`NULL`，交由后面堆去拓展。
　　第一种方法的话很明显时间上效率较低，但是能够保证较大的块能够被使用。后一种方法的话时间上效率较高，但是可能导致较大的某个块一直无法被利用，从而导致了空间的浪费。

```c
// getSizeClass 返回当前的size 值对应在第几条链表上
static void* find_fit(int size)
{
    char* link_begin = start_link_list + WSIZE * getSizeClass(size);
    while(link_begin != end_link_list) {
        char* cur_node = LINK_NEXT(link_begin);
        while(cur_node != NULL && GET_SIZE(HDRP(cur_node)) < size) {
            cur_node = LINK_NEXT(cur_node);
        }
        if (cur_node != NULL)
            return cur_node;
        link_begin += WSIZE;
    }
    return NULL;
}
```



### 2.3mm_malloc 和mm_free函数

​		有几个要注意的点。首先，分配内存的时候，我们需要一个大小至少为`WSIZE`(`allocated block`的头部) + `size`的块，且最小为16。如果小于16的话，会导致`free`的时候放不下，从而出现问题。其次，注意好标志位。`free`的时候下一个块的第二个标志位应该清零。以及`free`的时候，要顺便看下前后能不能合并，可以合并的话应该合并完后再插入到链表当中。

```c
void *mm_malloc(size_t size)
{
    size_t newsize = MAX(ALIGN(size + WSIZE), 16), incr;
    void* addr;
    // 这里我用findFitAndRemove寻找满足要求的块，找到的话顺便删除，没找到则返回NULL
    if ((addr = findFitAndRemove(newsize)) == NULL) {
        incr = MAX(CHUNKSIZE, newsize);
        extend_heap(incr / DSIZE);
        addr = findFitAndRemove(newsize);
    }
    return place(addr, newsize);
}

void mm_free(void *ptr)
{
    size_t size = GET_SIZE(HDRP(ptr));
    size_t prev_alloc = GET_PREV_ALLOC(HDRP(ptr));
    AND(HDRP(NEXT_BLKP(ptr)), ~0x2);      // 标志位清零
    PUT(HDRP(ptr), PACK(size, prev_alloc));
    PUT(FTRP(ptr), PACK(size, prev_alloc));
    PUT(NEXT_PTR(ptr), NULL);
    PUT(LAST_PTR(ptr), NULL);
    coalesced(ptr);                       // 这个很重要，记得合并&插入链表
}

```

### 2.4 place函数

​		这个函数实现块的放置策略，将大的块放在右侧，小的块放在左侧，然后当大的块free掉之后，就能形成一个更大的块来存放。其实更多情况下这更像是一种针对数据造函数的思想，当然，如果数据更改或者是一些顺序更换，这样的写法就有时候反而会导致效率极度下降。

```c
static void* place(void* ptr, size_t size)
{
    size_t all_size = GET_SIZE(HDRP(ptr)), res_size = all_size - size;
    if (res_size < 16) {
        OR(HDRP(NEXT_BLKP(ptr)), 0x2);
        size = all_size;
        PUT(HDRP(ptr), PACK(size, 0x3));
    }
    else if (size < 96) {
        char* new_block = (char*)ptr + size;
        PUT(HDRP(new_block), PACK(res_size, 0x2));
        PUT(FTRP(new_block), PACK(res_size, 0x2));
        PUT(NEXT_PTR(new_block), NULL);
        PUT(LAST_PTR(new_block), NULL);
        add_node(new_block, getSizeClass(res_size));
        PUT(HDRP(ptr), PACK(size, 0x3));
    } else {
        char* new_block = (char*)ptr + res_size;
        PUT(HDRP(ptr), PACK(res_size, 0x2));
        PUT(FTRP(ptr), PACK(res_size, 0x2));
        PUT(NEXT_PTR(ptr), NULL);
        PUT(LAST_PTR(ptr), NULL);
        add_node(ptr, getSizeClass(res_size));
        PUT(HDRP(new_block), PACK(size, 0x3));
        ptr = new_block;
        OR(HDRP(NEXT_BLKP(ptr)), 0x2);
    }
    return ptr;
}
```



### 2.5 realloc函数

​		关于这个函数，是因为它有比较多的可以优化的地方。`trace`文件中的最后两个测试如果不采用一定的优化的话，会导致空间利用率很低。

​		首先，如果`realloc`的`size`比之前还小，那么我们不需要进行拷贝，直接返回即可(或者可以考虑对当前块进行分割)；其次，如果下一块是一个空闲块的话，我们可以直接将其占用。这样的话可以很大程度上减少外部碎片。充分地利用了空闲的块。接着，如果下一个块恰好是堆顶，我们可以考虑直接拓展堆，这样的话就可以避免free和malloc，提高效率。最后，实在没有办法的情况下，我们再考虑重新malloc一块内存，并且free掉原先的内存块。这里要注意一下malloc和free的顺序，如果直接换过来的话可能导致错误。

```c
void *mm_realloc(void *ptr, size_t size)
{
    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }
    if (ptr == NULL)
        return mm_malloc(size);

    size_t oldBlockSize = GET_SIZE(HDRP(ptr));
    size_t oldSize = oldBlockSize - WSIZE;

    if (oldSize >= size) {
        // 这里有另一种策略，如果realloc分配的内存过小，我们可以考虑对这个块进行分割，一定程度上提高了内存的利用率
        return ptr;
    } else {
        size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(ptr)));
        size_t next_size = GET_SIZE(HDRP(NEXT_BLKP(ptr)));
        // 下一块内存是空闲的，并且和当前块大小加起来足够用，则占用下一块内存
        if (!next_alloc && next_size + oldSize >= size) {
            delete_node(ptr + oldBlockSize, getSizeClass(next_size));
            OR(HDRP(NEXT_BLKP(NEXT_BLKP(ptr))), 0x2);
            PUT(HDRP(ptr), PACK(next_size + oldBlockSize, GET_PREV_ALLOC(HDRP(ptr)) | 0x1));
            return ptr;
        }
        // 下一块内存刚好是堆的顶部，直接拓展即可。可以不用free后再malloc。
        if (NEXT_BLKP(ptr) == start_pos) {
            size_t newsize = ALIGN(size - oldSize);
            if (mem_sbrk(newsize) == (void*)-1)
                return NULL;
            PUT(HDRP(ptr), PACK(oldBlockSize + newsize, GET_PREV_ALLOC(HDRP(ptr)) | 0x1));
            start_pos += newsize;
            PUT(HDRP(start_pos), PACK(0, 0x3));
            return ptr;
        }
        void *newptr = mm_malloc(size);
        if (newptr == NULL)
            return NULL;
        memcpy(newptr, ptr, oldSize);
        mm_free(ptr);
        return newptr;
    }
}
```

​	最后结果：

![image-20220126185342041](E:\ustc\CSAPP\labs\lab5\malloclab.assets\image-20220126185342041.png)



## 三、实验总结

​		`malloclab`对我来说很有难度，基本上都是借鉴了网上的博客，同时也体现了自己在内存管理方面知识掌握程度的薄弱，之后有时间我一定要自己仔细做一遍这个`lab`。



## 四、参考链接

- [csapp-Malloclab](https://littlecsd.net/2019/02/14/csapp-Malloclab/)
- [CSAPP实验五：动态内存分配（Malloc lab）](https://www.cxybb.com/article/qq_35739903/119654444)
- [内存管理：隐式空闲链表](https://zhuanlan.zhihu.com/p/376217387)

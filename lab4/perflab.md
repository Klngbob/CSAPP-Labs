# CSAPP：第四次实验 Performance Lab



## 一、实验说明

​		图像处理中存在很多函数，可以对这些函数进行优化，本实验主要关注两种图像处理操作

- 旋转：对图像逆时针旋转90度

- 平滑：对图像进行模糊操作

   图像用二维矩阵$M$表示，$M_{ij}$表示图像`M`的第`(i,j)`像素的值，像素值用红，绿，蓝表示。

![image-20220126172300771](E:\ustc\CSAPP\labs\lab4\lab4.assets\image-20220126172300771.png)

![image-20220126172314635](E:\ustc\CSAPP\labs\lab4\lab4.assets\image-20220126172314635.png)

```c
#define RIDX(i,j,n) ((i)*(n)+(j))
typedef struct { 
	unsigned short red; /* R value */ 
	unsigned short green; /* G value */
	unsigned short blue; /* B value */
} pixel
```

​		图像用一维数组表示，第$(i,j)$个像素表示为`I[RIDX(i,j,n)]`，`n`为图像的维数。




## 二、实验内容

### 2.1 Rotate

**1、宏展开**

```c
#define RIDX(i,j,n) ((i)*(n)+(j))

void naive_rotate(int dim, pixel *src, pixel *dst)
{
    int i, j;

    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            dst[RIDX(dim - 1 - j, i, dim)] = src[RIDX(i, j, dim)];
}
```

​		可以将宏展开：`RIDX(dim - 1 - j, i, dim)=dim*(dim - 1 - j)=dim^2 - dim - dim *j + i`，所以有部分常量可以先定义，再用寄存器保存`i*dim`和`j*dim`防止多次访存。

```c
void rotate(int dim, pixel *src, pixel *dst)
{
    int i, j;
    int dim_cst = dim * (dim - 1);
    int temp_i = 1;
    int temp_j = 1;
    for (i = 0; i < dim; i++)
    {
        temp_i = dim * i;
        for (j = 0; j < dim; j++)
        {
            temp_j = dim * j;
            dst[dim_cst + i - temp_j] = src[temp_i + j];
        }
    }
}
```

运行：

```shell
make driver
./driver
```

![image-20220126172952294](E:\ustc\CSAPP\labs\lab4\lab4.assets\image-20220126172952294.png)

​		可以看到`CPE`提升了一点，但是很有限。

2、**矩阵分块**

​		将矩阵分成相同规格的小矩阵，对这些小矩阵进行`rotate`操作，将矩阵分为一块八行八列：

```c
void rotate(int dim, pixel *src, pixel *dst)
{
    int i, j, a, b;
    int sdim = dim - 1;
    for (i = 0; i < dim; i += 8)
    {
        for (j = 0; j < dim; j += 8)
        {
            for (a = i; a < i + 8; a++)
            {
                for (b = j; b < j + 8; b++)
                {
                    dst[RIDX(sdim - b, a, dim)] = src[RIDX(a, b, dim)];
                }
            }
        }
    }
}
```

果然`CPE`有了显著提示，结果：

![image-20220126173553241](E:\ustc\CSAPP\labs\lab4\lab4.assets\image-20220126173553241.png)

**3、循环展开**

```c
int i,j,ki,kj;
//循环展开，先32*32分块，同时4*4循环展开
   for (i = 0; i < dim; i+=32) //每块32行
	for (j = 0; j < dim; j+=32) //每块32列
		for(ki=i; ki<i+32; ki+=4) //控制循环展开的行
			//控制循环展开的列
			for(kj=j; kj<j+32; kj+=4) {
				//在4*4的小块内，手写每一个像素点的旋转变换
				dst[RIDX(dim-1-kj, ki, dim)] = src[RIDX(ki, kj, dim)];
				dst[RIDX(dim-1-kj-1, ki, dim)] = src[RIDX(ki, kj+1, dim)];
				dst[RIDX(dim-1-kj-2, ki, dim)] = src[RIDX(ki, kj+2, dim)];
				dst[RIDX(dim-1-kj-3, ki, dim)] = src[RIDX(ki, kj+3, dim)];
				dst[RIDX(dim-1-kj, ki+1, dim)] = src[RIDX(ki+1, kj, dim)];
				dst[RIDX(dim-1-kj-1, ki+1, dim)] = src[RIDX(ki+1, kj+1, dim)];
				dst[RIDX(dim-1-kj-2, ki+1, dim)] = src[RIDX(ki+1, kj+2, dim)];
				dst[RIDX(dim-1-kj-3, ki+1, dim)] = src[RIDX(ki+1, kj+3, dim)];
				dst[RIDX(dim-1-kj, ki+2, dim)] = src[RIDX(ki+2, kj, dim)];
				dst[RIDX(dim-1-kj-1, ki+2, dim)] = src[RIDX(ki+2, kj+1, dim)];
				dst[RIDX(dim-1-kj-2, ki+2, dim)] = src[RIDX(ki+2, kj+2, dim)];
				dst[RIDX(dim-1-kj-3, ki+2, dim)] = src[RIDX(ki+2, kj+3, dim)];
				dst[RIDX(dim-1-kj, ki+3, dim)] = src[RIDX(ki+3, kj, dim)];
				dst[RIDX(dim-1-kj-1, ki+3, dim)] = src[RIDX(ki+3, kj+1, dim)];
				dst[RIDX(dim-1-kj-2, ki+3, dim)] = src[RIDX(ki+3, kj+2, dim)];
				dst[RIDX(dim-1-kj-3, ki+3, dim)] = src[RIDX(ki+3, kj+3, dim)];		
			}
```

​		程序运行结果如下，`CPE`还是提升的较明显，但和分块好像差不多。

![image-20220126174229841](E:\ustc\CSAPP\labs\lab4\lab4.assets\image-20220126174229841.png)



### 2.2 Smooth

**1、宏展开**

​		和`Rotate`一样，将宏在循环里的重复常数计算提出循环外，重写一下`avg`函数，定义`my_avg`函数：

```c
static pixel my_avg(int dim, int i, int j, pixel *src)
{
    int ii, jj, ii_dim;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for (ii = max(i - 1, 0); ii <= min(i + 1, dim - 1); ii++)
    {
        ii_dim = ii * dim;//提出宏里的常量
        for (jj = max(j - 1, 0); jj <= min(j + 1, dim - 1); jj++)
        {
            accumulate_sum(&sum, src[jj + ii_dim]);
        }
    }
    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst)
{
    //naive_smooth(dim, src, dst);
    int i, j;
    int temp_i;

    for (i = 0; i < dim; i++)
    {
        temp_i = i * dim;
        for (j = 0; j < dim; j++)
        {
            dst[temp_i + j] = my_avg(dim, i, j, src);
        }
    }
}
```

​		运行结果如下：`CPE`提升不是很多。

![image-20220126174542925](E:\ustc\CSAPP\labs\lab4\lab4.assets\image-20220126174542925.png)

2、先将边界条件计算完，按先计算四个角的值，再计算四条边的值，再计算非边界的点的值的顺序，这样可以避免大量的分支预测，还可以有良好的局部性。还有些细节，将判断第一行的循环移到判断非边界点的循环之前，可以改善一点缓存不命中的情况，代码如下：

```c
void smooth(int dim, pixel *src, pixel *dst)
{
    int i, j;

    pixel current_pixel;
    pixel *pcurrent_pixel = &current_pixel;

    i = 0;
    j = 0;
    pcurrent_pixel->red =
        (unsigned short)(((int)(src[RIDX(0, 0, dim)].red + src[RIDX(0, 1, dim)].red +
                                src[RIDX(1, 0, dim)].red + src[RIDX(1, 1, dim)].red)) /
                         4);
    pcurrent_pixel->green =
        (unsigned short)(((int)(src[RIDX(0, 0, dim)].green + src[RIDX(0, 1, dim)].green +
                                src[RIDX(1, 0, dim)].green + src[RIDX(1, 1, dim)].green)) /
                         4);
    pcurrent_pixel->blue =
        (unsigned short)(((int)(src[RIDX(0, 0, dim)].blue + src[RIDX(0, 1, dim)].blue +
                                src[RIDX(1, 0, dim)].blue + src[RIDX(1, 1, dim)].blue)) /
                         4);
    dst[RIDX(0, 0, dim)] = current_pixel;

    i = 0;
    j = dim - 1;
    pcurrent_pixel->red =
        (unsigned short)(((int)(src[RIDX(i, j, dim)].red + src[RIDX(i + 1, j, dim)].red +
                                src[RIDX(i, j - 1, dim)].red + src[RIDX(i + 1, j - 1, dim)].red)) /
                         4);
    pcurrent_pixel->green =
        (unsigned short)(((int)(src[RIDX(i, j, dim)].green + src[RIDX(i + 1, j, dim)].green +
                                src[RIDX(i, j - 1, dim)].green + src[RIDX(i + 1, j - 1, dim)].green)) /
                         4);
    pcurrent_pixel->blue =
        (unsigned short)(((int)(src[RIDX(i, j, dim)].blue + src[RIDX(i + 1, j, dim)].blue +
                                src[RIDX(i, j - 1, dim)].blue + src[RIDX(i + 1, j - 1, dim)].blue)) /
                         4);
    dst[RIDX(i, j, dim)] = current_pixel;

    i = dim - 1;
    j = 0;
    pcurrent_pixel->red =
        (unsigned short)(((int)(src[RIDX(i, j, dim)].red + src[RIDX(i - 1, j, dim)].red +
                                src[RIDX(i, j + 1, dim)].red + src[RIDX(i - 1, j + 1, dim)].red)) /
                         4);
    pcurrent_pixel->green =
        (unsigned short)(((int)(src[RIDX(i, j, dim)].green + src[RIDX(i - 1, j, dim)].green +
                                src[RIDX(i, j + 1, dim)].green + src[RIDX(i - 1, j + 1, dim)].green)) /
                         4);
    pcurrent_pixel->blue =
        (unsigned short)(((int)(src[RIDX(i, j, dim)].blue + src[RIDX(i - 1, j, dim)].blue +
                                src[RIDX(i, j + 1, dim)].blue + src[RIDX(i - 1, j + 1, dim)].blue)) /
                         4);
    dst[RIDX(i, j, dim)] = current_pixel;

    i = dim - 1;
    j = dim - 1;
    pcurrent_pixel->red =
        (unsigned short)(((int)(src[RIDX(i, j, dim)].red + src[RIDX(i - 1, j, dim)].red +
                                src[RIDX(i, j - 1, dim)].red + src[RIDX(i - 1, j - 1, dim)].red)) /
                         4);
    pcurrent_pixel->green =
        (unsigned short)(((int)(src[RIDX(i, j, dim)].green + src[RIDX(i - 1, j, dim)].green +
                                src[RIDX(i, j - 1, dim)].green + src[RIDX(i - 1, j - 1, dim)].green)) /
                         4);
    pcurrent_pixel->blue =
        (unsigned short)(((int)(src[RIDX(i, j, dim)].blue + src[RIDX(i - 1, j, dim)].blue +
                                src[RIDX(i, j - 1, dim)].blue + src[RIDX(i - 1, j - 1, dim)].blue)) /
                         4);
    dst[RIDX(i, j, dim)] = current_pixel;

    j = 0;
    for (i = 1; i < dim - 1; i++)
    {
        pcurrent_pixel->red =
            (unsigned short)(((int)(src[RIDX(i - 1, j, dim)].red + src[RIDX(i - 1, j + 1, dim)].red +
                                    src[RIDX(i, j, dim)].red + src[RIDX(i, j + 1, dim)].red +
                                    src[RIDX(i + 1, j, dim)].red + src[RIDX(i + 1, j + 1, dim)].red)) /
                             6);
        pcurrent_pixel->green =
            (unsigned short)(((int)(src[RIDX(i - 1, j, dim)].green + src[RIDX(i - 1, j + 1, dim)].green +
                                    src[RIDX(i, j, dim)].green + src[RIDX(i, j + 1, dim)].green +
                                    src[RIDX(i + 1, j, dim)].green + src[RIDX(i + 1, j + 1, dim)].green)) /
                             6);
        pcurrent_pixel->blue =
            (unsigned short)(((int)(src[RIDX(i - 1, j, dim)].blue + src[RIDX(i - 1, j + 1, dim)].blue +
                                    src[RIDX(i, j, dim)].blue + src[RIDX(i, j + 1, dim)].blue +
                                    src[RIDX(i + 1, j, dim)].blue + src[RIDX(i + 1, j + 1, dim)].blue)) /
                             6);
        dst[RIDX(i, j, dim)] = current_pixel;
    }

    i = dim - 1;
    for (j = 1; j < dim - 1; j++)
    {
        pcurrent_pixel->red =
            (unsigned short)(((int)(src[RIDX(i, j, dim)].red + src[RIDX(i - 1, j, dim)].red +
                                    src[RIDX(i, j - 1, dim)].red + src[RIDX(i - 1, j - 1, dim)].red +
                                    src[RIDX(i, j + 1, dim)].red + src[RIDX(i - 1, j + 1, dim)].red)) /
                             6);
        pcurrent_pixel->green =
            (unsigned short)(((int)(src[RIDX(i, j, dim)].green + src[RIDX(i - 1, j, dim)].green +
                                    src[RIDX(i, j - 1, dim)].green + src[RIDX(i - 1, j - 1, dim)].green +
                                    src[RIDX(i, j + 1, dim)].green + src[RIDX(i - 1, j + 1, dim)].green)) /
                             6);
        pcurrent_pixel->blue =
            (unsigned short)(((int)(src[RIDX(i, j, dim)].blue + src[RIDX(i - 1, j, dim)].blue +
                                    src[RIDX(i, j - 1, dim)].blue + src[RIDX(i - 1, j - 1, dim)].blue +
                                    src[RIDX(i, j + 1, dim)].blue + src[RIDX(i - 1, j + 1, dim)].blue)) /
                             6);
        dst[RIDX(i, j, dim)] = current_pixel;
    }

    j = dim - 1;
    for (i = 1; i < dim - 1; i++)
    {
        pcurrent_pixel->red =
            (unsigned short)(((int)(src[RIDX(i - 1, j, dim)].red + src[RIDX(i - 1, j - 1, dim)].red +
                                    src[RIDX(i, j, dim)].red + src[RIDX(i, j - 1, dim)].red +
                                    src[RIDX(i + 1, j, dim)].red + src[RIDX(i + 1, j - 1, dim)].red)) /
                             6);
        pcurrent_pixel->green =
            (unsigned short)(((int)(src[RIDX(i - 1, j, dim)].green + src[RIDX(i - 1, j - 1, dim)].green +
                                    src[RIDX(i, j, dim)].green + src[RIDX(i, j - 1, dim)].green +
                                    src[RIDX(i + 1, j, dim)].green + src[RIDX(i + 1, j - 1, dim)].green)) /
                             6);
        pcurrent_pixel->blue =
            (unsigned short)(((int)(src[RIDX(i - 1, j, dim)].blue + src[RIDX(i - 1, j - 1, dim)].blue +
                                    src[RIDX(i, j, dim)].blue + src[RIDX(i, j - 1, dim)].blue +
                                    src[RIDX(i + 1, j, dim)].blue + src[RIDX(i + 1, j - 1, dim)].blue)) /
                             6);
        dst[RIDX(i, j, dim)] = current_pixel;
    }

    i = 0;
    for (j = 1; j < dim - 1; j++)
    {
        pcurrent_pixel->red =
            (unsigned short)(((int)(src[RIDX(i, j, dim)].red + src[RIDX(i + 1, j, dim)].red +
                                    src[RIDX(i, j - 1, dim)].red + src[RIDX(i + 1, j - 1, dim)].red +
                                    src[RIDX(i, j + 1, dim)].red + src[RIDX(i + 1, j + 1, dim)].red)) /
                             6);
        pcurrent_pixel->green =
            (unsigned short)(((int)(src[RIDX(i, j, dim)].green + src[RIDX(i + 1, j, dim)].green +
                                    src[RIDX(i, j - 1, dim)].green + src[RIDX(i + 1, j - 1, dim)].green +
                                    src[RIDX(i, j + 1, dim)].green + src[RIDX(i + 1, j + 1, dim)].green)) /
                             6);
        pcurrent_pixel->blue =
            (unsigned short)(((int)(src[RIDX(i, j, dim)].blue + src[RIDX(i + 1, j, dim)].blue +
                                    src[RIDX(i, j - 1, dim)].blue + src[RIDX(i + 1, j - 1, dim)].blue +
                                    src[RIDX(i, j + 1, dim)].blue + src[RIDX(i + 1, j + 1, dim)].blue)) /
                             6);
        dst[RIDX(i, j, dim)] = current_pixel;
    }


    for (i = 1; i < dim - 1; i++)
    {
        for (j = 1; j < dim - 1; j++)
        {
            pcurrent_pixel->red =
                (unsigned short)(((int)(src[RIDX(i + 1, j, dim)].red + src[RIDX(i + 1, j - 1, dim)].red +
                                        src[RIDX(i, j, dim)].red + src[RIDX(i - 1, j, dim)].red +
                                        src[RIDX(i, j - 1, dim)].red + src[RIDX(i - 1, j - 1, dim)].red +
                                        src[RIDX(i, j + 1, dim)].red + src[RIDX(i - 1, j + 1, dim)].red +
                                        src[RIDX(i + 1, j + 1, dim)].red)) /
                                 9);
            pcurrent_pixel->green =
                (unsigned short)(((int)(src[RIDX(i + 1, j, dim)].green + src[RIDX(i + 1, j - 1, dim)].green +
                                        src[RIDX(i, j, dim)].green + src[RIDX(i - 1, j, dim)].green +
                                        src[RIDX(i, j - 1, dim)].green + src[RIDX(i - 1, j - 1, dim)].green +
                                        src[RIDX(i, j + 1, dim)].green + src[RIDX(i - 1, j + 1, dim)].green +
                                        src[RIDX(i + 1, j + 1, dim)].green)) /
                                 9);
            pcurrent_pixel->blue =
                (unsigned short)(((int)(src[RIDX(i + 1, j, dim)].blue + src[RIDX(i + 1, j - 1, dim)].blue +
                                        src[RIDX(i, j, dim)].blue + src[RIDX(i - 1, j, dim)].blue +
                                        src[RIDX(i, j - 1, dim)].blue + src[RIDX(i - 1, j - 1, dim)].blue +
                                        src[RIDX(i, j + 1, dim)].blue + src[RIDX(i - 1, j + 1, dim)].blue +
                                        src[RIDX(i + 1, j + 1, dim)].blue)) /
                                 9);
            dst[RIDX(i, j, dim)] = current_pixel;
        }
    }
}
```

​		程序运行结果，`CPE`提升效果显著：

![image-20220126175656052](E:\ustc\CSAPP\labs\lab4\lab4.assets\image-20220126175656052.png)



## 三、实验总结

​		做这个`lab`明显感觉自己懈怠了很多，一方面因为考试，另一方面确实这一章的内容没学好，题目又相对开放，大部分都是借鉴网上博客的做法。总之，自己还需要加强这块内容的学习！



## 四、参考链接

- [深入理解计算机系统CSAPP-perfLab：kernels.c性能优化实验：rotate优化详细实验日志（含六个优化版本）](https://blog.csdn.net/qq_41112170/article/details/102907772#_108)
- [Performance Lab 笔记](https://github.com/Exely/CSAPP-Labs/blob/master/notes/perflab.md)


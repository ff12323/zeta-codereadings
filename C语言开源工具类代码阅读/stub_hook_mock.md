









## 第-1章 问题与坑



### -1.1、【core】内存只读->在malloc分配内存时core

#### 1、初步分析

代码与堆栈：

```

    void *save_bytes = malloc(hookHeadSize);
Program received signal SIGSEGV, Segmentation fault.
(gdb) bt
#0  0x00007ffff78a4926 in _int_malloc (av=av@entry=0x7ffff7a19c80 <main_arena>, 
    bytes=bytes@entry=14) at ./malloc/malloc.c:4380
#1  0x00007ffff78a52e2 in __GI___libc_malloc (bytes=14) at ./malloc/malloc.c:3321
#2  0x00007ffff7fb2c87 in hooker::Hooker::saveOriginFuncBytes (this=0x55555556bed0, 
    func=0x555555556450 <add(int, int)>)
```

=============== 汇编与寄存器。

1、位置1：

```assembly
   0x00007ffff7fb2c62 <+144>:   lea    0x3417(%rip),%rax        # 0x7ffff7fb6080
   0x00007ffff7fb2c69 <+151>:   mov    %rax,%rsi
   0x00007ffff7fb2c6c <+154>:   lea    0x345d(%rip),%rax        # 0x7ffff7fb60d0
   0x00007ffff7fb2c73 <+161>:   mov    %rax,%rdi
   0x00007ffff7fb2c76 <+164>:   call   0x7ffff7fb2080 <__assert_fail@plt>
   0x00007ffff7fb2c7b <+169>:   mov    -0x68(%rbp),%rax
   0x00007ffff7fb2c7f <+173>:   mov    %rax,%rdi
   0x00007ffff7fb2c82 <+176>:   call   0x7ffff7fb24a0 <malloc@plt>
=> 0x00007ffff7fb2c87 <+181>:   mov    %rax,-0x58(%rbp)
   0x00007ffff7fb2c8b <+185>:   cmpq   $0x0,-0x58(%rbp)
   0x00007ffff7fb2c90 <+190>:   jne    0x7ffff7fb2d08 <_ZNK6hooker6Hooker19saveOriginFuncBytesEPv+310>
   0x00007ffff7fb2c92 <+192>:   mov    $0x10,%edi
   0x00007ffff7fb2c97 <+197>:   call   0x7ffff7fb1f40 <__cxa_allocate_exception@plt>
   0x00007ffff7fb2c9c <+202>:   mov    %rax,%rbx
   0x00007ffff7fb2c9f <+205>:   lea    -0x70(%rbp),%rax
   0x00007ffff7fb2ca3 <+209>:   mov    %rax,%rdi
```

2、位置2

```assembly
(gdb) 
(gdb) i r rcx
rcx            0x21                33
(gdb) i r rax
rax            0x55555556c450      9382499233083

   0x00007ffff78a4917 <+3815>:  or     $0x1,%rdx
   0x00007ffff78a491b <+3819>:  shl    $0x2,%rcx
   0x00007ffff78a491f <+3823>:  or     %r13,%rcx
   0x00007ffff78a4922 <+3826>:  or     $0x1,%rcx
=> 0x00007ffff78a4926 <+3830>:  mov    %rcx,0x8(%rax)
   0x00007ffff78a492a <+3834>:  mov    0x17bbcc(%rip),%eax        # 0x7ffff7a204fc <perturb_byte>
   0x00007ffff78a4930 <+3840>:  mov    %rdx,0x8(%rsi)
   0x00007ffff78a4934 <+3844>:  test   %eax,%eax
```

#### 2、简化现象

经过现象的简化。发现这个框架，当对一个函数进行hook之后。再次调用malloc就会出现这个core

```c++
    hooker.hook(reinterpret_cast<void*>(add), 
                reinterpret_cast<void*>(mock_add), reinterpret_cast<void**>(&old_add));
    printf("..............\n");
    malloc(199);
```

#### 3、汇编与段错误

SIGSEGV：根据这个错误，发现很可能是汇编代码。要对一个只读的内存地址，进行写操作。

From the information provided, it appears that your program has encountered a segmentation fault (SIGSEGV). A segmentation fault occurs when a program tries to access memory that it is not allowed to, such as <font color="#F9DDA2">accessing memory that hasn't been allocated or writing to read-only memory</font>.

gdb验证：

- 当我们对rax寄存器指向的内存进行，修改内存操作。但是完全没有改变。。。

```
(gdb) x/2 0x55555556c3f0
0x55555556c3f0: 0xf8458bfc      0xc35dd001
(gdb) x/2 0x55555556c3f0
0x55555556c3f0: 0xf8458bfc      0xc35dd001
(gdb) set {long}(0x55555556c3f0 + 0x8)=1111
(gdb) x/2 0x55555556c3f0
0x55555556c3f0: 0xf8458bfc      0xc35dd001

```

正常：如果一个内存是可以修改的，则可以完成。

> ```c
> (gdb) set *((int *) 0xbfbb0000) = 20
> ```

```
(gdb) x/2 0x555555557034
0x555555557034: 0x6c6c6568      0x6f77006f
(gdb) set {int}0x555555557034=0
(gdb) x/2 0x555555557034
0x555555557034: 0x00000000      0x6f77006f
```

#### 4、根本原因

确定了具体原因，再查看路径上的代码，可以发现有一个地方。把malloc分配出来的一片内存（应该包含管理部分），设置为只读。

- 影响：导致下一次调用 malloc直接core
- 验证：把内存属性设置为 `PROT_READ | PROT_WRITE | PROT_EXEC` ，就不会出现这个问题。

```c++
void hooker::HookerX64::doHook(void *func,void *newAddr,void **origFunc) const {
     。。。。
         void *old = malloc(index);
		printf("HookerX64::doHookbackup : %p \n", old);
        if (old != nullptr) 
        {
			memcpy(old, func, index);
            //#define CODE_READ_ONLY			PROT_READ | PROT_EXEC
			changeCodeAttrs(old, CODE_READ_ONLY);
			*origFunc = old;
		}
    
    
void hooker::Hooker::changeCodeAttrs(void *func, int attr) const {
    int pagesize = getpagesize();
    long start = PAGE_START((long)func,pagesize);

    if (mprotect((void *)start, (size_t)pagesize, attr) < 0) {
        throw hooker::error::HookerError("mprotect error");
    }
}
```



## 第1章 （playcpp）inline-hook

- 仓库：https://github.com/changan29/playcpp

### 1.1 基本设计

- hook：
  - 1、给定函数的地址，其在内存中属于代码段 `.text`；属于只读 =====>  需要通过系统调用： `mprotect`，将其设置为可写，而修改汇编代码；
  - 2、Map：函数地址 <----> 函数的头部指令 （`std::unordered_map<long,long> gHookedMap`），使用一个hashmap来记录函数的起始地址，与这个函数的头部（因为要进行inline hook，会把这个函数的头部修改为 jmp到其它地方。。。所以需要进行备份。。。在unhook的时候，要进行恢复）
  - 3、x86-64位机器实现：通过修改text段，函数代码的开头14个字节。变为 ===> `jmp mock_func`（ 14字节 == jmp指令（6字节） + 函数地址（8字节））
  - 4、备份原函数：把一个原函数的函数指针，将函数的text代码段进行，通过malloc赋值到堆上。=====>  就可以在mock函数里调用原函数了。。。

```c++
	/*
	  x64位下使用的跳转是
	jmp 或者 call模式。
	jmp共使用14个字节，0xFF2500000000为6个字节，目标地址为00000000`00000000为8字节。

	  call模式
	  0xff1500000002为6个字节，目标地址为00000000`00000000为8字节。
	*/

    *(uint16_t *)&f[0] = 0x25ff;
    *(int *)&f[2] = 0x00000000;
    *(long *)&f[6] = (long)newAddr;
```

- unhook：恢复修改的部分。

### 1.2 text段的修改

```sh
(gdb) disassemble  add
Dump of assembler code for function _Z3addii:
   0x0000555555556470 <+0>:     endbr64
   0x0000555555556474 <+4>:     push   %rbp
   0x0000555555556475 <+5>:     mov    %rsp,%rbp
   0x0000555555556478 <+8>:     mov    %edi,-0x4(%rbp)
   0x000055555555647b <+11>:    mov    %esi,-0x8(%rbp)
   0x000055555555647e <+14>:    mov    -0x4(%rbp),%edx
   0x0000555555556481 <+17>:    mov    -0x8(%rbp),%eax
   0x0000555555556484 <+20>:    add    %edx,%eax
   0x0000555555556486 <+22>:    pop    %rbp
   0x0000555555556487 <+23>:    ret
End of assembler dump.
(gdb) n
..............
80          hooker.hook(reinterpret_cast<void*>(add),
(gdb) n
hookHeadSize: 14 ,  originFunctionSize: 14
save_bytes: 0x55555556c330
HookerX64::doHookbackup : 0x55555556c3e0
HookerX64::doHook newAddr: 0x555555556488
82          printf("..............\n");
(gdb) disassemble add
Dump of assembler code for function _Z3addii:
   0x0000555555556470 <+0>:     jmp    *0x0(%rip)        # 0x555555556476 <_Z3addii+6>
   0x0000555555556476 <+6>:     mov    %ah,0x55(%rbp,%rdx,2)
   0x000055555555647a <+10>:    push   %rbp
   0x000055555555647b <+11>:    push   %rbp
   0x000055555555647c <+12>:    add    %al,(%rax)
   0x000055555555647e <+14>:    mov    -0x4(%rbp),%edx
   0x0000555555556481 <+17>:    mov    -0x8(%rbp),%eax
   0x0000555555556484 <+20>:    add    %edx,%eax
   0x0000555555556486 <+22>:    pop    %rbp
   0x0000555555556487 <+23>:    ret
End of assembler dump.

```

===========

jmp：

- 跳转的地址是rip，就是下一条指令值，所指向的内存的位置。
- 查看这个内存地址所指向的位置。就是通过hook所挂载的 `mock_add`

```

(gdb) x/4x 0x0000555555556476
0x555555556476 <_Z3addii+6>:    0x55556488      0x00005555      0x8bfc558b      0xd001f845
(gdb) disassemble 0x0000555555556488
Dump of assembler code for function _Z8mock_addii:
   0x0000555555556488 <+0>:     endbr64
   0x000055555555648c <+4>:     push   %rbp
   0x000055555555648d <+5>:     mov    %rsp,%rbp
   0x0000555555556490 <+8>:     sub    $0x10,%rsp
   0x0000555555556494 <+12>:    mov    %edi,-0x4(%rbp)
   0x0000555555556497 <+15>:    mov    %esi,-0x8(%rbp)
   0x000055555555649a <+18>:    mov    0x2cbf(%rip),%rcx        # 0x555555559160 <old_add>

```



## 第2章 MockCpp



Mock之前：

```sh
[----------] Global test environment set-up.
[----------] 1 test from A
[ RUN      ] A.B

Breakpoint 1, A_B_Test::TestBody (this=0x55555569c900) at /home/user/redis-5.0/gtest-with-mockcpp-master/testgtest.cpp:53
53          MockFunc(add_int);
(gdb) disassemble  add_int
Dump of assembler code for function _Z7add_intii:
   0x000055555556871f <+0>:     endbr64 
   0x0000555555568723 <+4>:     push   %rbp
   0x0000555555568724 <+5>:     mov    %rsp,%rbp
   0x0000555555568727 <+8>:     mov    %edi,-0x14(%rbp)
   0x000055555556872a <+11>:    mov    %esi,-0x18(%rbp)
   0x000055555556872d <+14>:    mov    -0x14(%rbp),%edx
```

Mock之后：

```
(gdb) n
55          std::thread t1(calc_sum, 0, 20000);
(gdb) disassemble  add_int
Dump of assembler code for function _Z7add_intii:
   0x000055555556871f <+0>:     jmp    *0x0(%rip)        # 0x555555568725 <_Z7add_intii+6>
   0x0000555555568725 <+6>:     ja     0x5555555686d0 <_ZN33sample_test_case_sample_test_Test8TestBodyEv+238>
   0x0000555555568727 <+8>:     push   %rsi
   0x0000555555568728 <+9>:     push   %rbp
   0x0000555555568729 <+10>:    push   %rbp
```



# 附录A：各种各样



## 二、获取函数的长度

- https://stackoverflow.com/questions/4156585/how-to-get-the-length-of-a-function-in-bytes



### 2.1 通过链接器帮助

You can get this information from the linker if you are <font color="#F9DDA2">using a custom linker script</font>. Add a linker section just for the given function, with linker symbols on either side:

```c
mysec_start = .;
*(.mysection)
mysec_end = .;
```

Then you can specifically assign the function to that section. The difference between the symbols is the length of the function:

```c
#include <stdio.h>

int i;

 __attribute__((noinline, section(".mysection"))) void test_func (void)
{
    i++;
}

int main (void)
{
    extern unsigned char mysec_start[];
    extern unsigned char mysec_end[];

    printf ("Func len: %lu\n", mysec_end - mysec_start);
    test_func ();

    return 0;
}
```

This example is for GCC, but any C toolchain should have a way to specify which section to assign a function to. I would check the results against the assembly listing to verify that it's working the way you want it to.

> 改进点：
>
> You can do this without a custom linker script by calling the section by a valid C identifier name, **for example `mysection` in**stead of `.mysection`, and using `__start_mysection` and `__stop_mysection` instead of `mysec_start` and `mysec_end`. <font color="#F9DDA2">The linker automatically defines these symbols for each section encountered</font>.

```c
#include <stdio.h>

int i;

 __attribute__((noinline, section("mysec"))) void test_func (void)
{
    i++;
}

int main (void)
{
    extern char __start_mysec[];
    extern char __stop_mysec[];

    printf ("Func len: %lu\n", __stop_mysec - __start_mysec);
    test_func ();

    return 0;
}
```

### 2.2 没有一般的办法

（，可执行文件，无调试信息。其没有任何的方式存储函数长度，所以不可能在运行时自己解析出这个信息。。。）<font color="#F9DDA2">Executables (at least ones which have debug info stripped) doesn't store function lengths in any way. So there's no possibility to parse this info in runtime by self</font>. If you have to manipulate with functions, you should do something with your objects in linking phase or by accessing them as files from your executable. For example, you may tell linker to link symbol tables as ordinary data section into the executable, assign them some name, and parse when program runs. But remember, this would be specific to your linker and object format.

Also note, that function layout is also platform specific and there are some things that make the term "function length" unclear:

1. Functions may have store used constants in code sections directly after function code and access them using PC-relative addressing (ARM compilers do this).
2. Functions may have "prologs" and "epilogs" which may may be common to several functions and thus lie outside main body.
3. Function code may inline other function code

They all may count or not count in function length.

Also function may be completely inlined by compiler, so it loose its body.

## 三、《Calculate a C Function Size x64, x86》

- https://gurhanpolat.medium.com/calculate-c-function-size-x64-x86-c1f49921aa1a

Hi to everyone,

For a project i’m working on i needed to calculate a function size for anti-debugging purposes, but could not find any acceptable solution for this problem on the internet.

This let me to think alone and i came up with a solution.

（，办法：将一些操作码放在函数的结尾。。。 然后通过搜索的办法得的。。。）The solution was simple, <font color="#F9DDA2">placing some known opcode at the end of the function we need to find the size</font>. We already know the start address, and <font color="clan">if we put known opcode at the end, we can search from the start to the opcodes we placed in order to calculate the function size at runtime</font>.

So let’s do that together

In order to see what’s going on in the background, switch your assembler output option to /FAcs for both debug and release configuration of your project, like seen in the below picture.



。。。

。。。




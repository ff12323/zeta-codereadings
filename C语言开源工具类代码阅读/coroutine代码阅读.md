项目地址：https://github.com/cloudwu/coroutine





== glibc的用户态上下文（ucontext）

栈对象：

```c#
/* Structure describing a signal stack.  */
typedef struct
  {
    void *ss_sp; //栈指针的位置，起始点？
    int ss_flags;
    size_t ss_size; 
  } stack_t;
```



用户上下文：

- `uc_stack`：上下文的栈状态
- `uc_mcontext`：上下文的寄存器状态

```c
/* Userlevel context.  */
typedef struct ucontext_t
  {
    unsigned long int __ctx(uc_flags);
    struct ucontext_t *uc_link;
    stack_t uc_stack;
    mcontext_t uc_mcontext;
    sigset_t uc_sigmask;
    struct _libc_fpstate __fpregs_mem;
    __extension__ unsigned long long int __ssp[4];
  } ucontext_t;

/* Context to describe whole processor state.  */
typedef struct
  {
    gregset_t __ctx(gregs);
    /* Note that fpregs is a pointer.  */
    fpregset_t __ctx(fpregs);
    __extension__ unsigned long long __reserved1 [8];
} mcontext_t;
```



=== 寄存器上下文之通用寄存器

```c
/* Container for all general registers.  */
typedef greg_t gregset_t[__NGREG];

/* Number of general registers.  */
#define __NGREG	23

/* Type for general register.  */
__extension__ typedef long long int greg_t;
```

=== 寄存器上下文之浮点数寄存器（？）

。。。（略）

。。。






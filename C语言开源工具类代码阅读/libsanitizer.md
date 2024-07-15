仓库：LLVM "compiler-rt" runtime libraries（编译器的运行时库）

- https://compiler-rt.llvm.org/





第一次&&不会的记录：

- RTL：（，详细参考GCC文档；）**register transfer language** (寄存器转化语言) is a kind of intermediate representation (IR) that is very close to assembly language, such as that which is used in a compiler. It is used to describe data flow at the register-transfer level of an architecture. 
- POD：（，纯净旧数据；）A **Plain Old Data** Structure in C++ is an aggregate class that contains only PODS as members, has no user-defined destructor, no user-defined copy assignment operator, and no nonstatic members of pointer-to-member type.
  - https://stackoverflow.com/questions/146452/what-are-pod-types-in-c



## 第-1章 问题与坑

### 1、symbol lookup error: : undefined symbol

- 1、在使用自己编译出来的liblsan的时候，动态库在nm查看下有符号。但是报：undefined symbol

```sh
user@user-vm:~/github/gcc2/gcc-7.3.0/x86_64-pc-linux-gnu/libsanitizer/lsan/.libs$ export LD_PRELOAD=/home/user/github/gcc2/gcc-7.3.0/x86_64-pc-linux-gnu/libsanitizer/lsan/.libs/liblsan.so

user@user-vm:~/github/gcc2/gcc-7.3.0/x86_64-pc-linux-gnu/libsanitizer/lsan/.libs$ ./a.out 
./a.out: symbol lookup error: ./a.out: undefined symbol: __lsan_init
```

=========

> nm：
>
> ```
>        -D
>        --dynamic
>            Display the dynamic symbols rather than the normal symbols.
>            This is only meaningful for dynamic objects, such as certain
>            types of shared libraries.
> ```

查看自己编译的库：

- 缺少：`__lsan_init`

```sh
user@user-vm:~/github/gcc2/gcc-7.3.0/x86_64-pc-linux-gnu/libsanitizer/lsan/.libs$ nm -D liblsan.so.0.0.0 | grep init
==85512==Zyh hello world: enter CheckForLeaks...
0000000000038610 T __asan_backtrace_initialize
00000000000373b0 T __asan_cplus_demangle_init_info
                 U pthread_attr_init@GLIBC_2.2.5
0000000000018280 T __sanitizer_cov_init
00000000000182b0 T __sanitizer_cov_module_init
00000000000184c0 W __sanitizer_cov_trace_pc_guard_init
```

查看系统的库：

```sh
user@user-vm:~/github/gcc2/gcc-7.3.0/x86_64-pc-linux-gnu/libsanitizer/lsan/.libs$ nm -D /lib/x86_64-linux-gnu/liblsan.so.0 | grep init
==85639==Zyh hello world: enter CheckForLeaks...
000000000004a7a0 T __asan_backtrace_initialize
0000000000044a70 T __asan_cplus_demangle_init_info
000000000000cf70 T __lsan_init
                 U pthread_attr_init@GLIBC_2.2.5
00000000000191b0 W __sanitizer_cov_8bit_counters_init
00000000000191d0 W __sanitizer_cov_bool_flag_init
00000000000191e0 W __sanitizer_cov_pcs_init
0000000000018e40 W __sanitizer_cov_trace_pc_guard_init
                 U sched_getaffinity@GLIBC_2.3.4
```

### 2、编译的asan无法运行。

- 原因：感觉是编译的gcc版本太低，系统版本高。

```sh
user@user-vm:~/github/gcc2/gcc-7.3.0/x86_64-pc-linux-gnu/libsanitizer/asan/.libs$ LD_PRELOAD="./libasan.so" ./a.out 
==86303==Shadow memory range interleaves with an existing memory mapping. ASan cannot proceed correctly. ABORTING.
==86303==ASan shadow was supposed to be located in the [0x00007fff7000-0x10007fff7fff] range.
==86303==This might be related to ELF_ET_DYN_BASE change in Linux 4.12.
==86303==See https://github.com/google/sanitizers/issues/856 for possible workarounds.
==86303==Process memory map follows:
```



# 一、sanitizer_common

## 1.1 tmp

### 1、线程状态分析

```c
enum ThreadStatus {
  ThreadStatusInvalid,   // Non-existent thread, data is invalid.
  ThreadStatusCreated,   // Created but not yet running.
  ThreadStatusRunning,   // The thread is currently running.
  ThreadStatusFinished,  // Joinable thread is finished but not yet joined.
  ThreadStatusDead       // Joined, but some info is still available.
};
```

问题1：ThreadStatusFinished,  // Joinable thread is finished but not yet joined.  ，  what this in linux，pls tell detailed and principle and example？

> GPT：
> In Linux, "Joinable thread is finished but not yet joined" refers to <font color="#F9DDA2">a thread state in which a thread has completed its execution, but its resources (such as memory) have not yet been released.</font> This state allows another thread (typically the parent thread) to join with the finished thread and retrieve its exit status or perform any necessary cleanup operations.
>
> When a thread is created in Linux, it can be set to a "joinable" or "detached" state. If a thread is created as joinable, the parent thread has the responsibility to explicitly call the `pthread_join()` function to wait for the thread to finish and retrieve its exit status. Until the join operation is performed, the finished thread remains in the "Joinable thread is finished but not yet joined" state.
>
> The principle behind the joinable thread state is to ensure proper synchronization and resource management in multi-threaded applications. By joining with a finished thread, the parent thread can determine if the child thread completed successfully, retrieve any data or results produced by the child thread, and release any resources associated with it.



线程查找的例子：1、定义回调类型，方便自定义功能  2、定义具体回调函数  3、遍历已知的所有线程上下文信息，通过回调确认  4、返回匹配的线程id（特殊处理： 最大值表示非法ID（`const u32 ThreadRegistry::kUnknownTid = ~0U;`） ）

> ~0U （unsigned int）===》 4294967295 ===》`'0b11111111111111111111111111111111'` 

```c++
static bool FindThreadByUid(ThreadContextBase *tctx, void *arg) {
  uptr uid = (uptr)arg;
  if (tctx->user_id == uid && tctx->status != ThreadStatusInvalid) {
    return true;
  }
  return false;
}

u32 ThreadTid(uptr uid) {
  return thread_registry->FindThread(FindThreadByUid, (void*)uid);
}


  typedef bool (*FindThreadCallback)(ThreadContextBase *tctx, void *arg);
  // Finds a thread using the provided callback. Returns kUnknownTid if no
  // thread is found.
  u32 FindThread(FindThreadCallback cb, void *arg);

u32 ThreadRegistry::FindThread(FindThreadCallback cb, void *arg) {
  BlockingMutexLock l(&mtx_);
  for (u32 tid = 0; tid < n_contexts_; tid++) {
    ThreadContextBase *tctx = threads_[tid];
    if (tctx != 0 && cb(tctx, arg))
      return tctx->tid;
  }
  return kUnknownTid;
}
```



## 1.2 原子性

### 1.2.1 `__asm__ ("" ::: "memory")`

- https://stackoverflow.com/questions/14950614/working-of-asm-volatile-memory

1、

GCC特性：

- （，asm是一个gcc扩展，其允许汇编语言被嵌套到C代码中；。。。）`__asm__` is a gcc extension of permitting assembly language statements to be entered nested within your C code - used here for its property of being able to specify side effects that prevent the compiler from performing certain types of optimisations (which in this case might end up generating incorrect code).
- （，volatile其确保这个asm语句，其本身不要被重新排序；）`__volatile__` is required to ensure that the **asm** statement itself is not reordered with any other volatile accesses any (a guarantee in the C language).
- （，memory是一个GCC指令； 说明这个内联汇编对于全局内存有副作用； 所以不要影响局部变量而被考虑进入；）`memory` is an instruction to GCC that (sort of) says that the inline asm sequence has side effects on global memory, and hence not just effects on local variables need to be taken into account.

2、

```
__asm__ __volatile__("" ::: "memory");
```

（，创建一个编译器级别的内存屏障，强制优化器不要去重排序内存访问，在此屏障之内；）creates a compiler level memory barrier forcing optimizer to not re-order memory accesses across the barrier.

（，例子：为了访问一些地址以特定的顺序；）For example, if you need to **access some address in a specific order** (<font color="#F9DDA2">probably because that memory area is actually backed by a different device rather than a memory</font>) you need to be able tell this to the compiler otherwise it may just optimize your steps for the sake of efficiency.

Assume in this scenario you must increment a value in address, read something and increment another value in an adjacent address.

```c
int c(int *d, int *e) {
        int r;
        d[0] += 1;
        r = e[0];
        d[1] += 1;
        return r;
}
```

Problem is compiler (`gcc` in this case) can rearrange your memory access to get better performance if you ask for it (`-O`). Probably leading to a sequence of instructions like below:

```c
00000000 <c>:
   0:   4603        mov r3, r0
   2:   c805        ldmia   r0, {r0, r2}
   4:   3001        adds    r0, #1
   6:   3201        adds    r2, #1
   8:   6018        str r0, [r3, #0]
   a:   6808        ldr r0, [r1, #0]
   c:   605a        str r2, [r3, #4]
   e:   4770        bx  lr
```

Above values for `d[0]` and `d[1]` are loaded at the same time. Lets assume this is something you want to avoid then you need to tell compiler not to reorder memory accesses and that is to use `asm volatile("" ::: "memory")`.

```c
int c(int *d, int *e) {
        int r;
        d[0] += 1;
        r = e[0];
        asm volatile("" ::: "memory");
        d[1] += 1;
        return r;
}
```

So you'll get your instruction sequence as you want it to be:

```c
00000000 <c>:
   0:   6802        ldr r2, [r0, #0]
   2:   4603        mov r3, r0
   4:   3201        adds    r2, #1
   6:   6002        str r2, [r0, #0]
   8:   6808        ldr r0, [r1, #0]
   a:   685a        ldr r2, [r3, #4]
   c:   3201        adds    r2, #1
   e:   605a        str r2, [r3, #4]
  10:   4770        bx  lr
  12:   bf00        nop
```

It should be noted that this is only compile time memory barrier to avoid compiler to reorder memory accesses, as it puts no extra hardware level instructions to flush memories or wait for load or stores to be completed. CPUs can still reorder memory accesses if they have the architectural capabilities and memory addresses are on `normal` type instead of `strongly ordered` or `device` ([ref](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0211k/Babcddgd.html)).

## 1.3 mmap的（low-level）Vector

通过内部的系统调用，mmap而创建的一种数据结构。不是使用malloc之类的内存分配器。

- 目的：为了避免这里的内存操作被检测到。所以使用低层级的数据结构。

```c++
// A low-level vector based on mmap. May incur a significant memory overhead for
// small vectors.
// WARNING: The current implementation supports only POD types.
template<typename T>
class InternalMmapVectorNoCtor {
```

## 1.4 64位内存分配器

SizeClassAllocator64：

- Space（空间）：是地址空间的一个部分，其起始地址是 `SpaceBeg`，占据了 `kSpaceSize`个字节；（，1、如是起始是`~0`，则由mmap动态创建的选择，否则是一固定的地址  2、kSpaceSize的大小是2的幂次；）
  - 保护性：开始的部分，被`mprotect-ed`保护；
- Region（区域）：存在 `kNumClasses`种区域，它们都有相同的大小；一个区域的目的是为了一种大小的类（相同大小的块。）
- UserChunk（用户块）：一块返回给用户的内存。
- MetaChunk（元数据块）：每一个用户块都有着一个kMetadataSize大小的用户块。
- FreeArray（释放数组）：是一个数组保存着释放的块（通过4字节的偏移，而记录到达是哪一个块）

```c
// SizeClassAllocator64 -- allocator for 64-bit address space.
// The template parameter Params is a class containing the actual parameters.
//
// Space: a portion of address space of kSpaceSize bytes starting at SpaceBeg.
// If kSpaceBeg is ~0 then SpaceBeg is chosen dynamically my mmap.
// Otherwise SpaceBeg=kSpaceBeg (fixed address).
// kSpaceSize is a power of two.
// At the beginning the entire space is mprotect-ed, then small parts of it
// are mapped on demand.
//
// Region: a part of Space dedicated to a single size class.
// There are kNumClasses Regions of equal size.
//
// UserChunk: a piece of memory returned to user.
// MetaChunk: kMetadataSize bytes of metadata associated with a UserChunk.

// FreeArray is an array free-d chunks (stored as 4-byte offsets)
//
// A Region looks like this:
// UserChunk1 ... UserChunkN <gap> MetaChunkN ... MetaChunk1 FreeArray
```



# 二、lsan

激活路径。

```
初始化：
  __lsan_init


exit
...
__do_global_dtors_aux
...
__lsan::DoLeakCheck
```





### 2.y.1 源码-

添加模块初始化的打印日志：

```c++
void InitializePlatformSpecificModules() {
  ListOfModules modules;
  modules.init();
  for (LoadedModule &module : modules) {
    Printf("module name : %s\n", module.full_name());
```

===========

```sh
user@user-vm:~/github/gcc-8.5.0/x86_64-pc-linux-gnu/libsanitizer/lsan/.libs$ LD_PRELOAD="./liblsan.so" ./a.out 
module name : /home/user/github/gcc-8.5.0/x86_64-pc-linux-gnu/libsanitizer/lsan/.libs/a.out
module name : linux-vdso.so.1
module name : ./liblsan.so
module name : /lib/x86_64-linux-gnu/libc.so.6
module name : /home/user/github/gcc-8.5.0/x86_64-pc-linux-gnu/libstdc++-v3/src/.libs/libstdc++.so.6
module name : /lib/x86_64-linux-gnu/libm.so.6
module name : /lib64/ld-linux-x86-64.so.2
module name : /lib/x86_64-linux-gnu/libgcc_s.so.1
priPtr:0x4004b920f000
```

---

模块类型：

- 对于上面加载的模块，通过文件名是否以 ld开头。而判断是否为链接器。

```c++
static const char kLinkerName[] = "ld";

static bool IsLinker(const char* full_name) {
  return LibraryNameIs(full_name, kLinkerName);
}

// Match full names of the form /path/to/base_name{-,.}*
bool LibraryNameIs(const char *full_name, const char *base_name) {
  const char *name = full_name;
  // Strip path.
  while (*name != '\0') name++;
  while (name > full_name && *name != '/') name--;
  if (*name == '/') name++;
  uptr base_name_length = internal_strlen(base_name);
  if (internal_strncmp(name, base_name, base_name_length)) return false;
  return (name[base_name_length] == '-' || name[base_name_length] == '.');
}
```

Lsan添加的日志打印：对pc的检查。

```
caller_pc(0x5562e7daa23d), skip_linker_allocations(1)... 
Address beg:0x7fdfa744c000, end:0x7fdfa744db50, pc:0x5562e7daa23d
Address beg:0x7fdfa744e000, end:0x7fdfa7477335, pc:0x5562e7daa23d
Address beg:0x7fdfa7478000, end:0x7fdfa7482ee4, pc:0x5562e7daa23d
Address beg:0x7fdfa7484620, end:0x7fdfa74872d8, pc:0x5562e7daa23d

hex(0x7fdfa74872d8-0x7fdfa7484620)
'0x2cb8' ：正好与ld里面加载的PT_LOAD段的大小相等。
```

`readelf -e /lib64/ld-linux-x86-64.so.2`：

```
Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  LOAD           0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000001b50 0x0000000000001b50  R      0x1000
  LOAD           0x0000000000002000 0x0000000000002000 0x0000000000002000
                 0x0000000000029335 0x0000000000029335  R E    0x1000
  LOAD           0x000000000002c000 0x000000000002c000 0x000000000002c000
                 0x000000000000aee4 0x000000000000aee4  R      0x1000
  LOAD           0x0000000000037620 0x0000000000038620 0x0000000000038620
                 0x0000000000002af0 0x0000000000002cb8  RW     0x1000
```





## 2.2 栈追踪

文件：`libsanitizer/lsan/lsan.h`

- 作用：在每一个拦截函数（如：拦截malloc）。在调用的时候，需要记录调用栈信息。
- 宏：定义一个局部变量（栈对象） + 调用函数赋值此变量。

```c++
#define GET_STACK_TRACE(max_size, fast)                       \
  __sanitizer::BufferedStackTrace stack;                      \
  GetStackTraceWithPcBpAndContext(&stack, max_size,           \
                                  StackTrace::GetCurrentPc(), \
                                  GET_CURRENT_FRAME(), nullptr, fast);

#define GET_STACK_TRACE_FATAL \
  GET_STACK_TRACE(kStackTraceMax, common_flags()->fast_unwind_on_fatal)

#define GET_STACK_TRACE_MALLOC                                      \
  GET_STACK_TRACE(__sanitizer::common_flags()->malloc_context_size, \
                  common_flags()->fast_unwind_on_malloc)
```

### 1、获取pc和rbp的值

- 一个获取rip寄存器的值。
- 一个获取rbp寄存器的值。（就是当前函数的栈帧的起始地址）

```c++
#define GET_CALLER_PC() (__sanitizer::uptr) __builtin_return_address(0)
#define GET_CURRENT_FRAME() (__sanitizer::uptr) __builtin_frame_address(0)
```

### 2、栈帧记录

说明：

- `max_depth`：来自asan的一个flag的值，`common_flags()->malloc_context_size`。 asan的默认值是1。但是经过Lsan的初始化，改为了30。

```c++
static void InitializeFlags() {
  // Set all the default values.
  SetCommonFlagsDefaults();
  {
    CommonFlags cf;
    cf.CopyFrom(*common_flags());
    cf.external_symbolizer_path = GetEnv("LSAN_SYMBOLIZER_PATH");
    cf.malloc_context_size = 30;
    cf.intercept_tls_get_addr = true;
    cf.detect_leaks = true;
    cf.exitcode = 23;
    OverrideCommonFlags(cf);
  }
```

- fast：
- 协程的问题：默认情况下，是会对栈帧的地址进行校验的。。。如果当前处于一个私有栈，则会不处于当前的线程栈范围内。直接返回。

```c++
// Get the stack trace with the given pc and bp.
// The pc will be in the position 0 of the resulting stack trace.
// The bp may refer to the current frame or to the caller's frame.
ALWAYS_INLINE
void GetStackTraceWithPcBpAndContext(__sanitizer::BufferedStackTrace *stack,
                                     __sanitizer::uptr max_depth,
                                     __sanitizer::uptr pc, __sanitizer::uptr bp,
                                     void *context, bool fast) {
  uptr stack_top = 0, stack_bottom = 0;
  ThreadContext *t;
  if (fast && (t = CurrentThreadContext())) {
    stack_top = t->stack_end();
    stack_bottom = t->stack_begin();
  }
  if (!SANITIZER_MIPS || IsValidFrame(bp, stack_top, stack_bottom)) {
    stack->Unwind(max_depth, pc, bp, context, stack_top, stack_bottom, fast);
  }
}

```



### 3、x86-快速Unwind

- https://stackoverflow.com/questions/2331316/what-is-stack-unwinding

Returning from the called function will pop the top frame off of the stack, perhaps leaving a return value. The more general act of popping one or more frames off the stack to resume execution elsewhere in the program is called **stack unwinding** and must be performed when non-local control structures are used, such as those used for exception handling. In this case, the stack frame of a function contains one or more entries specifying exception handlers. When an exception is thrown, the stack is unwound until a handler is found that is prepared to handle (catch) the type of the thrown exception.

Some languages have other control structures that require general unwinding. Pascal allows a global goto statement to transfer control out of a nested function and into a previously invoked outer function. This operation requires the stack to be unwound, removing as many stack frames as necessary to restore the proper context to transfer control to the target statement within the enclosing outer function. Similarly, C has the setjmp and longjmp functions that act as non-local gotos. Common Lisp allows control of what happens when the stack is unwound by using the unwind-protect special operator.

When applying a continuation, the stack is (logically) unwound and then rewound with the stack of the continuation. This is not the only way to implement continuations; for example, using multiple, explicit stacks, application of a continuation can simply activate its stack and wind a value to be passed. The Scheme programming language allows arbitrary thunks to be executed in specified points on "unwinding" or "rewinding" of the control stack when a continuation is invoked.

```c++
void BufferedStackTrace::FastUnwindStack(uptr pc, uptr bp, uptr stack_top,
                                         uptr stack_bottom, u32 max_depth) {
  const uptr kPageSize = GetPageSizeCached();
  CHECK_GE(max_depth, 2);
  trace_buffer[0] = pc;
  size = 1;
  if (stack_top < 4096) return;  // Sanity check for stack top.
  uhwptr *frame = GetCanonicFrame(bp, stack_top, stack_bottom);
  // Lowest possible address that makes sense as the next frame pointer.
  // Goes up as we walk the stack.
  uptr bottom = stack_bottom;
  // Avoid infinite loop when frame == frame[0] by using frame > prev_frame.
  while (IsValidFrame((uptr)frame, stack_top, bottom) &&
         IsAligned((uptr)frame, sizeof(*frame)) &&
         size < max_depth) {
            
    uhwptr pc1 = frame[1];
    // Let's assume that any pointer in the 0th page (i.e. <0x1000 on i386 and
    // x86_64) is invalid and stop unwinding here.  If we're adding support for
    // a platform where this isn't true, we need to reconsider this check.
    if (pc1 < kPageSize)
      break;
    if (pc1 != pc) {
      trace_buffer[size++] = (uptr) pc1;
    }
    bottom = (uptr)frame;
    frame = GetCanonicFrame((uptr)frame[0], stack_top, bottom);
  }
}
```



### Writing a Linux Debugger Part 8: Stack unwinding

- https://blog.tartanllama.xyz/writing-a-linux-debugger-unwinding/



```

((unsigned long *)0x88dff9c0)[1]

```



## 2.3 报告



### 2.x.1 拦截调用

汇总：总共拦截15种。

```
  INTERCEPT_FUNCTION(malloc);
  INTERCEPT_FUNCTION(free);
  INTERCEPT_FUNCTION(cfree);
  INTERCEPT_FUNCTION(calloc);
  INTERCEPT_FUNCTION(realloc);
  INTERCEPT_FUNCTION(memalign);
  INTERCEPT_FUNCTION(posix_memalign);
  INTERCEPT_FUNCTION(__libc_memalign);
  INTERCEPT_FUNCTION(valloc);
  INTERCEPT_FUNCTION(pvalloc);
  INTERCEPT_FUNCTION(malloc_usable_size);
  INTERCEPT_FUNCTION(mallinfo);
  INTERCEPT_FUNCTION(mallopt);
  INTERCEPT_FUNCTION(pthread_create);
  INTERCEPT_FUNCTION(pthread_join);
```



拦截器以及其原理：

定义：

- 为某一个函数原型，提供一个封装的原型。用于替换。（如下：`libsanitizer\lsan\lsan_interceptors.cc` ， 定义了malloc和free的拦截函数。。。）

  ```c++
  INTERCEPTOR(void*, malloc, uptr size) {
    ENSURE_LSAN_INITED;
    GET_STACK_TRACE_MALLOC;
    return Allocate(stack, size, 1, kAlwaysClearMemory);
  }
  
  INTERCEPTOR(void, free, void *p) {
    ENSURE_LSAN_INITED;
    Deallocate(p);
  }
  ```

- 2、

```c++
#define INTERCEPTOR(ret_type, func, ...) \
  DEFINE_REAL(ret_type, func, __VA_ARGS__) \
  DECLARE_WRAPPER(ret_type, func, __VA_ARGS__) \
  extern "C" \
  INTERCEPTOR_ATTRIBUTE \
  ret_type WRAP(func)(__VA_ARGS__)

```

获取真实函数的地址：

- dlsym：（获取在一个共享对象或可执行文件中的一个符号的地址；）obtain address of a symbol in a shared object or executable.
- RTLD_NEXT：（，发现下一次所期望的符号出现，在本文件之后的搜索顺序之中；  提供包装器。。。 激活真实的函数其位于其他共享对象文件中。。。）Find the next occurrence of the desired symbol in the  search order after the current object.  This allows one to  <font color="#F9DDA2">provide a wrapper around a function in another shared  object</font>, so that, for example, the definition of a function  in a preloaded shared object (see `LD_PRELOAD` in ld.so(8))  can find and invoke the "real" function provided in  another shared object (or for that matter, the "next"  definition of the function in cases where there are  multiple layers of preloading).
  - 作用：对于一个要被拦截的函数名，用来获取其实际的地址。

```c++
namespace __interception {
bool GetRealFunctionAddress(const char *func_name, uptr *func_addr,
    uptr real, uptr wrapper) {
  *func_addr = (uptr)dlsym(RTLD_NEXT, func_name);
  return real == wrapper;
}

```

辅助函数：

- #func：给定一个函数，将其转化为函数名。即字符串。。。
- 功能2：拦截一个函数。。。

```c
#define INTERCEPT_FUNCTION_LINUX_OR_FREEBSD(func)                          \
  ::__interception::GetRealFunctionAddress(                                \
      #func, (::__interception::uptr *)&__interception::PTR_TO_REAL(func), \
      (::__interception::uptr) & (func),                                   \
      (::__interception::uptr) & WRAP(func))

# define INTERCEPT_FUNCTION(func) INTERCEPT_FUNCTION_LINUX_OR_FREEBSD(func)
```



### 2.x.2 报告泄漏

#### 1、DoLeakCheck：入口

- CheckForLeaks：

  - DoStopTheWorld：处于锁内，暂停整个程序的执行（停止世界），保留当前的指针图状态，以此为依据而进行内存泄漏的检查，生成报告。

    - 问题1：追踪器任务（tracer task）会被进入死锁状态，永远hang住，如何冻结的线程（threads is frozen）持有libdl的锁；

    - ？？不懂：锁是可重入的？（lock is reentrant）、libc如何区分父子线程？？？

    - `dl_iterate_phdr`：（walk through list of shared objects）遍历所有的一个应用运行所需要的动态对象，以及其加载的顺序；每一个共享库，都会调用这个回调函数；

    - StopTheWorld（略）：通过一个追踪线程（tracer thread）

      - `pthread_sigmask`：（examine and change mask of blocked signals）检查与改变阻塞信号的掩码。。。。<font color="clan"> 同步信号不应该被阻塞</font>；

      ```c
      // Synchronous signals that should not be blocked.
      static const int kSyncSignals[] = { SIGABRT, SIGILL, SIGFPE, SIGSEGV, SIGBUS,
                                          SIGXCPU, SIGXFSZ };
      ```

      - internal_clone：通过克隆来实现，无法使用包装 glibc's clone wrapper
      - 目的（猜测）：自我感觉，它需要把每一个线程都暂停。然后通过clone每一个线程，在传入的回调中而执行。从而达到检查的目的？？？

```c
// LSan calls dl_iterate_phdr() from the tracer task. This may deadlock: if one
// of the threads is frozen while holding the libdl lock, the tracer will hang
// in dl_iterate_phdr() forever.
// Luckily, (a) the lock is reentrant and (b) libc can't distinguish between the
// tracer task and the thread that spawned it. Thus, if we run the tracer task
// while holding the libdl lock in the parent thread, we can safely reenter it
// in the tracer. The solution is to run stoptheworld from a dl_iterate_phdr()
// callback in the parent thread.
void DoStopTheWorld(StopTheWorldCallback callback, void *argument) {
  DoStopTheWorldParam param = {callback, argument};
  dl_iterate_phdr(DoStopTheWorldCallback, &param);
}
```

#### 2、（🔥）ClassifyAllChunks

CheckForLeaksCallback

- ClassifyAllChunks：关键的步骤，遍历所有的chunk。为他们的元数据块设置泄漏的tag。（有无泄漏）

部分1：

- 1、先将遍历所有块，如果是分配的且标签是 `kIgnored`（要被忽略的）。则将这些块，放入Vector的frontier里面。

- 2、扫描所有的全局变量，里有着指向堆指针：（Scans global variables for heap pointers.）

  - 1、LSAN标志：`use_globals`（默认值为 true），如果是false直接return。 ===（等效）===> 全局变量持有指针，也会报告泄漏；
  - 2、🤢（略）

- 3、大概：这里本就是一个回调。 `suspended_threads`就是所有的线程；内部遍历处理；

  - 1、LSAN标志：`use_registers`（默认值为 true），如果是false直接return。 ===（等效）===>寄存器持有指针，也会报告泄漏；

  - 2、LSAN标志：`use_stacks`（默认值为 true），如果是false直接return。 ===（等效）===>寄存器持有指针，也会报告泄漏；

    - 获取当前线程的栈起始地址，栈结束地址。

      > 自我记录：
      >
      > 当我使用<font color="#F9DDA2">一个C语言协程框架时，其栈是通过mmap分配的私有栈。会超过当前范围，而打印这个日志</font>。

    ```c++
          LOG_THREADS("Stack at %p-%p (SP = %p).\n", stack_begin, stack_end, sp);
          if (sp < stack_begin || sp >= stack_end) {
            // SP is outside the recorded stack range (e.g. the thread is running a
            // signal handler on alternate stack, or swapcontext was used).
            // Again, consider the entire stack range to be reachable.
            LOG_THREADS("WARNING: stack pointer not in stack range.\n");
    ```

  - 3、LSAN标志：`use_tls`（默认值为 true），如果是false直接return。 ===（等效）===>线程局部存储持有指针，也会报告泄漏；

  - 4、🤢（具体检测实现，略。。。）

- 4、大概：这个地方的扫描的区域是通过 `__lsan_register_root_region`自己主动添加根区域。

  - LSAN标志：`use_root_regions`（默认值为 true），如果是false直接return。 ===（等效）===>**手动添加的根区域**持有指针，也会报告泄漏；

- 5、设置标签：把上面很多步骤中的添加的frontier，设值为 `kReachable`，而不报告泄漏。

```c++
  // Holds the flood fill frontier.
  Frontier frontier(1);

  ForEachChunk(CollectIgnoredCb, &frontier);
  ProcessGlobalRegions(&frontier);
  ProcessThreads(suspended_threads, &frontier);
  ProcessRootRegions(&frontier);
  FloodFillTag(&frontier, kReachable);
```

=========================

这一个步骤的代价很大，通过前面而跳过那些已经可达（reachable）的块

```c++
  // The check here is relatively expensive, so we do this in a separate flood
  // fill. That way we can skip the check for chunks that are reachable
  // otherwise.
  LOG_POINTERS("Processing platform-specific allocations.\n");
  CHECK_EQ(0, frontier.size());
  ProcessPlatformSpecificAllocations(&frontier);
  FloodFillTag(&frontier, kReachable);
```

- ProcessPlatformSpecificAllocations

```c++
// Handles dynamically allocated TLS blocks by treating all chunks allocated
// from ld-linux.so as reachable.
// Dynamic TLS blocks contain the TLS variables of dynamically loaded modules.
// They are allocated with a __libc_memalign() call in allocate_and_init()
// (elf/dl-tls.c). Glibc won't tell us the address ranges occupied by those
// blocks, but we can make sure they come from our own allocator by intercepting
// __libc_memalign(). On top of that, there is no easy way to reach them. Their
// addresses are stored in a dynamically allocated array (the DTV) which is
// referenced from the static TLS. Unfortunately, we can't just rely on the DTV
// being reachable from the static TLS, and the dynamic TLS being reachable from
// the DTV. This is because the initial DTV is allocated before our interception
// mechanism kicks in, and thus we don't recognize it as allocated memory. We
// can't special-case it either, since we don't know its size.
// Our solution is to include in the root set all allocations made from
// ld-linux.so (which is where allocate_and_init() is implemented). This is
// guaranteed to include all dynamic TLS blocks (and possibly other allocations
// which we don't care about).
void ProcessPlatformSpecificAllocations(Frontier *frontier) {
  StackDepotReverseMap stack_depot_reverse_map;
  ProcessPlatformAllocParam arg;
  arg.frontier = frontier;
  arg.stack_depot_reverse_map = &stack_depot_reverse_map;
  arg.skip_linker_allocations =
      flags()->use_tls && flags()->use_ld_allocations && linker != nullptr;
  ForEachChunk(ProcessPlatformSpecificAllocationsCb, &arg);
}
```

- ProcessPlatformSpecificAllocationsCb
  - 问题点：If `caller_pc` is <font color="red">unknown, this chunk may **be allocated in a coroutine**. Mark it as reachable, as we can't properly report its allocation stack anyway</font>.（，无法获取调用者的pc，这个块可能是在一个协程上分配的。因为无法报告，所以都当做可达块来处理。。。？？？）

```c++
// ForEachChunk callback. Identifies unreachable chunks which must be treated as
// reachable. Marks them as reachable and adds them to the frontier.
static void ProcessPlatformSpecificAllocationsCb(uptr chunk, void *arg) {
  CHECK(arg);
  ProcessPlatformAllocParam *param =
      reinterpret_cast<ProcessPlatformAllocParam *>(arg);
  chunk = GetUserBegin(chunk);
  LsanMetadata m(chunk);
  if (m.allocated() && m.tag() != kReachable && m.tag() != kIgnored) {
    u32 stack_id = m.stack_trace_id();
    uptr caller_pc = 0;
    if (stack_id > 0)
      caller_pc = GetCallerPC(stack_id, param->stack_depot_reverse_map);
    // If caller_pc is unknown, this chunk may be allocated in a coroutine. Mark
    // it as reachable, as we can't properly report its allocation stack anyway.
    if (caller_pc == 0 || (param->skip_linker_allocations &&
                           linker->containsAddress(caller_pc))) {
      m.set_tag(kReachable);
      param->frontier->push_back(chunk);
    }
  }
}
```





#### 3、LeakReport：报告对象

要点记录：

- 检测原理：每一个内存块，<font color="red">默认值都 `kDirectlyLeaked`。而在停止世界时，会从根区域（root）开始遍历每一个块。如果可以到达某一个块，则修改类型为 `kReachable` </font>等。。。 这样：就能报告泄漏的地方。

```c
对于每一块内存区域, 有着元数据管理。每一个分配出的chunk块，都对应着一个元数据区域。
chun_ptr
 ↓   96字节
[[        chunk       ] [        chunk       ] [        chunk       ]......[meat data][meat data][meat data]]

  
 
 struct ChunkMetadata {
  u8 allocated : 8;  // Must be first. // memory_order用来标记一个内存块的情况。。。。
  ChunkTag tag : 2;  //4种类型：1、可达 2、直接泄漏 3、间接泄漏 4、忽略
  uptr requested_size : 54;
  u32 stack_trace_id;
};

// Chunk tags.
enum ChunkTag {
  kDirectlyLeaked = 0,  // default
  kIndirectlyLeaked = 1,
  kReachable = 2,
  kIgnored = 3
};

enum memory_order {
  memory_order_relaxed = 1 << 0,
  memory_order_consume = 1 << 1,
  memory_order_acquire = 1 << 2,
  memory_order_release = 1 << 3,
  memory_order_acq_rel = 1 << 4,
  memory_order_seq_cst = 1 << 5
};
```

AddLeakedChunk：添加一个泄漏的对象

- 使用内部的mmap的vector来管理泄漏对象。步骤：1、遍历vector，如果栈id和泄漏类型相同。则对同一个泄漏对象进行统计   2、如果没有相同的，则创建一个新的Leak对象，放入push到vector中； 3、如果有标志位 `report_objects`，则记录每一个对象的信息到另一个Vector中；（不会聚合到一起，遇到一个直接push）

```c++
struct Leak {
  u32 id;  //自增计数, 用作标识。
  uptr hit_count;  //相同栈id与泄漏类型：命中的次数。。。
  uptr total_size;  //泄漏的总大小
  u32 stack_trace_id;   //栈追踪id, 记录打印。
  bool is_directly_leaked; //泄漏一共2个类型：bool值标记
  bool is_suppressed; //是否被屏蔽
};

struct LeakedObject {
  u32 leak_id;
  uptr addr;
  uptr size;
};
```




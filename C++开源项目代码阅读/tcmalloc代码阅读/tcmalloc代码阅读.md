地址：https://github.com/google/tcmalloc

阅读策略：先看文档，再看代码。





```
amortize： 分期偿还；摊还
scavenger ：拾荒者;食腐动物;
```



## ch0 记录



doc && 第一次：

- TC（Thread Caching）、allocation frameworks、raw memory requests（例子：`mmap()`）、memory cache（目的：避免系统调用，方便后续的 re-allocated）
- detailed **insight**（into  xxx）、**hugepage aware** pageheap
- 不懂的：
  - restartable sequences (RSEQ)、legacy behavior、bookkeeping
- 重要的：
  - these cache implementations allows TCMalloc to **avoid requiring locks** for most memory allocations and deallocations



翻译记录：

- size class：尺寸类别



## common.h

Configuration：宏配置

- There are four different models for tcmalloc which are created by defining a // set of constant variables differently:
  - DEFAULT:  The default configuration strives for good performance while trying to  minimize fragmentation.  It uses a smaller page size to reduce  fragmentation, but allocates per-thread and per-cpu capacities similar to  `TCMALLOC_INTERNAL_32K_PAGES` / `TCMALLOC_INTERNAL_256K_PAGES`.
  - TCMALLOC_INTERNAL_32K_PAGES：
    -  Larger page sizes (32KB) increase the bookkeeping granularity used by  TCMalloc for its allocations.  This can reduce PageMap size and traffic to  the innermost cache (the page heap), but can increase memory footprints. As  TCMalloc will not reuse a page for a different allocation size until the  entire page is deallocated, this can be a source of increased memory  fragmentation.
    -  Historically, larger page sizes improved lookup performance for the  pointer-to-size lookup in the PageMap that was part of the critical path.  With most deallocations leveraging C++14's sized delete feature  (https://isocpp.org/files/papers/n3778.html), this optimization is less  significant.
  - TCMALLOC_INTERNAL_256K_PAGES：This configuration uses an even larger page size (256KB) as the unit of  accounting granularity.
  - TCMALLOC_INTERNAL_SMALL_BUT_SLOW： Used for situations where minimizing the memory footprint is the most  desirable attribute, even at the cost of performance.



## size_class_info.h



### SizeClassInfo：

- Ds：（提前计算好的尺寸类别参数）Precomputed size class parameters.
- size：Max size storable in that class
- pages：Number of pages to allocate at a time
- num_to_move： Number of objects to move between a per-thread list and a central list in  one shot.  We want this to be not too small so we can amortize the lock  overhead for accessing the central list.  Making it too big may temporarily  cause unnecessary memory wastage in the per-thread free list until the  scavenger cleans up the list.
- max_capacity： Max per-CPU slab capacity for the default 256KB slab size.  Scaled up/down for larger/smaller slab sizes.



## size_classes.cc



Columns in the following tables:

- bytes: size of the size class
- pages: number of pages per span
- batch: preferred number of objects for transfers between caches
- cap: （最大cpu缓存容量）maximum cpu cache capacity
- class: （尺寸类别编号）size class number
- objs: number of objects per span
- waste/fixed:  fixed per-size-class overhead due to end-of-span fragmentation and other factors. For instance, if we have a 96 byte size class, and use a single 8KiB page, then we will hold 85 objects per span, and have 32 bytes left over. There is also a fixed component of 48 bytes of TCMalloc metadata per span. Together, the fixed overhead would be wasted/allocated = (32 + 48) / (8192 - 32) ~= 0.98%.
- waste/sampling: overhead due to heap sampling(rounding to page size, proxy object, metadata).
- inc: increment from the previous size class. This caps the dynamic overhead component based on mismatches between the number of bytes requested and the number of bytes provided by the size class. Together they sum to the total overhead; for instance if you asked for a 50-byte allocation that rounds up to a 64-byte size class, the dynamic overhead would be 28%, and if waste were 22% it would mean (on average) 25 bytes of overhead for allocations of that size.

大小分为80个大小类别：

```c
static constexpr SizeClassInfo List[] = {
//                                         |    waste     |
//  bytes pages batch   cap    class  objs |fixed sampling|    inc
  {     0,    0,    0,    0},  //  0     0  0.00%    0.00%   0.00%
    
  {262144,   32,    2,  128},  // 80     1  0.02%    0.03%  14.29%
};
```


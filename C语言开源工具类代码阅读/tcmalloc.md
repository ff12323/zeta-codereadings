Columns in the following tables:
- bytes: size of the size class
- pages: number of pages per span
- batch: preferred number of objects for transfers between caches
- cap: maximum cpu cache capacity
- class: size class number
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


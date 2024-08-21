



# rte_common.h

目的：Generic, commonly-used macro and inline function definitions for DPDK.



__rte_aligned：强制对齐

- 源码：使用编译器的对齐功能

```
#define __rte_aligned(a) __attribute__((__aligned__(a)))
```


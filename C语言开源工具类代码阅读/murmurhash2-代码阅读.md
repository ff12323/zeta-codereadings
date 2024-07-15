







### 1、abrandoned / murmur2

- https://github.com/abrandoned/murmur2

MurmurHash2

- Note - This code makes a few assumptions about how your machine behaves -
  1. We can read a 4-byte value from any address without crashing
  2. sizeof(int) == 4

  And it has a few limitations -

  1. It will not work incrementally.
  2. （，大小端结果不同）It will not produce the same results on little-endian and big-endian machines.  

- 'm' and 'r' are mixing constants generated offline. They're not really 'magic', they just happen to work well.

  - m = 0x5bd1e995
  - r = 24

- 。。。

```
MurmurHash2：
  Ds： 哈希函数
  Para：
    key：key的指针
    len：key的长度（字节数）
    seed：随机数种子？？？
  Ret：哈希值
```


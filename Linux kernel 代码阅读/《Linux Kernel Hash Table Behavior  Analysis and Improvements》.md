论文信息：《Linux Kernel Hash Table Behavior:  Analysis and Improvements》
作者：Chuck Lever, Sun-Netscape Alliance
地址：http://www.citi.umich.edu/techreports/reports/citi-tr-00-1.pdf



```
 hundredths：  a hundredth is a single part of something that has been divided equally into a hundred parts.
 nominal state： 名义状态量
 skewed： deviating from what is normal, direct, or accurate
 degeneration： Degeneration is a process of decline. Anything that's getting worse is going through degeneration. 
 prune：to cut off unwanted branches from a tree, bush, or other plant
 prudent：wise or judicious in practical affairs;
```



## ch0 记录



记录：

- ch02：
  - SPEC SDM benchmark suite
- ch05：
  - （，关键问题：哈希函数应该耗费多少CPU周期，而得到一个合理的桶大小分布？。。。）how many CPU cycles  should be spent by the hash function to get a  reasonable bucket size distribution? In most practical  situations, a simple shift-add function suffices.  However, one should always test with actual data  before deciding on a hash function implementation. 



不懂的索引：

- ch01：
  - Lookups on longer chains can push  useful data out of CPU caches
- ch03：
  - 3.3：
    - Some may argue that shrinking the dcache  unnecessarily might lower the overall effectiveness of  the cache, but we believe that shrinking the cache more  aggressively will help, rather than hurt, overall system  performance <u>because a smaller cache allows faster  lookups and causes less CPU cache pollution</u>

英语不懂：

- a  multiplicative hash might **be inferior** even if it cuts the  average number of loop iterations per lookup request  by **a factor of four or more**. （google： 即使乘法哈希将每个查找请求的平均循环迭代次数减少四倍或更多，它也可能表现较差。）



## ch01 介绍

Hash tables are a venerable and well-understood data  structure often used for high-performance applications  because of their excellent average lookup time. Linux,  an open-source POSIX-compliant operating system,  relies on hash tables to manage pages, buffers, inodes,  and other kernel-level data objects.

Why worry about the kernel’s hash tables? As we  show, Linux performance depends on the efficiency  and scalability of these tables. On a small machine  with 32M of physical RAM, a page cache hash table  with 2048 buckets is probably large enough to hold all  possible cache pages in chains shorter than three.  However, a hash table this small cannot hold all  possible pages on a larger machine with, say, 512M of  physical RAM while maintaining short chains to keep  lookup times quick. Keeping hash chains short is even  more important on modern CPUs because of the effects  of CPU cache pollution on overall system  performance. <u>Lookups on longer chains can push  useful data out of CPU caches</u>. 

> ？？？ 为什么会导致缓存过期。


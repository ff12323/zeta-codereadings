

## ch01 rte_memzone.h

RTE Memzone

> 翻译Note：
>
> memory area：内存地带
>
> memory zone：内存区

（，内存区分配器的目标是预留物理内存连续的部分；这些区被一个名字标识；）The goal of the memzone allocator is to reserve contiguous portions of physical memory. These zones are identified by a name.

The memzone descriptors are shared by all partitions and are located in a known place of physical memory. This zone is accessed using rte_eal_get_configuration(). The lookup (by name) of a memory zone can be done in any partition and returns the same physical address.

（，一个预留的内存区不能被**去预留**。预留应该只能在初始化时完成；）A reserved memory zone cannot be unreserved. The reservation shall be done at initialization time only.

- 【？？？】为什么做这样的限制？

---

### 1、工具宏

内存区大小：

- Use 2MB pages
- Use 1GB pages.
- Use 16MB pages.
- Use 16GB pages.
- Use 256KB pages.
- Use 256MB pages
- Use 512MB pages.
- Use 4GB pages.

RTE_MEMZONE_NAMESIZE（32）：（内存区名字的最大长度；）Maximum length of memory zone name

> 》：宏不一定要定义在文件的某个区域，可以在用的地方定义。



rte_memzone： A structure describing a memzone, which is a contiguous portion of physical memory identified by a name.

- name[32]：Name of the memory zone.
- iova：（起始IO地址）Start IO address.
- 匿名联合体：
  - addr：Start virtual address.
  - addr_64：（，U64，占位确保大小）Makes sure addr is always 64-bits
- len：Length of the memzone.
- hugepage_sz：The page size of underlying memory
- socket_id：NUMA socket ID.
- flags：（内存区的特征）Characteristics of this memzone



## ch02 rte_ring_core.h

### 1、结构体

rte_ring：An RTE ring structure. 

- // （，生产者与消费者有着一个头部索引 与 尾部索引；）The producer and the consumer have a head and a tail index. The particularity of these index is that they are not between 0 and `size(ring)-1`. These indexes are between 0 and 2^32 -1, and **we mask their value** when we access the `ring[]` field. Thanks to this assumption, <u>we can do subtractions between 2 index values in a modulo-32bit base: that's why the overflow of the indexes is not a problem</u>.
  - 【？？？】 什么是对value进行掩码？
  - 【？？？】为什么索引的溢出不是一个问题？
- 
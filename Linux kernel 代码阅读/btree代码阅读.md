

DOC: B+Tree basics

A B+Tree is a data structure for looking up arbitrary (currently allowing unsigned long, u32, u64 and 2 * u64) keys into pointers. The data structure is described at https://en.wikipedia.org/wiki/B-tree, we currently do not use binary search to find the key on lookups.

Each B+Tree consists of a head, that contains bookkeeping information and a variable number (starting with zero) nodes. Each node contains the keys and pointers to sub-nodes, or, for leaf nodes, the keys and values for the tree entries.

Each node in this implementation has the following layout:

```
 [key1, key2, ..., keyN] [val1, val2, ..., valN]
```

Each key here is an array of unsigned longs, `geo->no_longs` in total. The number of keys and values (N) is geo->no_pairs.







see http://programming.kicks-ass.net/kernel-patches/vma_lookup/btree.patch

A relatively simple B+Tree implementation.  I have written it as a learning exercise to understand how B+Trees work.  Turned out to be useful as well.

B+Trees can be used similar to Linux radix trees (which don't have anything in common with textbook radix trees, beware).  Prerequisite for them working well is that access to a random tree node is much faster than a large number of operations within each node.

Disks have fulfilled the prerequisite for a long time.  More recently DRAM has gained similar properties, as memory access times, when measured in cpu cycles, have increased.  Cacheline sizes have increased as well, which also helps B+Trees.

Compared to radix trees, B+Trees are more efficient when dealing with a **sparsely populated address space**.  Between 25% and 50% of the memory is occupied with valid pointers.  When densely populated, radix trees contain ~98% pointers - hard to beat.  Very sparse radix trees contain only ~2% pointers.

This particular implementation stores pointers identified by a long value. Storing NULL pointers is illegal, lookup will return NULL when no entry was found.

（，。。特点：1、最小值在右边，而不是在左边  2、节点上所有已使用的槽位在左边，未使用的值为NULL；）A tricks was used that is not commonly found in textbooks.  The lowest values are to the right, not to the left.  All used slots within a node are on the left, all unused slots contain NUL values.  Most operations simply loop once over all slots and terminate on the first NUL.



### 1、结构体

btree_head：b树头部

- node: the first node in the tree
- mempool: mempool used for node allocations
- height: current of the tree



btree_geo：（树的几何大小）btree geometry

- keylen：单个key的以（unsigned long）为单位的长度（支持32、64、128）
- no_pairs：key-val对的个数（key的个数）
- no_longs：（unsigned  long）key的个数

### 2、工具宏

BUG_ON：

- 如果不开启调试特性，直接跳过。
- 否则，cond为假，则assert失败。

```c
#ifdef NDEBUG
#define BUG_ON(cond) do { if (cond) {} } while (0)
#else
#define BUG_ON(cond) assert(!(cond))
#endif
```



### 3、API

#### btree_lookup 

- 如果树的高度为0，返NULL
- 。。。

```
btree_lookup：
	Ds：（在b树中查找一个key）look up a key in the btree
	Para：
        @head: the btree to look in
        @geo: the btree geometry
        @key: the key to look up
    Ret：
       1、如果查找到，返回key对应的val
       2、未找到，返NULL
```



#### btree_insert：

- 调试判断 val不为空
- 返回 ，调用 `btree_insert_level `的结果（level为1）

```
btree_insert 
  Ds：（插入一个项到b树中）insert an entry into the btree
  Para：
    @head: the btree to add to
    @geo: the btree geometry
    @key: the key to add (must not already be present)
    @val: the value to add (must not be %NULL)
    @gfp: allocation flags for node allocations
 Ret：
  1、returns 0 if the item could be added 
  2、or an error code if it failed (may fail due to memory pressure).
     -ENOMEM：内存分配失败
```

btree_insert_level

- 如果b树高度 小于 @level：
  - （func：b树高度增加，头结点移到下一层）
- flag：retry
- 

```
  Ds：
  Para：
    【upper】：同上，5个参数，上层API传入。
    level：插入的key，在树中的高度。
 Ret：
    【upper】：同上
```

btree_grow

- 从内存池分配新节点
- 如何头结点不为NULL：
  - 获取头结点最后一个key的下标
  - 将最后一个key，设置到（新节点）的第1对的key
  - 设置到（新节点）的第1对的val，为头节点指针。
- 修改@b树头部为（新节点），高度++
- 返回 0

```
btree_grow：
  Ds：b树高度增加，头结点移到下一层
  Ret：
     -ENOMEM：失败
     0：成功
```

getfill：返回@b树节点的 @start为起始下标，@b树几何的key-val数量为终止，的第一个为NULL的下标（全满则是越界的下标，不存在的后一个）

find_level

```
find_level：
  Ds：locate the correct leaf node in the btree
  Para：
    head：
    geo：
    key：
    level：
  Ret：返回key所在的叶子节点
```



#### btree_last：

- 当前节点 为 起始节点
- 如果树的高度为0，返NULL
- 遍历b树的（中间节点的）高度：
  - 获取当前节点的第1项val（最大值），成为（当前节点）
- （当前节点）此时是叶子节点，获取其第1项的key，copy 到 @key
- 返回 第1项的val

```
btree_last：
	Ds： （获取b树中的最后一项的val）get last entry in btree
	Para：
        @head: btree head
        @geo: btree geometry
        @key: 【out】last key
    Ret：
        1、the last entry in the btree, and sets @key to the key of that entry; 
        2、NULL if the tree is empty, in that case key is not changed.
```



### 4、内部工具函数

bkey：返会b树节点中第（n）个key的指针

- @node：b树节点
- @n：key的下标
- 从b树的几何中获取key的单位长度， 单位长度 x 下标 = n-th节点的起始下标，数组取值并返回。

bval：返回b树节点中第（n）个val，val是（void *）指针

- @node：b树节点
- @n：val的下标
- 从b树的几何中获取key的（unsigned long）的个数为起始点，加上n的下标，访问节点数组。（unsigned long） ==> （void *）

setkey：设置b树节点中第（n）个key的值

- 。。。

setval：设置b树节点中第（n）个val的值

- 。。。

clearpair：设置b树节点中第（n）个key-val 对的值为0

- 。。。





#### 4.2 longxxx类工具

longcmp

```
longcmp：
  Ds：从0开始往后进行整数比较，比较第一个不同的大小情况
  Para：
    l1；（unsigned long）数组
    l2：同上
    n：数组长度（约束：二者等长）
  Ret：
    -1：第一个不同的，l1更小
    1：第一个不同的，l1更大
    0：无不同，二者相同
```




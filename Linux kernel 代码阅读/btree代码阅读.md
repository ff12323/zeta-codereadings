

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

A tricks was used that is not commonly found in textbooks.  The lowest values are to the right, not to the left.  All used slots within a node are on the left, all unused slots contain NUL values.  Most operations simply loop once over all slots and terminate on the first NUL.



### 1、结构体

btree_head：b树头部

- node: the first node in the tree
- mempool: mempool used for node allocations
- height: current of the tree



btree_geo：btree geometry

- keylen：key的长度（支持32、64、128）
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
- 

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

```
  Ds：
  Para：
    【upper】：同上，5个参数，上层API传入。
    level：插入的key，在树中的高度。
 Ret：
    【upper】：同上
```



### 4、内部工具函数
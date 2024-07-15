

英语生词：

```
Shard：a piece or fragment of a brittle substance （碎片）
```



【静态常量，kBlockSize ，4096，块大小】



### **Arena**：内存范围

- Ds： 内存范围对象，对于小块的内存分配，统一用块大小管理（可以支持对齐/不对齐的地址），可以避免内存碎片化？而大块的内存直接分配。
- 私有属性：
  - alloc_ptr_：记录分配的状态，多分配的内存，其中剩余的空间的起始地址。
  - alloc_bytes_remaining_：剩余空间的大小。
  - blocks_：块列表，记录分配的起始地址，方便后续释放内存。

Allocate：注意，使用new分配的内存地址，总是对齐的。

- @bytes：字节数
- Ret：
  - （，返回一个指向新分配的内存块的指针，大小为xxx字节）Return a pointer to a newly allocated memory block of "@bytes" bytes.
- // The semantics of what to return are a bit messy if we allow 0-byte allocations, so we disallow them here (we don't need them for our internal use).
  - 内部使用，assert @字节数 > 0

- 如果 @字节数 小于等于 [剩余空间大小]：
  - 从 剩余的 直接分配，并返回 地址。

- 否则剩余空间被丢弃（浪费），返回（func：直接分配内存块）。



AllocateAligned：同上，带上对齐。

- 获取对齐大小，其必须是 2^n，符合公式（1.1）
- 将 [剩余的空间的起始地址] & (对齐 - 1) 得到取余后的不对齐的大小。
- 如果 对齐后的字节数 小于等于 [剩余空间大小]：
  - 【。。。】从 剩余的 对齐开始的地方进行分配。并返回
- 否则 ，返回（直接分配内存块）

$$
x \& (x -1) = 2^{n} , n=0,1,2,..., x \in \Z^{+}  \\
  y \& (2^n -1) = modval\tag{1.1}
$$

===private

AllocateFallback：

- @bytes：分配字节数
- 如果 分配字节数 大于 （块大小） 的1/4：
  - // Object is more than a quarter of our block size.  Allocate it separately to avoid wasting too much space in leftover bytes.
  - 返回（func：直接分配内存块）
- // （当前的块剩余空间可能存在浪费）We waste the remaining space in the current block.
  - （func：直接分配内存块），大小为（**块大小**）
  - 减去实际使用大小，设置[剩余的空间的起始地址] 以及[剩余的空间大小]

- 返回分配的地址

AllocateNewBlock：直接分配内存块。

- @block_bytes：
- new参数指定的内存块，将地址放到 [块列表]中，[内存统计] fetch_add更新。返回分配的地址。





### Varint

Endian-neutral encoding:

- （，固定长度的数组被编码为LSB优先）Fixed-length numbers are encoded with least-significant byte first
- In addition we support variable length "varint" encoding
- Strings are encoded prefixed by their length in varint format



EncodeVarint64：

- 定义一个常量B，其值为128（0b10000000）。这是因为varint编码使用7位来表示数值，最高位用来标识是否还有下一个字节。
- 将dst指针转换为一个uint8_t类型的指针ptr，这样可以直接操作内存中的字节。
- while  v 大于等于 B： 说明varint的字节还有后续。
  - 将v的值与B进行按位或操作（取v的低7位，且MSB置1），然后将结果存储在ptr指向的内存中，并将ptr向后移动一位。
  - 将v右移7位。去除已编码的内容，继续。
- 当v小于B时，将v的值转换为一个uint8_t类型，然后存储在ptr指向的内存中，并将ptr向后移动一位。

```
DS： 对64位整数进行varint64编码
Para：
  dst：varint编码的buffer，起始地址（include）
  v：被编码的，64位无符号整数
Ret：
  varint编码的buffer，结束地址（not include）
```



### **BloomFilterPolicy**：



## cache.cc【❓🤢 。。。】



---

LRU cache implementation

（，缓存项有着一个"in_cache"标志位而指示缓存是否对这个项有着一个引用。。。）Cache entries have an "in_cache" boolean indicating whether the cache has a reference on the entry.  The only ways that this can become false without the entry being passed to its "deleter" are via Erase(), via Insert() when an element with a duplicate key is inserted, or on destruction of the cache.

（，缓存对其中的项维持在2个链表中。在缓存中的所有项在其中一个链表上，不能同时在2个上面。任然被客户端所引用的项，但从缓存中擦除则不在2个链表上。。）The cache keeps two linked lists of items in the cache.  All items in the cache are in one list or the other, and never both.  Items still referenced by clients but erased from the cache are in neither list.  The lists are：

- in-use：  contains the items currently referenced by clients, in no particular order.  (This list is used for invariant checking.  If we removed the check, elements that would otherwise be on this list could be left as disconnected singleton lists.)
- LRU:  contains the items not currently referenced by clients, in LRU order Elements are moved between these lists by the Ref() and Unref() methods, when they detect an element in the cache acquiring or losing its only external reference.

An entry is a variable length heap-allocated structure.  Entries are kept in a circular doubly linked list ordered by access time.

### **LRUHandle**：缓存项

- value：值
- deleter：删除函数指针
- next_hash：
  - 目的：构成哈希桶上的链表

- next，prev：双向循环链表
  - 目的：链接缓存的2个链表（使用链 或者 缓存链）；

- charge：耗费计量
- key_length：key的长度。
- in_cache：标志位，（该项是否在缓存中）Whether entry is in the cache.
- refs：（引用，包含缓存引用，如果存在）References, including cache reference, if present.
- hash：（，对key的哈希值，用于快速分片和比较）Hash of key(); used for fast sharding and comparisons
- `char key_data[1]`：（柔性数组：存放变成的key）Beginning of key

---

（，缓存是一个接口而映射键到值；。。。；其会自动地去除项而为新的项让出空间。值有着特定的耗费对于缓存容量；例如，一个缓存的值是变长字符串，可能会使用字符串的长度作为字符串的耗费。）A Cache is an interface that maps keys to values.  It has internal synchronization and may be safely accessed concurrently from multiple threads.  It may automatically evict entries to make room for new entries.  Values have a specified charge against the **cache capacity**.  For example, a cache where the values are variable length strings, may use the length of the string as the charge for the string.

A builtin cache implementation with a least-recently-used eviction policy is provided.  Clients may use their own implementations if they want something more sophisticated (like scan-resistance, a custom eviction policy, variable cache sizing, etc.)

### 【❓】**HandleTable**：Cache自定义哈希表

- Ds：（，为什么使用自定义的哈希表？）// We provide our own simple hash table since it removes a whole bunch of **porting hacks** and is also faster than some of the built-in hash table implementations in some of the compiler/runtime combinations we have tested.  E.g., readrandom speeds up by ~5% over the g++ 4.4.3's builtin hashtable.



#### Insert：

- 寻找合适位置。
- 替换上去
- 如果旧的不存在
  - 元素个数_ ++
  - 如果 元素个数_  大于  桶个数_
    - （func：Resize） // Since each cache entry is fairly large, **we aim for a small average linked list length** (<= 1).
- 返回旧的

```
Ds：插入一个缓存项（key在哈希表中已经存在/不存在）
Para：
  h：
ret：
  1、旧的缓存项：存在同key
  2、null：不存在
```

#### 【❓】Remove：

> ?：为什么这里的哈希链修改是这个样子的，无法理解。

```
ret：
  1、旧的缓存项：存在同key
  2、null：不存在
```



#### 【私有属性】：

- // （哈希表包含一个桶数组，每个桶都是一个哈希在这个桶的缓存项的链表）The table consists of an array of buckets where each bucket is a linked list of cache entries that hash into the bucket.
  - length_：桶个数，数组的长度
  - elems_：元素的个数。
  - list_：缓存项二维指针（数组）

#### FindPointer：

- @哈希值  &  长度，而取得桶；
- while 遍历桶链表，找到匹配的缓存项并返回。

```
Para：
  key：
  hash：
ret：二维指针
  1、Return a pointer to slot that points to a cache entry that matches key/hash.  
  2、If there is no such cache entry, return a pointer to the trailing slot in the corresponding linked list.
```

#### Resize：❓

- 按照2倍增长，直到大于 元素个数_
- new  分配新的数组空间
- 遍历 原先的 哈希桶数组：
  - 获取桶地址
  - while 桶元素不为 null：遍历缓存项
    - 【？？？】

```

```



### **LRUCache**：内部实现

- Ds：（分片哈希的一个单个分片）A single shard of sharded cache.

【。。。】

（，隐藏结构体：对于缓存中的项的隐藏表示。）Opaque handle to an entry stored in the cache.

- 参考：https://stackoverflow.com/questions/3854113/what-is-an-opaque-value-in-c

```c
struct Handle {};
```

#### Insert：

- 加锁（互斥帮助类，构造器内加锁；范围：函数内）。
- 分配 缓存项 的内存，总大小 = 结构体大小 - 1 + key大小 
  - Note：减1，是因为柔性数组分配，但是属性字段里填充的是1，不是0。
- 初始化项：
  - 引用计数置 1  // （表示项的引用）for the returned handle.
- 如果 容量_ 大于 0：插入缓存
  - 缓存项的引用计数 ++，在缓存中的标志位设置true  // （表示缓存的引用）for the cache's reference.
  - 加入到  使用链_ 尾部
  - 使用量_ += @消耗
  - 哈希表_ 插入替换 缓存项，并删除旧的 项；
- 否则：// （，缓存关闭）don't cache. (capacity_==0 is supported and turns off caching.)
  - // next is read by key() in an assert, so it must be initialized
    - 缓存项的 next指针 设置为 null

- while 使用量_  大于 容量_  且 LRU链的next不是指向自身： // 容量不够，淘汰释放LRU链上的旧项
  - 获取头部项。
  - 哈希表_ 删除头部项，并释放删除 内存；
  - 断言：已经删除 // to avoid unused variable when compiled NDEBUG

- 返回 缓存项


```
Para：
  【upper】：同分片LRU的上层传入的4个参数
  hash：key的哈希值
ret：新分配的缓存项
```

#### Lookup：

```
Ds： 在缓存中查找key
Para：
  key：
Ret：
  1. If the cache has no mapping for "key", returns nullptr.
  2. Else return a handle that corresponds to the mapping.
Caller：
  The caller must call this->Release(handle) when the returned mapping is no longer needed.
```



---

#### 私有属性：

- capacity_：（，容量）Initialized before use.
- mutex_：（互斥锁，包含如下状态）protects the following state.
- `GUARDED_BY(mutex_)`
  - usage_： 使用量，源自耗费。
  - lru_：（，lru链：）Dummy head of LRU list.
    - （，新的项插入到链表尾部；旧的项在链表头部）`lru.prev` is newest entry, `lru.next` is oldest entry.
    - Entries have refs==1 and in_cache==true.
  - in_use_：（，使用链：）Dummy head of in-use list.
    - Entries are in use by clients, and have refs >= 2 and in_cache==true.
  - table_：缓存自定义的哈希表。（用于快速查找）

#### Ref：

- 如果 @缓存项的引用数为1，且在在缓存中。// If on `lru_` list, move to `in_use_` list.
  - 将@缓存项从缓存双向链表上移除。
  - 将@缓存项插入到 使用链_ 的尾部。
- @缓存项的引用计数++

```
Para：
  e： 缓存项

```

#### Unref：

- @缓存项的引用计数--
- 如果@缓存项的引用计数为0：deallocate
  - 调用删除函数指针；
  - free释放@缓存项
- 否则如果： @缓存项 在 使用链_ 且 引用计数为1：No longer in use; move to `lru_` list.
  - 将@缓存项从缓存双向链表上移除。
  - 将@缓存项插入到 LRU链_ 的尾部。

#### FinishErase：

- e = @缓存项
- 如果 e 不为 null：
  - 断言：e在缓存中。
  - e从缓存双向链表上移除。
    - 【？？？】：这里是否意味着 e 的引用计数已经为1，Unref是直接删除；
  - e的在缓存的标志位，设置false；
  - 使用量_ 减少
  - （func：unref）
- 返回 标志位

```
Ds： If e != nullptr, finish removing *e from the cache; it has already been removed from the hash table.
ret：标志位
  Return whether e != nullptr.
```

LRU_Remove：将@缓存项从缓存双向链表上移除。

LRU_Append：Make "e" newest entry by inserting just before *list



【静态常量，kNumShardBits，4， 分片比特数】

【静态常量，kNumShards，16 = 2^（分片比特数）， 分片数】

### **ShardedLRUCache**：【I】分片LRU（封装）

- 父类：Cache

#### Insert：

- 计算给定值的哈希值
- 由哈希值，进行比特位偏移，得到分片下标。而得到实际插入的LRU缓存。
- 返回 （func：LRU缓存。插入）

```
Ds： Insert a mapping from key->value into the cache and assign it the specified charge against the total cache capacity.
  1. When the inserted entry is no longer needed, the key and value will be passed to "deleter".
Para：
  key：
  value：
  charge：
  deleter：
Ret：
  Returns a handle that corresponds to the mapping.  
Caller：
  The caller must call this->Release(handle) when the returned mapping is no longer needed.
```

---

#### 私有属性：

- shard_[16]（常量：分片数）：LRU缓存数组
- last_id_：





## logging.cc





## 附录A



### 1、《protocol buffers，Base 128 Varints》

（，变长整数。这种编码允许无符号64位整数使用在1字节到10字节之间，小的值使用更少的字节。）Variable-width integers, or *varints*, are at the core of the wire format. <u>They allow encoding unsigned 64-bit integers using anywhere between one and ten bytes, with small values using fewer bytes.</u>

> 使用场景：这种编码方式常用于网络通信和数据存储，特别是在需要高效存储和传输数字数据的场景中。

（，varint的每个字节的MSB是一个**连续标志位**，表示后面一个字节是不是varint的一部分；而低位的7比特是负载；）Each byte in the varint has a *continuation bit* that indicates if the byte that follows it is part of the varint. This is the *most significant bit* (MSB) of the byte (sometimes also called the *sign bit*). The lower 7 bits are a payload; <u>the resulting integer is built by appending together the 7-bit payloads of its constituent bytes.</u>

So, for example, here is the number 1, encoded as ``01`` – it’s a single byte, so the MSB is not set:

```proto
0000 0001
^ msb
```

And here is 150, encoded as ``9601`` – this is a bit more complicated:

```proto
10010110 00000001
^ msb    ^ msb
```

How do you figure out that this is 150? First you drop the MSB from each byte, as this is just there to tell us whether we’ve reached the end of the number (as you can see, it’s set in the first byte as there is more than one byte in the varint). These 7-bit payloads are in little-endian order. Convert to big-endian order, concatenate, and interpret as an unsigned 64-bit integer:

```proto
10010110 00000001        // Original inputs.
 0010110  0000001        // Drop continuation bits.
 0000001  0010110        // Convert to big-endian.
   00000010010110        // Concatenate.
 128 + 16 + 4 + 2 = 150  // Interpret as an unsigned 64-bit integer.
```

Because varints are so crucial to protocol buffers, in protoscope syntax, we refer to them as plain integers. `150` is the same as ``9601``.








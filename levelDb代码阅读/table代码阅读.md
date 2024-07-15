



```
extent：the area covered by something.（范围）
```





## ch0 自我记录

第一次的记录：

- iterator_wrapper.h：
  -  A **internal wrapper class** with an interface similar to Iterator that  caches the valid() and key() results for an underlying iterator.  <u>This can help avoid virtual function calls and also gives better **cache locality**.</u>



### 索引：

- block.cc：
  - Block -> 私有属性：data block结构图
    - 注意：<font color="#F9DDA2">index block的编码方式与 data block是一样的</font>（虽然二者的key，value不同）
- format.cc
  - 头部：
    - 物理块结构图
    - ReadBlock：读取物理块 + crc校验 + 压缩类型处理（解压）
  - footer：footer结构图
- table.cc：
  - rep：sstable的逻辑结构图
- block_build.cc：
  - data block的entry 结构图
- table_build.cc：
  - `TableBuilder::Finish`：写入管理数据区域：filter block、metaindex block、index block、footer

- filter_block.cc：
  - FilterBlockBuilder：filter block的结构图
    - GenerateFilter： call  布隆过滤器
    - 写入（创建）filter block的函数调用，正则表达式： `(StartBlock AddKey*)* Finish`
      - GenerateFilter




### 问题记录

1、【ok】为什么index block 使用 block对象表示？这个迭代器内部实现，明显是data block的内容迭代？

- 自答：<font color="#F9DDA2">index block的编码方式与 data block是一样的</font>（虽然二者的key，value不同）

```c++
Status Table::InternalGet(const ReadOptions& options, const Slice& k, void* arg,
                          void (*handle_result)(void*, const Slice&,
                                                const Slice&)) {
  Status s;
  Iterator* iiter = rep_->index_block->NewIterator(rep_->options.comparator);
  iiter->Seek(k);
```

2、【ok】构造函数中，参数的类型，与字段类型不一样？

index_iter_是 IteratorWrapper类型、参数 index_iter 是 `Iterator*`类型。

- 自答：对C++不熟悉，C++ explicit关键字 （用于构造函数），所以迭代器指针赋值，就等效于 传参并调用构造函数；

```c++
TwoLevelIterator::TwoLevelIterator(Iterator* index_iter,
                                   BlockFunction block_function, void* arg,
                                   const ReadOptions& options)
    : block_function_(block_function),
      arg_(arg),
      options_(options),
      index_iter_(index_iter),
      data_iter_(nullptr) {}
```



## "index" block

> 》：
>
>  从设计的角度来思考，对于xxx的设计，代码要如何写来实现它。这样，更容易在阅读复杂代码时，理解其编写的目的。

```
         ┌─────────────┐            
┌────────► data block 1│            
│        ┌─────────────┴───────────┐
│ ┌──────►       data block 2      │
│ │      ┌───────────────────┬─────┘
│ │ ┌────►   data block 3    │      
│ │ │    └───────────────────┘      
│ │ │                               
│ │ │    ┌─────────────┐            
│ │ │    │             │            
│ │ │    ├─────────────┤            
│ │ │    │             │            
│ │ │    └─────────────┘            
│ │ │                                                      
│ │ │                               
│ │ │      index block              
│ │ │    ┌───┬──────────┐           
└─┼─┼────┤key│v:hd range│           
  │ │    ├───┼──────────┤           
  └─┼────┤key│          │           
    │    ├───┼──────────┤           
    └────┤key│          │           
         └───┴──────────┘           
```



## 【✔】block.cc



### **Block**：块对象

- Ds：表示一个数据块（data block） 或者 索引块（index block）
- 构造器：
  - 将（块内容）的属性复制到这里。
  - 如果 @大小 小于 4字节：
    - @大小 设置为0，标记错误。
  - 否则：
    - 。。。

#### NewIterator：

- Ds：
- 如果 重启点个数为0：
  - 返回 EmptyIterator（）错误迭代器
- 否则：
  - 返回  数据块（data block） 的迭代器



#### 私有属性：

- data_：数据块（data block）的起始地址。
- size_：数据块（data block）的大小。 
- restart_offset_：（，**重启数组**？？？在数据中的偏移。）Offset in `data_` of restart array
  - 答：如下图所示

- owned_：标志位，表示是否为堆上分配，表示（`data_` 通过 new方法分配的）

NumRestarts：对块内容的最后4字节解码为U32（LSB编码），就是重启点的个数。

```
data block 图：
   注：entry结构图 【@see  block_build.cc】
     ┌────────────────┐
   ┌►│  entry 1       │
   │ ├────────────────┤
   │ │  entry 2       │
   │ ├────────────────┤
┌──┤►│  ......        │
│  │ ├────────────────┤
│  │ │  entry n       │
│  │ ├────────────────┤
│  └─┤restart point 1 │  4字节大小
│    ├────────────────┤
└────┤restart point 2 │
     ├────────────────┤
     │......          │
     ├────────────────┤
     │restart point   │  4字节大小，LSB编码
     │length          │
     └────────────────┘
```



### DecodeEntry：

- 如果长度小于3，则参数非法，返回null
- 由于是varint32编码，可以判断字节是否大于（2^7 = 128）而得出这个varint32是否只有1个字节。
- 如果 `(*shared | *non_shared | *value_length) < 128` ：
  - 快速解码，前3个字节正好是解码的内容。// Fast path: all three values are encoded in one byte each
  - p向前移动3个 

- 否则：
  - 解码1、2、3

- 如：剩余长度（limit - 当前p） 小于  非共享key  + value长度：
  - 非法参数，返回NULL

- 返回p

```
Ds： （，对数据块里的一个项进行解码。） Helper routine: decode the next block entry starting at "p",  storing the number of shared key bytes, non_shared key bytes,  and the length of the value in "*shared", "*non_shared", and  "*value_length", respectively.  Will not dereference past "limit".

Para；
  p：解码项的起始地址
  limit：解码项的终止地址
  shared：【out】storing the number of shared key bytes
  non_shared：【out】 storing the number of non_shared key bytes
  value_length：【out】storing the length of the value in "*shared", "*non_shared", and  "*value_length"
ret：
  1、 If any errors are detected, returns nullptr.  
  2、Otherwise, returns a  pointer to the key delta (just past the three decoded values).
```



### Iter：数据块迭代器

- 私有内部类

Valid：返回 当前项的偏移地址小于 重启数组的偏移地址

- true：有效
- false：无效

key：返回当前的key_

value：返回当前的value_

Next:：（call：解析一个项）

Prev：

- Scan backwards to a restart point before current_
  - while 当前重启点_ 的项下标 大于等于 当前
    - 如果 重启点下标_  == 0：
      - 设置为非法 // No more entries
      - 返回
    - 重启点下标_ --
- （func：将自身seek到前一个重启点）
- do：
  - 循环直到当前项的结束地址，到达源项的起始地址；// Loop until end of current entry hits the start of original entry
- while （func：解析下一个项地址 ） 且 解析后下一个项的偏移地址  小于 源地址
- END

Seek：

- // （，现用二分法在重启数组中搜索，找到最后的一个重启点key 小于 目标）Binary search in restart array to find the last restart point with a key < target
- 如果 当前有效：
  - //  If we're already scanning, use the current position as a starting point. This is beneficial if the key we're seeking to is ahead of the  current position.
  - 比较当前key
  - 小于0、大于0：。。。
  - 等于0：直接返回 // We're seeking to the key we're already at.
- while：
  - 二分法
  - 【。。。】
- //  We might be able to use our current position within the restart block.  This is true if we determined the key we desire is in the current block  and is after than the current key.
  - 是否可以跳过seek：
    - 不能，则seek到 left的重启点
- // （在当前所处的重启块，线性搜索第一个key >= 目标）Linear search (within restart block) for first key >= target
  - while True：
    - 如果 解析一个项 结束了：
      - 返回
    - 如果 当前key_  >= 目标：
      - 返回

```
Para：
  target： seek的目标
```

SeekToFirst：先seek到重启点0，再解析下一个key

SeekToLast：先seek到最后一个重启点，再while循环解析下一个key直到重启点数组的前面。

#### 【私有属性】

- comparator_：
- data_：underlying block contents
- restarts_：（，重启点数组在内容的起始偏移量）Offset of restart array (list of fixed32)
- num_restarts_：（，重启点个数）Number of uint32_t entries in restart array
- current_： `current_` is offset in `data_` of current entry.  >= `restarts_` if !Valid
- restart_index_： Index of restart block in which `current_` **falls**
- key_：
- value_：临时变量，记录当前**项**的value部分slice表示
- status_：

> Note： 重启点导致，当前的 当前的重启点 对应的 是 entry i，但是 当前entry 是 j；而两者之间key有重叠，需要二者组合一起才能得到完整的 entry

NextEntryOffset：返回 当前项的尾部地址（项地址_ + 项大小_ ） - 数据块起始地址_  //（，返回当前项的尾部地址，就等同于下一个项的起始地址的偏移） Return the offset in data_ just past the end of the current entry.

GetRestartPoint：返回给定重启点下标的解码内容，就是重启点项的偏移大小。

SeekToRestartPoint：

- 清理当前key
- 重启点下标_  = @index
- // ParseNextKey() starts at the end of value_, so set value_ accordingly
- `value_ = Slice(data_ + offset, 0);` // 把seek的项设置为（不存在的前一个项）value为项的起始地址而长度为0；这样解析下一个项的时候，就是从起始地址开始。

#### ParseNextKey：解析一个项

- 当前地址偏移_ = 获取下一个项的偏移地址
  - 检测其是否超过重启点限制
  - 将2个当前项字段设置为非法；// No more entries to return.  Mark as invalid.
  - 返回 false
- Decode next entry
- 如果解码的地址是null 或者 当前key_  的大小 小于 共享长度：
  - 报告数据破坏错误
  - 返回 false

- 否则：
  - 当前key_  重新调整大小 为 共享长度。
  - 前key_  追加 非共享的长度
  - 当前的value_ 使用剩余的大小构建slice
  - 如果 当前重启数据下标_ + 1 小于 总的个数 并且  下一个下标的得到的重启点（项的偏移大小） 小于  当前项的偏移大小_
    - 当前重启数据下标_ ++

  - 返回true




## 【✔】format.cc

【静态常量，kTableMagicNumber，`echo http://code.google.com/p/leveldb/ | sha1sum` ，取其值的前64位 = `0xdb4775248b80fb57ull` ， Footer魔数】

【静态常量，kBlockTrailerSize，1-byte type + 32-bit crc = 5， 块的尾部大小】

sstable文件的物理结构：为了提高**整体的读写效率**，一个sstable文件按照固定大小进行块划分。

```
block块： 固定大小 4kb
  data：实际数据
  type： compression type，压缩类型说明了Block中存储的数据是否进行了数据压缩，若是，采用了哪种算法进行压缩。leveldb中默认采用Snappy算法进行压缩。
  crc：循环冗余校验校验码，校验范围包括数据以及压缩类型。
                  1字节   4字节crc
┌────────────────┬────┬──────┐
│    data        │type│ crc  │
└────────────────┴────┴──────┘
┌────────────────┬────┬──────┐
│    data        │type│ crc  │
└────────────────┴────┴──────┘
┌────────────────┬────┬──────┐
│    data        │type│ crc  │
└────────────────┴────┴──────┘
```



### **BlockHandle**：块范围

- // （，是一个指针，指向一个文件的存放一个数据块或元数据块的所在范围）BlockHandle is a pointer to the extent of a file that stores a data block or a meta block.
- 【枚举常量，kMaxEncodedLength，10 + 10  = 20，（块范围的最大编码长度）Maximum encoding length of a BlockHandle】

getter与setter：offset、size属性的设置

EncodeTo：@dst以varint64的方式编码offset和size

DecodeFrom：将 @input的内容进行varint64解码到offset和size，返回状态码。

【私有属性】

- offset_： 块在文件中的偏移
- size_：块在文件中的大小



### **Footer** ：尾部

- // （，封装固定的信息，存放在每一个table文件的尾部）Footer encapsulates the fixed information stored at the tail end of every table file.
  - 关键点：由于footer大小是固定的，且在尾部。所以是加载table数据的开始步骤

- 【枚举常量，kEncodedLength，2 * （块范围的最大编码长度）+ （Footer魔数） = 48，Footer长度】
  - Encoded length of a Footer.  Note that the serialization of a Footer will always occupy exactly this many bytes.  It consists of two block handles and a magic number.

- 私有属性：
- metaindex_handle_： 元数据块范围
- index_handle_：数据块范围

getter与setter：index_handle、metaindex_handle属性的。

EncodeTo：Footer格式编码

```
Ds：
Para：
  dst：【out】将footer对象的内存信息，编码（如下格式）后，追加写到dst中。
编码图：
       ┌──►┌──────────────┐                    
       │   │  meta index  │ varint64, 变长       
       │   │   handle     │                    
       │   ├──────────────┤                    
40 byte│   │              │                    
    ───┤   │ index handle │ varint64, 变长       
       │   ├──────────────┤                    
       │   │              │                    
       │   │   padding    │                    
       │   │              │                    
       └──►├──────────────┤ magic 小端编码                
           │magic 32 Lower│ 4 byte  
           ├──────────────┤                    
           │magic 32 high │ 4 byte 
           └──────────────┘                    
```

DecodeFrom：

- todo



### **BlockContents**：块内容（ slice的封装）

- data：Actual contents of data
- cachable：True iff data can be cached
- heap_allocated：True iff caller should delete[] data.data()



### ReadBlock：读取物理块的内容。

- （，读取块内容 + （ 块尾部大小） ）Read the block contents as well as the type/crc footer.
- 分配读buffer
- 调用读文件的读接口
- 如果结果不OK：
  - 返回 结果
- 如果 slice结果块的大小 不是 期望大小
  - 返回 数据破坏（“truncated block read”）
- 如果读选项 中 校验为true：
  - （，CRC校验）Check the crc of the type and the block contents
  - 。。。
  - 如果 内容crc 不等于 计算crc：
    - 返回 数据破坏（"block checksum mismatch"）
- switch  块类型：
  - 无压缩：
    - 如果 读文件，slice的数据 不等于 buffer：（，假设slice的内容在文件打开时回一直存活，直接使用。）Use it directly under the assumption that it will be live while the file is open.
      - 释放 buffer
      - @result 的 data赋值为新的Slice对象。属性：非堆上分配，缓存设置为false（Do not **double-cache**）
    - 否则：
      - @result 的 data赋值为buffer的Slice对象。属性：堆上分配，需要缓存。
    - break；
  - Snappy压缩：【🤢：exception】
    - 对数据块计算解压后的长度
    - 分配解压后长度的buffer
    - 解压到buffer中
    - 设置到@result
  - Zstandard压缩：【🤢🤢🤢】
- 返回 OK

```
Ds：
Para：
  file：被读取的文件
  options：读可选项
  handle：块范围
    【in】：指定文件读取的范围。
  result：【out】读取的块范围结果内容
Ret：Status
  结果状态
```



## iterator_wrapper.h

### IteratorWrapper：迭代器包装

 A **internal wrapper class** with an interface similar to Iterator that  caches the valid() and key() results for an underlying iterator.  <u>This can help avoid virtual function calls and also gives better **cache locality**.</u>

【私有属性】

- iter_：迭代器
- valid_：记录当前是否有效
- key_：记录迭代器的当前key



## 【❌】iterator.cc



## 【❌】merger.cc



## table.cc【I】读取sstable文件

### **Table**： 

#### Open：

- read file footer
- decode footer
- Read the index block
- 如果上面2个都成功了：
  - （准备好服务请求）we're ready to serve requests.

```
Ds:  （， 尝试打开存放在给定大小的文件中的table， 并读取允许从table中取回数据的必要的元数据项；）Attempt to open the table that is stored in bytes [0..file_size) of "file", and read the metadata entries necessary to allow retrieving data from the table. 
Para:
  options：
  file: 打开的文件大小
     file must remain live while this Table is in use.
  file_size：
  table：【out】 new分配内存，table内存（构造器，new 分配关键Rep属性）
Ret:
 1、If successful, returns ok and sets "*table" to the newly opened table.  The client should delete "*table" when no longer needed. 
   1.1 Does not take ownership of "*source", but the client must ensure that "source" remains live for the duration of the returned table's lifetime.
 2、If there was an error while initializing the table, sets "*table" to nullptr and returns a non-ok status.  
```



---

#### 【私有属性】

- rep_：私有结构体

#### ReadMeta：

```
Ds： 依据解析好的尾部信息，从sstable的对象中读取解析 元数据区域信息。
Para：
  footer： sstable的尾部信息
```

#### InternalGet：

- Ds：Calls (*handle_result)(arg, ...) with the entry found after a call to Seek(key).  May not make such a call if filter policy says that key is not present.
- 创建 `rep_->index_block` 索引块的迭代器
- 索引块的迭代器 seek 给定的value，定位到 key 所属的data block
- 如果 迭代器 有效：
  - 【。。。】找到对应的布隆过滤器，对key进行查看是否存在：
    - 不存在，无需继续查找；// Not found

  - 否则，对data block构建 块阅读器，返回迭代器。
  - 迭代器 seek 对应的key
  - 如果有效；
    - 调用 @handle_result 保存 key ，value

- 返回 状态

```
Para：
  options：读选项
  k：key
  arg：传递给函数指针的参数
    【out】：
     例子：设置指定key的value； saver保存器
  handle_result： 函数指针
    例子：version_set.cc
      void SaveValue(void* arg, const Slice& ikey, const Slice& v) 
```





### 【🔥】**Rep**：sstable对象的内部结构形式

- options：
- status：
- file：sstable文件，读取其内容。
- cache_id：
- filter：
- filter_data：
- metaindex_handle：
- index_block：数据索引的块对象

```
        ┌──────────────────┬────┐         
        │   data Block 1   │    │         
        ├──────────────────┤    │         
┌──────►│    ......        │    │data     
│       ├──────────────────┤    ├─►       
│       │   data Block N   │────┘         
│       ├──────────────────┼────┐        
│ ┌────►│  Filter Block    │    │         
│ │     ├──────────────────┤    │   
│ └─────┤                  │    │manage      
│  ┌───►│ Meta Index Block │    │meta-data
│  │    ├──────────────────┤    ├──►      
└──┼────┤  Index Block     │    │         
   │ ┌─►│                  │    │         
   │ │  ├──────────────────┤    │         
   │ └──┤   Footer         │    │         
   └────┤                  │    │         
        └──────────────────┴────┘         
```

> 设计思路：
>
> 1、footer是固定大小的，且在文件的尾部；
>
> 2、数据块是变长大小的序列，每一个数据块都需要一个索引项记录其具体位置，每一个索引项都是固定大小。
>
> 3、索引块由固定大小的项，数量不固定，需要footer确定其范围。



## table_builder.cc【I】构造sstable文件

TableBuilder provides the interface used to build a Table (an immutable and sorted map from keys to values). 

Multiple threads can invoke const methods on a TableBuilder without external synchronization, but if any of the threads may call a non-const method, all threads accessing the same TableBuilder must use external synchronization.

### **TableBuilder**：构造sstable文件

- 构造器：（，创建一个表构建器，内容存放在@file可写文件中；）Create a builder that will store the contents of the table it is building in *file.  
  - 【limit】Does not close the file.  It is up to the caller to close the file after calling Finish().

#### Add：

- // （，添加key，value到这个被构建的表）Add key,value to the table being constructed.
- // REQUIRES: key is after any previously added key according to comparator.
- // REQUIRES: Finish(), Abandon() have not been called

```
Para：
  key：
  value
This：
  数据块：
  索引块字段：
  
```

#### Flush：

- DS： Advanced operation: flush any buffered key/value pairs to file.  Can be used to ensure that two adjacent entries never live in  the same data block.  Most clients should not need to use this method.  
- REQUIRES: Finish(), Abandon() have not been called

```

```



#### Finish：

- Ds：Finish building the table.  Stops using the file passed to the constructor after this function returns. 
- REQUIRES: Finish(), Abandon() have not been called
- Impl：写入元数据 + 尾部内容。
  - //Write filter block
  - //Write metaindex block
  - //Write index block
  - //Write footer



---

#### 【私有属性】

- rep_：构建sstable的内部表示



#### **Rep**：内部私有类

- options：
- index_block_options：
- file：可写文件
- offset：文件写入偏移
- status：
- data_block：数据块的构造器
- index_block：索引块的构造器
- last_key：位置上？ 最后一个添加的key
- num_entries：
- closed：标志位，文件是否已经关闭，不可写入内容。
- filter_block：过滤器块的构造器
- pending_index_entry：标志位，true表示数据库是空的；
  - //  （，我们直到看见下一个数据块的第一个key才会产生一个块的索引项；例子，一个块边界在 "the quick brown fox" 和  "the who"之间；我们可以使用 "the r" 作为索引块项的key，因为其 大于等于 第一个项的所有key，并且 小于随后项的所有key；）We do not emit the **index entry** for a block until we have seen the  first key for the next data block.  This allows us to use shorter  keys in the index block.  For example, consider a block boundary  between the keys "the quick brown fox" and "the who".  We can use  "the r" as the key for the index block entry since it is >= all  entries in the first block and < all entries in subsequent  blocks.
  - Invariant: r->pending_index_entry is true only if data_block is empty.

- pending_handle：（添加到index块的范围）Handle to add to index block
- compressed_output：



## block_build.cc

原理图：【@see block.cc】

(，生成块，其键是前缀压缩的）BlockBuilder generates blocks where keys are **prefix-compressed**：

- （，当我们存储一个键时，丢弃掉与之前字符串共享的前缀。这样有助于显著地减少需要的空间。此外，一旦到达每K个键，我们不使用前缀压缩并存储整个键。称之为一个“重启点”。块的尾部存储所有重启点的偏移量，并可以被用于在查找一个特定的键时做二分查找；。值是未压缩而紧随着键的后面；）When we store a key, we drop **the prefix shared with the previous string**.  This helps reduce the space requirement significantly. Furthermore, once every K keys, we do not apply the prefix compression and store the entire key.  We call this a "**restart point**".  The tail end of the block stores the offsets of all of the restart points, and can be used to do a binary search when looking for a particular key.  Values are stored as-is (without compression) immediately following the corresponding key.
- 注：previous string指的是当前entry的前一个entry；

An entry for a particular key-value pair has the form：

- shared_bytes == 0 for **restart points**.

```
   shared_bytes: varint32
   unshared_bytes: varint32
   value_length: varint32
   key_delta: char[unshared_bytes]
   value: char[value_length]
 
data block中entry的原理图：
  1、entry中的key如何获取？
    1.1 如果是重启点，则 key_delta 本身就是key
    1.2 如果不是，需要前缀组合，key = 重启点key[shared_bytes] + key_delta
┌────────────────┐                    
│   shared_bytes │  varint32          
├────────────────┤                    
│  unshared_bytes│  varint32          
├────────────────┤                    
│  value_length  │  varint32          
├────────────────┤                    
│                │                    
│   key_delta    │ len: unshared_bytes
│                │                    
│                │                    
├────────────────┤                    
│                │                    
│                │                    
│                │                    
│    value       │ len: value_length  
│                │                                  
│                │                    
└────────────────┘        

```

The trailer of the block has the form：

- `restarts[i]`：（，包含第i个重启点在块内部的偏移量。） contains the offset within the block of the ith restart point.

```
  restarts: uint32[num_restarts]
  num_restarts: uint32
```



**BlockBuilder**：

Add：

- 断言：1、未调用Finish方法 2、下标 小于等于 可选项->重启间隔 3、缓冲区为空 或 @key 大于 最后一个key
- 前缀计数 为 0
- 如果 下标 小于 （重启间隔）：
  - // （取二者之间最小值）See how much sharing to do with previous string
  - while 上一个key的字符 与 当前key的字符 相同
    - 前缀计数++
- 否则：到达重启点，重启压缩
  - （重启点向量容器）尾部新增 缓冲区大小（等于 重启点偏移）
  - （游标变量）置0
- // Add "`<shared><non_shared><value_size>`" to buffer_
  - varint32编码
- // Add string delta to buffer_ followed by value
  - 把差量的key，加入缓冲区，然后是value。
- // Update state
  - 更新（临时变量key）为插入的key
  - （游标变量）++

```
Para：
  key：
  value：
This：
   buffer_：【out】将编码后的项，追加到缓冲区。
   其他各种属性
```



---

私有属性：

- options_：
- buffer_：（目标缓冲区）Destination buffer
- restarts_：（重启点向量容器）Restart points
- counter_：（，游标变量，记录从重启点之后的项数量；）Number of entries emitted since restart
- finished_： 标志位，Has Finish() been called?
- last_key_：临时变量，记录最后一个插入的key。







## filter_block.cc

【静态常量，kFilterBaseLg，11，对每2KB的数据生成过滤器 2**11 = 2KB（过滤器对应数据大小的log 2值）】

【静态常量，kFilterBase， 2KB = 1 << 11 ， 过滤器数据大小】

 

### **FilterBlockBuilder**：过滤器块构造器

- // (过滤器块构造器用于构建一个特定的sstable的所有过滤器。其生产一个单个字符串而存储为表中的一个特殊的块；）A FilterBlockBuilder is used to construct all of the filters for a particular Table.  It generates a single string which is stored as a special block in the Table.
- 调用约束：要按照如下的顺序进行。
  - The sequence of calls to FilterBlockBuilder must match the regexp:  `(StartBlock AddKey*)* Finish`



```

fliter block的编码图：
 filter：参数 【私有属性】result_
 offset：参考 【私有属性】filter_offsets_
 例子： this::Finish
    [filter 0]
    [filter 1]
    [filter 2]
    ...
    [filter N-1]

    [offset of filter 0]                  : 4 bytes
    [offset of filter 1]                  : 4 bytes
    [offset of filter 2]                  : 4 bytes
    ...
    [offset of filter N-1]                : 4 bytes

    [offset of beginning of offset array] : 4 bytes
    lg(base)                              : 1 byte
```



#### AddKey：

- start_ 入栈 @key的大小
- keys_ 字符串追加 @key



#### Finish

```

```



#### 【私有属性】

- policy_：
  - 例子：布隆过滤器
- keys_：// Flattened key contents
  - string：`[key0][key1][key2]...`
- start_：下标索引向量数组  // Starting index in `keys_` of each key
  - `[len(key0)][len(key1)][len(key2)]...`， 数组的大小可以表示key的数量
- result_：过滤器的数据 。。。 // Filter data computed so far
- tmp_keys_： 临时参数
  - // policy_->CreateFilter() argument
- filter_offsets_：过滤器偏移向量数组
  - 目的：一个result_ 里面可以同时存放多个过滤器的创建，而每一个过滤器创建的起始地址就在这里记录。

#### GenerateFilter

- 获取key的数量
- 如果为0：
  - // Fast path if there are no keys for this filter
  - 。。。
  - 返回
- // Make list of keys from flattened key structure
  - 【？？？】
- // Generate filter for current set of keys and append to result_.
  - 过滤器偏移向量数组_ 入栈 过滤器的数据_ 的大小
  - （布隆过滤器：创建过滤器，为当前的key），结果放到 result_ 中
- 临时参数_  清理
- keys_ 清理
- start_ 清理



### **FilterBlockReader**：过滤器块阅读器

#### KeyMayMatch：

- 将 @地址偏移 转为 2K的 向下取整的下标
- 如果 下标 小于 （。 **偏移数组**的元素个数）
  - 。。。
- 返回true（错误的情况被当做有潜在匹配的可能；Errors are treated as potential matches）

```
Ds： 通过过滤器查找key是否存在
Para：
  block_offset：key所属块的地址偏移
  key：查找的key
Ret：
  true： 可能存在（false positive）
  false： 不存在
```

---

#### 【私有属性】

- policy_：过滤策略，类型
- data_：过滤的数据，起始地址，block的头部（Pointer to filter data (at block-start)）
- offset_：过滤数据，结束地址，block尾部
- num_：（，**偏移数组**的元素个数）Number of entries in **offset array**
- base_lg_：（过滤器对应数据大小的log 2值）



## two_level_iterator.cc



### NewTwoLevelIterator【I】：对外接口

- （，功能：通过迭代索引层，再迭代数据层，从而对整个sstable文件的所有key-value进行迭代； 二层迭代器包含一个索引迭代器其值指向一个块序列，而块本身是一个键-值对序列。）Return a new two level iterator.  A two-level iterator contains an index iterator whose values point to a sequence of blocks where each block is itself a sequence of key,value pairs.  The returned two-level iterator yields the concatenation of all key/value pairs in the sequence of blocks.  Takes ownership of "index_iter" and will delete it when no longer needed.
- Uses a supplied function to convert an index_iter value into an iterator over the contents of the corresponding block.
- Impl：包装函数，参数传递，构建 TwoLevelIterator 对象；
  - 返回  TwoLevelIterator 对象


```

Para：
  index_iter：
  block_function： 函数指针
    例子： Iterator* GetFileIterator(void* arg, const ReadOptions& options,  const Slice& file_value)
  arg： 函数指针的参数
    例子： TableCache*
  options： 读可选项
```

### **TwoLevelIterator**：内部实现



#### 【私有属性】

- block_function_：构造器传入
- arg_：构造器传入
- status_：
- index_iter_：
- data_iter_ ： ，可能是null
- data_block_handle_： If `data_iter_` is non-null, then "`data_block_handle_`" holds the "`index_value`" passed to `block_function_` to create the `data_iter_`.

#### InitDataBlock

```

```








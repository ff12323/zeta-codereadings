



```
extent：the area covered by something.（范围）
```



### **Block**：

- 构造器：
  - 将（块内容）的属性复制到这里。
  - 如果 @大小 小于 4字节：
    - @大小 设置为0，标记错误。

  - 否则：
    - 。。。


---

私有属性：

- data_：数据内容的起始地址。
- size_：数据内容的大小。 
- restart_offset_：（，**重启数组**？？？在数据中的偏移。）Offset in `data_` of restart array
- owned_：标志位，表示是否为堆上分配，就是表示（`Block owns data_[]`）

NumRestarts：对块内容的最后4字节解码为U32（LSB编码），就是重启点的个数。





### "index" block

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

### metaindex block

与上一个类似，但是key为metaindex块的名字。



## format.cc

【静态常量，kTableMagicNumber，`echo http://code.google.com/p/leveldb/ | sha1sum` ，取其值的前64位 = `0xdb4775248b80fb57ull` ， Footer魔数】

【静态常量，kBlockTrailerSize，1-byte type + 32-bit crc = 5， 块的尾部大小】

```
block块：
┌────────────────┬────┬──────┐
│    data        │type│ crc  │
└────────────────┴────┴──────┘
```



### **BlockHandle**：块范围

- // （，是一个指针，指向一个文件的存放一个数据块或元数据块的所在范围）BlockHandle is a pointer to the extent of a file that stores a data block or a meta block.
- 【枚举常量，kMaxEncodedLength，10 + 10  = 20，（块范围的最大编码长度）Maximum encoding length of a BlockHandle】

### **Footer** ：尾部

- // （，封装固定的信息，存放在每一个table文件的尾部）Footer encapsulates the fixed information stored at the tail end of every table file.
  - 关键点：由于footer大小是固定的，且在尾部。所以是加载table数据的开始步骤

- 【枚举常量，kEncodedLength，2 * （块范围的最大编码长度）+ （Footer魔数） = 48，Footer长度】
  - Encoded length of a Footer.  Note that the serialization of a Footer will always occupy exactly this many bytes.  It consists of two block handles and a magic number.

- 私有属性：
- metaindex_handle_： 元数据块范围
- index_handle_：数据块范围

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





### **BlockContents**：块内容（ slice的封装）





ReadBlock：

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







## table.cc【I】读取sstable文件

**Table类**： 

----

**public**

### Open：

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

**private**

- rep_：私有结构体



**Rep**：内部私有类

- options：
- status：
- file：sstable文件，读取其内容。
- cache_id：
- filter：
- filter_data：
- metaindex_handle：
- index_block：



ReadMeta：

```
Ds： 依据解析好的尾部信息，从sstable的对象中读取解析 元数据区域信息。
Para：
  footer： sstable的尾部信息
```





## table_builder.cc【I】构造sstable文件

TableBuilder provides the interface used to build a Table (an immutable and sorted map from keys to values). 

Multiple threads can invoke const methods on a TableBuilder without external synchronization, but if any of the threads may call a non-const method, all threads accessing the same TableBuilder must use external synchronization.

**TableBuilder**：构造sstable文件

- 构造器：（，创建一个表构建器，内容存放在@file可写文件中；）Create a builder that will store the contents of the table it is building in *file.  
  - 【limit】Does not close the file.  It is up to the caller to close the file after calling Finish().

Add：

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

Finish：

- Ds：Finish building the table.  Stops using the file passed to the constructor after this function returns. 
- REQUIRES: Finish(), Abandon() have not been called
- Impl：写入元数据 + 尾部内容。
  - //Write filter block
  - //Write metaindex block
  - //Write index block
  - //Write footer



---

私有内容：

**Rep**：内部私有类

- file：可写文件
- offset：文件写入偏移
- status：
- data_block：
- index_block：
- last_key：位置上？ 最后一个添加的key
- num_entries：
- closed：标志位
- filter_block：过滤器块的构造
- pending_index_entry：标志位
- pending_handle：（添加到index块的范围）Handle to add to index block
- compressed_output：



## block_build.cc

(，生成块，其键是前缀压缩的）BlockBuilder generates blocks where keys are **prefix-compressed**：

- （，当我们存储一个键时，丢弃掉与之前字符串共享的前缀。这样有助于显著地减少需要的空间。此外，一旦每K个键，我们不使用前缀压缩并存储整个键。称之为一个“重启点”。块的尾部存储所有重启点的偏移量，并可以被用于在查找一个特定的键时做二分查找；。值是未压缩而紧随着键的后面；）When we store a key, we drop the prefix shared with the previous string.  This helps reduce the space requirement significantly. Furthermore, once every K keys, we do not apply the prefix compression and store the entire key.  We call this a "**restart point**".  The tail end of the block stores the offsets of all of the restart points, and can be used to do a binary search when looking for a particular key.  Values are stored as-is (without compression) immediately following the corresponding key.

An entry for a particular key-value pair has the form：

- shared_bytes == 0 for **restart points**.

```
   shared_bytes: varint32
   unshared_bytes: varint32
   value_length: varint32
   key_delta: char[unshared_bytes]
   value: char[value_length]
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

 

**FilterBlockBuilder**：过滤器块构造器

- // (过滤器块构造器用于构建一个特定的sstable的所有过滤器。其生产一个单个字符串而存储为表中的一个特殊的块；）A FilterBlockBuilder is used to construct all of the filters for a particular Table.  It generates a single string which is stored as a special block in the Table.
- 调用约束：要按照如下的顺序进行。
  - The sequence of calls to FilterBlockBuilder must match the regexp:  `(StartBlock AddKey*)* Finish`



```

fliter block的编码图：
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



**FilterBlockReader**：过滤器块阅读器

KeyMayMatch：

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

私有属性

- policy_：过滤策略，类型
- data_：过滤的数据，起始地址，block的头部（Pointer to filter data (at block-start)）
- offset_：过滤数据，结束地址，block尾部
- num_：（，**偏移数组**的元素个数）Number of entries in **offset array**
- base_lg_：（过滤器对应数据大小的log 2值）



## ch01 sstable读

### 1.1、二层迭代器



NewTwoLevelIterator：

- （，。。。； 二层迭代器包含一个索引迭代器其值指向一个块序列，而块本身是一个键-值对序列。）Return a new two level iterator.  A two-level iterator contains an index iterator whose values point to a sequence of blocks where each block is itself a sequence of key,value pairs.  The returned two-level iterator yields the concatenation of all key/value pairs in the sequence of blocks.  Takes ownership of "index_iter" and will delete it when no longer needed.
- Uses a supplied function to convert an index_iter value into an iterator over the contents of the corresponding block.

```

Para：
  index_iter：
  block_function： 函数指针
  arg：
  options： 读可选项
```



**two_level_iterator**：



---

私有属性：

- index_iter_：
- data_iter_ ： ，可能是null
- data_block_handle_： If `data_iter_` is non-null, then "`data_block_handle_`" holds the "`index_value`" passed to `block_function_` to create the `data_iter_`.










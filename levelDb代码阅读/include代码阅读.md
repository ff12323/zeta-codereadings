

概念：

- internal data structures、index blocks
- latency/performance hiccups（性能间隔）
- operating system buffer cache

不懂||第一次的记录：

- user data is stored in a set of blocks, and a block is the unit of reading from disk
- 【？？？】the machine crashes, some recent writes may be lost.  Note that if it is just the process that crashes (i.e., the machine does not reboot), no writes will be lost even if sync==false.
  - ？： write与fsync系统调用与当前DB行为的联系。



英语生词：

```
ramifications： 
   a complex or unwelcome consequence of an action or event；(众多复杂而又难以预料的)结果，后果;
```



### **Slice**：外部存储数据封装

- // （，是一个简单的结构体包含一个指向外部存储的指针，和其大小；Slice的用户必须确保。。。不再使用。。。在外部存储被释放后；）Slice is a simple structure containing a pointer into some **external storage** and a size.  The user of a Slice must ensure that the slice is not used after the corresponding external storage has been deallocated. 
- // Multiple threads can invoke const methods on a Slice without external synchronization, <u>but if any of the threads may call a non-const method, all threads accessing the same Slice must use external synchronization.</u>





### **Comparator**：比较器，key排序

-  Ds：比较器 + 字符串顺序的工具。
-  //A Comparator object provides a total order across slices that are used as keys in an sstable or a database.  A Comparator implementation must be **thread-safe** since leveldb may invoke its methods concurrently from multiple threads.

Compare：

```
Para：
  a：slice a
  b：slice b
Ret：
  < 0 iff "a" < "b",
  == 0 iff "a" == "b",
  > 0 iff "a" > "b"
```

FindShortestSeparator

- Ds：在2个字符之间，返回它们之间的最小分隔字符串。
- // Advanced functions: these are used to reduce the space requirements for internal data structures like **index blocks**. 
  - If `*start < limit`, changes `*start` to a short string in `[start,limit)`.
  - Simple comparator implementations may return with *start unchanged, i.e., an implementation of this method that does nothing is correct.
- @start：
- @limit：

FindShortSuccessor

-  Changes *key to a short string >= *key.



### **FilterPolicy**：过滤器

- （，一个数据库可以被配置上一个自定义的**过滤器策略**对象。这个对象负责从一个键的集合而创建一个小的过滤器；这些过滤器存放在leveldb中，并且会被其自动地咨询而决定是否从磁盘中读取一些信息。在很多情况下，一个过滤器可以减少多个的磁盘seek而组成一些到单个的磁盘seek，对于每次的Get调用）A database can be configured with a custom FilterPolicy object. This object is responsible for creating a small filter from a set of keys.  These filters are stored in leveldb and are consulted automatically by leveldb to decide whether or not to read some information from disk. In many cases, a filter can cut down the number of disk seeks form a handful to a single disk seek per `DB::Get()` call.



### **Iterator**：迭代器

析构器：

- 如果头节点有被使用：说明链表有数据
  - 运行头结点 清理函数
  - 遍历链表的后面节点：
    - 运行节点的 清理函数
    - 获取下一个节点
    - delete 删除当前节点

Valid：

- Ds：（一个迭代器要么指向一个 键/值 对，或者无效）An iterator is either positioned at a key/value pair, or not valid.
- Ret：
  - 1、true iff the iterator is valid.
  - 2、false，非法。

SeekToFirst：

- Ds：（，指向源的第一个key；如果源不为空，则迭代器在这个调用后将是有效的；）Position at the first key in the source.  The iterator is Valid() after this call iff the source is not empty.

SeekToLast：

- Ds：Position at the last key in the source.  
  - The iterator is Valid() after this call iff the source is not empty.

Seek：

- Ds：（，指向源中第一个与目标key相同或之后的key；）指向Position at the first key in the source that is at or past target. 
  - 【？？？】为什么要包含后一个的
  - The iterator is Valid() after this call iff the source contains an entry that comes at or past target.
- Para：
  - target：目标key的slice。

Next：

- Ds：（移动到源中的下一项）Moves to the next entry in the source.  
  - （，如果迭代器不是在最后一项，则调用后是有效的）After this call, Valid() is true iff the iterator was not positioned at the last entry in the source.
- REQUIRES: Valid()

Prev：

- Ds：Moves to the previous entry in the source.  
  - （，如果迭代器不是在第一项，则调用后是有效的）After this call, Valid() is true iff the iterator was not positioned at the first entry in source.
- REQUIRES: Valid()

key：

- Ds：Return the key for the current entry. 
  - limit： The underlying storage for the returned slice is valid only until the next modification of the iterator.
- REQUIRES: Valid()

value：

- Ds：Return the value for the current entry.  
  - limit：The underlying storage for the returned slice is valid only until the next modification of the iterator.
- REQUIRES: Valid()

RegisterCleanup：

- Ds：注册销毁清理函数
- Para：
  - function：Clients are allowed to register function/arg1/arg2 triples that will be invoked when this iterator is destroyed.
    - Note： that unlike all of the preceding methods, this method is not abstract and therefore clients should not override it.
  - arg1、arg2：传给清理函数
- Impl：
  - 如果 清理链表头_ 是未使用的：
    - 更新节点  设置 为 清理链表头_
  - 否则：
    - new 分配新节点，并插入链表头部
  - 更新节点的属性（从参数传入）

---

私有属性：

- cleanup_head_：清理链表头。

**CleanupNode**：

- 【类型】私有结构体
- next：下一个节点
- function：清理函数
- arg1、arg2：

IsEmpty：

- Ret：清理函数_ 为空；True if the node is not used. Only head nodes might be unused.

Run：调用清理函数。

---





### **Options**：可选项

- // （，控制数据库的行为）Options to control the behavior of a database (passed to `DB::Open`)
- // （以下参数是影响行为的。）Parameters that affect behavior
- comparator：
  - （，比较器用于定义键在表中的顺序。默认：字母表字节序；）Comparator used to define the order of keys in the table. Default: a comparator that uses **lexicographic byte-wise ordering** 
  - REQUIRES: The client must ensure that the comparator supplied here has the same name and orders keys *exactly* the same as the comparator provided to previous open calls on the same DB.

- paranoid_checks：
  - If true, the implementation will do **aggressive checking** of the data it is processing and will stop early if it detects any errors.  This may have unforeseen ramifications: for example, <u>a corruption of one DB entry may cause a large number of entries to become unreadable or for the entire DB to become unopenable</u>.
- env：
  - Use the specified object to interact with the environment, e.g. to read/write files, schedule background work, etc. Default: `Env::Default()`

- info_log：
  - （，日志文件。。。）Any internal progress/error information generated by the db will be written to info_log if it is non-null, or to a file stored in the same directory as the DB contents if info_log is null.

- // （以下是影响性能的参数）Parameters that affect performance
- write_buffer_size：4MB = 4 * 1024 * 1024
  - （，在转为一个排序好的在磁盘上的文件前，要在内存上构建好的数据总量（由一个磁盘无序的日志文件备份；））Amount of data to build up in memory (**backed by** an unsorted log on disk) before converting to a sorted on-disk file. 
  - （，更大的值增加性能，尤其在大块加载时；）Larger values increase performance, especially during bulk loads. Up to two write buffers may be held in memory at the same time, so you may wish to adjust this parameter to control memory usage. <u>Also, a larger write buffer will result in a longer recovery time the next time the database is opened.</u>

- max_open_files ：
  - Number of open files that can be used by the DB.  You may need to increase this if your database has a large working set (budget one open file per 2MB of working set).

- block_cache：
  - Control over blocks (<u>user data is stored in a set of blocks, and a block is the unit of reading from disk</u>). 
    - If non-null, use the specified cache for blocks. 
    - If null, leveldb will automatically create and use an 8MB internal cache.

- block_size：
  - （，每个块里的用户数据近似值； 未压缩的大小；）Approximate size of user data packed per block.  Note that the block size specified here corresponds to uncompressed data.  The actual size of the unit read from disk may be smaller if compression is enabled.  <u>This parameter can be changed dynamically.</u>

- block_restart_interval：
  - （， 在2个重启点之间进行**差分编码**的键数量。）Number of keys between restart points for delta encoding of keys. This parameter can be changed dynamically.  Most clients should leave this parameter alone.

- max_file_size ：
  - Leveldb will write up to this amount of bytes to a file before switching to a new one. 
  - Most clients should leave this parameter alone.  However if your filesystem is more efficient with larger files, you could consider increasing the value.  <u>The downside will be longer compactions and hence longer latency/performance hiccups</u>. Another reason to increase this parameter might be when you are initially populating a large database.
    - 【？？？】

- compression：snappy算法
  - （，问：可以动态改变的具体原因？）Compress blocks using the specified compression algorithm.  This parameter can be changed dynamically. 
  - Default: kSnappyCompression, which gives lightweight but fast compression. 
  - Typical speeds of kSnappyCompression on an Intel(R) Core(TM)2 2.4GHz: 
    - ~200-500MB/s compression 
    - ~400-800MB/s decompression 

  - Note that these speeds are significantly faster than most persistent storage speeds, and therefore it is typically never worth switching to kNoCompression.  Even if the input data is incompressible, the kSnappyCompression implementation will efficiently detect that and will switch to uncompressed mode.

- filter_policy：
  - If non-null, use the specified filter policy to reduce disk reads. Many applications will benefit from passing the result of NewBloomFilterPolicy() here.


ReadOptions：

- 读可选项
- verify_checksums：If true, all data read from underlying storage will be verified against corresponding checksums.
- fill_cache ：true
  - （，是否缓存在内存中？调用者可能在大块扫描时设置为false；）Should the data read for this iteration be cached in memory? Callers may wish to set this field to false for bulk scans.
- snapshot：
  - If "snapshot" is non-null, read as of the supplied snapshot (which must belong to the DB that is being read and which must not have been released).  
  - （，空，使用开始读操作时所处状态的隐性快照；）If "snapshot" is null, use an implicit snapshot of the state at the beginning of this read operation.

WriteOptions：

- 写可选项
- sync：
  - （，真：在写操作被认为完成之前，这个写将会从操作系统缓冲区缓存中刷入（flush）；。。。写操作会变慢。）If true, the write will be flushed from the operating system buffer cache (by calling `WritableFile::Sync()`) before the write is considered complete.  If this flag is true, writes will be slower. 
  - If this flag is false, and <u>the machine crashes, some recent writes may be lost.  Note that if it is just the process that crashes (i.e., the machine does not reboot), no writes will be lost even if sync==false.</u> 
  - In other words, a DB write with sync==false has similar crash semantics as the "write()" system call.  A DB write with sync==true has similar crash semantics to a "write()" system call followed by "fsync()".




CompressionType ：枚举

- // （，DB的内容存放在块的集合中，每一个块都持有一个键值对的序列；每一个块在存储于一个文件前都有可能被压缩。。。。）DB contents are stored in a set of blocks, each of which holds a sequence of key,value pairs.  Each block may be compressed before being stored in a file.  The following enum describes which compression method (if any) is used to compress a block.
- NOTE:  do not change the values of existing entries, as these are part of the persistent format on disk.
- kNoCompression：无压缩
- kSnappyCompression ：Snappy压缩
- kZstdCompression ：Zstandard压缩

> wiki：
>
> 1、Snappy是Google基于LZ77的思路用C++语言编写的快速数据压缩与解压程序库，并在2011年开源。它的目标并非最大压缩率或与其他压缩程序库的兼容性，而是非常高的速度和合理的压缩率。使用一个运行在64位模式下的酷睿i7处理器的单个核心，压缩速度250 MB/s，解压速度500 MB/s。
>
> 2、Zstandard是由Facebook的Yann Collet开发的一个无损数据压缩算法。





### **Table**：sstable

- // （，表是要给从字符串到字符串的一个有序映射；表是不可变的和持久化的；。。。）A Table is a sorted map from strings to strings.  Tables are immutable and persistent.  A Table may be safely accessed from multiple threads without external synchronization.
- @see 《table xxxx.md》



### TableBuilder：sstable构建器



### **Env**：环境

> An Env is an interface used by the leveldb implementation to access operating system functionality like the filesystem etc.  Callers may wish to provide a custom Env object when opening a database to get fine gain control; e.g., to rate limit file system operations. 
>
> All Env implementations are safe for concurrent access from multiple threads without any external synchronization.

Default：

- （，返回一个适合当前操作系统的默认环境；。。。）Return a default environment suitable for the current operating system.  **Sophisticated users** may wish to provide their own Env implementation instead of relying on this default environment.



。。。

---

RandomAccessFile：

- // （，对于随机读取文件内容的抽象表示；）A file abstraction for randomly reading the contents of a file.

Read：

```
Ds： Read up to "n" bytes from the file starting at "offset". "scratch[0..n-1]" may be written by this routine
Para：
  offset：读取文件的起始偏移
  n：期望的读取长度
  result：【out】Slice的通用表示，将 @scratch的buffer设置到其中。
    Request：May set "*result" to point at data in "scratch[0..n-1]", so "scratch[0..n-1]" must be live when "*result" is used.
    Implementation： （result的结果指针可能是指向其它数据，scratch从其中复制而来。）File implementation gave us pointer to some other data. 
  scratch：【out】读取的结果存放buffer
    caller：malloc before call
Thread： Safe for concurrent use by multiple threads.
Ret：
  OK
  non-OK： has error
```





### Cache： LRU缓存接口



### **DB**： 数据库接口，用户直接使用



### DumpFile：接口函数





### export：定义接口导出宏



### Status：状态码封装对象





### WriteBatch：批量操作原子更新








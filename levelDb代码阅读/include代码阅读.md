

概念：

- internal data structures、index blocks





**Slice**：

- // （，是一个简单的结构体包含一个指向外部存储的指针，和其大小；Slice的用户必须确保。。。不再使用。。。在外部存储被释放后；）Slice is a simple structure containing a pointer into some **external storage** and a size.  The user of a Slice must ensure that the slice is not used after the corresponding external storage has been deallocated. 
- // Multiple threads can invoke const methods on a Slice without external synchronization, <u>but if any of the threads may call a non-const method, all threads accessing the same Slice must use external synchronization.</u>





**Comparator**：

-  Ds：比较器 + 字符串顺序的工具。
-  //A Comparator object provides a total order across slices that are used as keys in an sstable or a database.  A Comparator implementation must be **thread-safe** since leveldb may invoke its methods concurrently from multiple threads.

FindShortestSeparator

- Ds：在2个字符之间，返回它们之间的最小分隔字符串。
- // Advanced functions: these are used to reduce the space requirements for internal data structures like **index blocks**. 
  - If `*start < limit`, changes `*start` to a short string in `[start,limit)`.
  - Simple comparator implementations may return with *start unchanged, i.e., an implementation of this method that does nothing is correct.
- @start：
- @limit：

FindShortSuccessor

-  Changes *key to a short string >= *key.



**FilterPolicy**：

- （，一个数据库可以被配置上一个自定义的**过滤器策略**对象。这个对象负责从一个键的集合而创建一个小的过滤器；这些过滤器存放在leveldb中，并且会被其自动地咨询而决定是否从磁盘中读取一些信息。在很多情况下，一个过滤器可以减少多个的磁盘seek而组成一些到单个的磁盘seek，对于每次的Get调用）A database can be configured with a custom FilterPolicy object. This object is responsible for creating a small filter from a set of keys.  These filters are stored in leveldb and are consulted automatically by leveldb to decide whether or not to read some information from disk. In many cases, a filter can cut down the number of disk seeks form a handful to a single disk seek per `DB::Get()` call.



**Iterator**：

- 



**Options**：

- // （，控制数据库的行为）Options to control the behavior of a database (passed to `DB::Open`)






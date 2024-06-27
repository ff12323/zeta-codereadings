

英语生词：

```
resurrect： restore (a dead person) to life.
```





Snapshot ：

- // Abstract handle to particular state of a DB. A Snapshot is an **immutable object** and can therefore be safely accessed from multiple threads without any external synchronization.

Range：

- （一个key的范围）A range of keys
- 属性：
  - start：开始的key，Included in the range
  - limit：结束的key， Not included in the range
    - ?：null是否表示包含最后一个key



### DB：

- //（，DB是一个持久化的有序的键到值之间的映射；DB在多线程无外部同步的情况下，进行并发访问是安全的；）A DB is a persistent ordered map from keys to values； A DB is safe for concurrent access from multiple threads without any **external synchronization**.



#### Open

```
Open:
  Ds： Open the database with the specified "name".
  Caller: should delete *dbptr when it is no longer needed.
  Ret：
  	 1、ores a pointer to a heap-allocated database in *dbptr and returns OK on success. 
  	 2、ores nullptr in *dbptr and returns a non-OK status on error. 
```



#### Put

```
Ds: Set the database entry for "key" to "value".
Para：
  options：写可选项
    Note: consider setting options.sync = true.
  key：
  value：
Ret:
  1、OK on success
     It is not an error if "key" did not exist in the database.
  2、a non-OK status on error.

```



#### Delete

```
Ds: Remove the database entry (if any) for "key". 
Para:
  options: 写可选项
      Note: consider setting options.sync = true.
  key:
Ret:
  1、OK on success
  2、a non-OK status on error.
```

#### Get

```
Ds: 
Para:
  options： 读可选项
  key：
  value：【out】
Ret：
  2、If the database contains an entry for "key" store the corresponding value in *value and return OK. 
  1、If there is no entry for "key" leave *value unchanged and return a status for which Status::IsNotFound() returns true. 
  2、May return some other Status on an error.
```

#### NewIterator：

```
Ds： 创建一个新的迭代器
Para：
  options：读可选项
Caller：
   1、Caller should delete the iterator when it is no longer needed.
   2、（返回的迭代器应该在db删除前完成删除）The returned iterator should be deleted before this db is deleted.
Ret：
	Return a heap-allocated iterator over the contents of the database. 
	  Note： The result of NewIterator() is initially invalid (caller must call one of the Seek methods on the iterator before using it).
	  
```

#### GetSnapshot

```
Ds：（，对于这个快照而创建的迭代器，将都能观察到对当前的DB状态的一个稳定快照）Iterators created with this handle will all observe a stable snapshot of the current DB state. 
Caller：
    The caller must call ReleaseSnapshot(result) when the snapshot is no longer needed.
Ret：快照
  a handle to the current DB state.
```

#### ReleaseSnapshot：

- Ds：Release a previously acquired snapshot.
- Caller：The caller must not use "snapshot" after this call.





。。。

。。。





#### DestroyDB

```
Ds： Destroy the contents of the specified database. Be very careful using this method.
Para：
  name：
  options：
Note: （，为了向后兼容性。。。）For backwards compatibility, if DestroyDB is unable to list the database files, Status::OK() will still be returned masking this failure.
```



#### RepairDB

- （，如果一个DB无法打开，你可以尝试调用这个方法恢复尽可能多的DB内容；。。。）If a DB cannot be opened, you may attempt to call this method to resurrect as much of the contents of the database as possible. Some data may be lost, so be careful when calling this function on a database that contains important information.







### DBImpl：

- father：DB



#### Get：

- @see：`DB::Get`
- 互斥量_ **加锁**
- 如果可选项带了快照：
  - 【。。。】

- 否则： 读取 版本链_ 的最近的一个顺序号。
- 获取并记录当前的 内部表_， `不可变表_` 的指针。
- 获取 版本链_ 的当前版本。
- 内存表，不可变表，当前版本 的 引用计数+1
- // （当读文件和内存表时解除互斥锁）Unlock while reading from files and memtables
  - **解锁**
  - （首先在内存表查找：First look in the memtable），如果存在，结束查找。
  - 否则如果：存在不可变表，在其中查找，如果存在，结束查找。
  - 否则：
    - 从 当前版本 中查找，并获取 状态。
    - 更新状态标志位 设置为 真；

  - **加锁**

- 如果是 从 当前版本中查找 且 。。。：
  - （func：可能需要调度进行整合）

- 内存表，不可变表，当前版本 的 引用计数-1
- 返回 查找的结果状态；
- END（局部变量销毁，**解锁**）

---

**私有属性**：

- // 构建后是常量的属性（Constant after construction）
  - env_：
  - internal_comparator_：内部比较器
  - internal_filter_policy_：
  - options_： 可选项。（可选项的comparator == &内部比较器）
  - owns_info_log_： 【？】
  - owns_cache_：【？】
  - dbname_： db的名字
- table_cache_：
- db_lock_：（文件锁，对持久化的DB状态加锁；）Lock over the persistent DB state.  Non-null iff successfully acquired.
- `mutex_`：互斥量_，包含自身状态属性。
- shutting_down_：
- `background_work_finished_signal_`：后台任务结束信号量；被（互斥量_）保护。
- mem_： 可写的内存数据库。
- `imm_`：冻结的，不可写的内存数据库。（（，或者说是正在被整合的）Memtable being compacted）；被（互斥量_）保护。
- has_imm_：原子标志位，是否存在。（So bg thread can detect non-null `imm_`）
- logfile_：可写文件，日志记录内存数据库。
- `logfile_number_`：日志文件数量；被（互斥量_）保护。
- log_： 日志写入器；
- `seed_`：用于采样（For sampling.）；被（互斥量_）保护。
- // （写入器的队列）Queue of writers.；被（互斥量_）保护。
  - writers_：双端队列
  - tmp_batch_：批量写对象

- `snapshots_`：快照链表；被（互斥量_）保护。
- pending_outputs_：（，sstable文件的集合，用于保护其被删除；因为他们正在整合的一部分）Set of table files to protect from deletion because they are part of ongoing compactions.
  - 被（互斥量_）保护。
- background_compaction_scheduled_：标志位，Has a background compaction been scheduled or is running?
  - 被（互斥量_）保护。

- `manual_compaction_`：手动整合的信息；被（互斥量_）保护。
- `versions_`： 版本集合；被（互斥量_）保护。
- `bg_error_`：【？？？】 Have we encountered a background error in **paranoid mode**?；被（互斥量_）保护。
- stats_[7（最大整合层级）] ：`stats_[level]` stores the stats for compactions that produced data for the specified "level".
  - 被（互斥量_）保护。



**CompactionState**：（每一层整合的状态；）Per level compaction stats.  

- 【class】：私有内部类

**Writer**：

- 【class】：私有内部类

**ManualCompaction**：（手动整合的信息）Information for a manual compaction

- 【class】：私有内部类



## dbformat.cc



【静态常量，kMaxSequenceNumber，((0x1ull << 56) - 1) = `0x00ffffffffffffff` ，最大序列号】

- // （，设计：最高位的8个字节空出来，这样就可以和类型字段压缩成1个64字节；为了节省空间；）We leave eight bits empty at the bottom so a type and sequence# can be packed together into 64-bits.

config：

- 【类型】：名空间
- 【目的】：（，常量组合。）Grouping of constants.  We may want to make some of these parameters set via options.
- 【静态常量，kNumLevels，7，最大的整合层级】



AppendInternalKey：【func】

```
Ds：将内部key按照如图的方式，输出。
Para：
  result：【out】存放结果字符串
  key：内部key对象
图： 内部key编码。
             LSB encoding
┌───────────┬───┬─┐           
│   key     │seq│t│           
└───────────┴───┴─┘           
   key_len    64bit           
```

ExtractUserKey：【func】

```
Ds：提取内部key的用户key
Para：
  internal_key： 包装内部key的slice
Ret：
  （内部key 减去 尾部8字节后，封装为Slice。）Returns the user key portion of an internal key.
```

ParseInternalKey：【func】

- 检验长度是否合法
- LSB 64位解码
- 获取属性赋值。
- 返回  类型是否合法。

```
Ds: Attempt to parse an internal key from "internal_key".
Para:
  internal_key:
  result：【out】解析的key
Ret:
  On success, stores the parsed data in "*result", and returns true.
  On error, returns false, leaves "*result" in an undefined state.
```





ValueType：

- 枚举类型，表示编码后的key是**删除键**/**正常键** 。是嵌入到下盘的结构体中。

### 【OK】**ParsedInternalKey** 

- （属性类对象：只提供属性）对于 内部key 进行解析之后，转为对象，包含关键的属性。
- user_key：原用户的key
- sequence：顺序号
- type：key类型（写入/删除）



### **InternalKeyComparator** ：

- // （，对于内部key的比较器而  ？？？）A comparator for internal keys that uses <u>a specified comparator for the user key portion and breaks ties by decreasing sequence number.</u>
- 父类：比较器

Compare：

- override1：
- 顺序：
  - increasing user key (according to user-supplied comparator)
  - decreasing sequence number
  - decreasing type (though sequence# should be enough to disambiguate)
- 提取2个内部key的用户key
- 使用 用户比较器_ 进行比较。
- 如果 2者 相等：
  - 比较2者解码后的 顺序号 + 类型
  - 顺序号 小的，返回 +1 ，表示更大；
  - 大的，返回-1；
- 返回 比较值。

```
override1：
  Para： slice类型
    a：
    b:

overload：
  Impl：
     将内部key转为slice类型，调用 override1 方法
  Para： 内部key类型
    a：
    b：
```



---

私有属性：

- user_comparator_：比较器

> break ties： the relationship between two people has come to an end

​	



### **InternalFilterPolicy** ：

- // Filter policy wrapper that converts from internal keys to user keys



### 【OK】**InternalKey**：内部key

- // Modules in this directory <u>should keep internal keys wrapped inside the following class</u> instead of plain strings so that we do not incorrectly use string comparisons instead of an InternalKeyComparator.
- 构造器：
  - 1、空构造 ，表示非法。（Leave `rep_` as empty to indicate it is invalid）
  - 2、


DecodeFrom：

- Ds：新的slice，替换内部key的rep_
- ret：false，替换的字符串为空；true，替换的不为空。

---

private：

- rep_：按照（内部key编码）的方式，以字符串存储key。



### 【OK】**LookupKey** :

- //  A helper class useful for `DBImpl::Get()`

构造器：将给定的key编码到内存空间，同时记录不同部分的起始地址到this属性中。

- 估计实际大小 为  key大小 + 13 （使用保守估计法：A conservative estimate）
- 如果 估计大小 小于 空间_  大小
  - 起始地址_  等于 空间_  起始地址
- 否则： 为new分配的起始地址
- 使用 varint32 对  总大小（key大小 + 尾部编码），设置到空间。设置 kstart_
- memcpy key到后面
- LSB 编码  顺序号 + 类型。
- 最后设置 end_

```
Para：
  user_key：
  s： key的顺序号
【out】
   1、this：
     space_
     标记位
   2、或者 malloc分配。
```

读取key的方法：

- 1、memtable_key：
- 2、internal_key
- 3、user_key

私有属性：

- `end_ - start_` ： 字符数组，表示一个memtable的key
- `end_ - kstart_`： 表示一个内部key
- `end_ - kstart_ - 8`： 表示一个userkey

```
注释：
  // We construct a char array of the form:
  //    klength  varint32               <-- start_
  //    userkey  char[klength]          <-- kstart_
  //    tag      uint64
  //                                    <-- end_
  // The array is a suitable MemTable key.
  // The suffix starting with "userkey" can be used as an InternalKey.
  
图： 
                     varint64 encoding
   ┌────┬───────────┬───┬─┐           
   │ len│   key     │seq│t│           
   └────┴───────────┴───┴─┘           
varint32   key_len    64bit           
```





## skiplist.h

### **SkipList**：

- 【模板类】：（key类型，比较器自定义） `<typename Key, class Comparator>` 



Insert：

```
Ds：插入@元素到跳表
Para：
  key：
REQUIRES: （跳表中不能存在与这个key相等的key）nothing that compares equal to key is currently in the list.
```

Contains：

```
Ds：查找@元素是否在跳表中存在
ret：
  Returns true iff an entry that compares equal to key is in the list.
```



---

【跳表】私有属性：

- 【枚举常量，kMaxHeight，12， 跳表最大高度】
- // Immutable after construction
  - compare_：比较器
  - arena_： （用于分配节点的内存范围；）Arena used for allocations of nodes
- head_： 跳表头部
- max_height_： （，跳表链的最大高度）Height of the entire list
  - limit：（， 。。。 阅读器存在竞争地读，但是过期的值是可以的；）Modified only by Insert().  Read racily by readers, but stale values are ok.
- rnd_：（，随机数生成器）
  - limit：Read/written only by Insert().



---

#### 1、**Iterator**：

- 【类型】公共内部类
- //（迭代跳表的内容）Iteration over the contents of a skip list

构造器：

- Ds：Initialize an iterator over the specified list.
- 返回值：（无效迭代器）The returned iterator is not valid.
- Impl：
  - 跳表_ 设置为 参数， 当前节点_ 设置为null

Seek：@see 接口

- 调用（跳表：查找大于或等于的节点）

SeekToLast：@see 接口

【迭代器】私有属性：

- list_： 迭代的跳表
- node_：（当前的？）跳表节点。



### **Node**：跳表节点

- 【类型】私有内部类
- 私有属性：
  - `next_[1]`： （，C++的柔性数组：数组的长度等同于节点的高度；）Array of length equal to the node height.  `next_[0]` is lowest level link.









## memtable.cc





**MemTable**：

- 构造器：（，内存表会被引用计数，初始化的引用计数为0，调用者至少要引用一次；）MemTables are reference counted.  The initial reference count is zero and the caller must call Ref() at least once.

Get：

- 获取查找key里的内存表key
- 。。。
- 迭代器定位到内存表key
- 如果 有效：
  - // Check that it belongs to same user key.  We do not check the sequence number since the Seek() call above should have skipped all entries with overly large sequence numbers.
  - 获取迭代器 当前的key
  - 获取key的起始地址
  - 比较迭代器的key 与 参数key 是否为同一个，是：
    - 解码尾部8字节
    - 如果key类型是 插入类型：
      - 设置后面的@value，返回 true
    - 如果是 删除类型：
      - @s设置为 not found，返回 true
- 返回 false

```
Ds：
Para：
  key：查找key对象
  value：【out】返回值
  s： 【out】返回值
Ret：
  1、@key有对应的value，设置到@value，返回true
  2、@key有删除类型的，设置@s为NotFound() error， 返回true
  3、二者都不存在，返回false。
  
图：
                  varint64               
                  encoding               
┌────┬───────────┬───┬─┬────┬───────────┐
│klen│   key     │seq│t│vlen│   value   │
└─▲──┴───────────┴───┴─┴─▲──┴───────────┘
  │     key_len    64bit │    value_len  
  │                      │               
  │                      │               
  │                      │               
  └─────varint32─────────┘               
```



---

私有属性：

- comparator_：比较器。
- refs_：this的引用计数
- arena_：
- table_：跳表；







## version_set.cc





Version：版本

- 私有属性：
  - vset_：（，指针：指向这个版本指向的版本集合）VersionSet to which this Version belongs
  - `next_`、`prev_`：（，双向循环链表：。。。）Next/Previous version in linked list
  - refs_：Number of **live refs** to this version






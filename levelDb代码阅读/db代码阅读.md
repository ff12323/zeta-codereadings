

英语生词：

```
resurrect： restore (a dead person) to life.
```

英语句子：

- 【？？？，翻译困难，感觉十分抽象】Return the earliest node that comes at or after key. 



## ch0 记录

索引：

- memtable.cc：
  - **MemTable**：内存中（跳表）中保存的key-value，编码格式图
- dbformat.cc：
  - AppendInternalKey： internal key（内部key），编码格式图
  - LookupKey : user key（用户key）；
- write_batch：
  - WriteBatch：批量写入的key-value（插入、删除），编码格式图
- log_writer：
  - EmitPhysicalRecord： 写入日志中的key-value，编码格式图







## DB

Snapshot ：

- // Abstract handle to particular state of a DB. A Snapshot is an **immutable object** and can therefore be safely accessed from multiple threads without any external synchronization.

Range：

- （一个key的范围）A range of keys
- 属性：
  - start：开始的key，Included in the range
  - limit：结束的key， Not included in the range
    - ?：null是否表示包含最后一个key

### 【⭐】DB：

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

#### Write：

```
Ds： （将特定的更新应用到数据库中）Apply the specified updates to the database.
Note: consider setting options.sync = true.
Ret：
  Returns OK on success, non-OK on failure.
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





#### GetProperty：

#### GetApproximateSizes：

#### CompactRange：



#### 【func】DestroyDB

```
Ds： Destroy the contents of the specified database. Be very careful using this method.
Para：
  name：
  options：
Note: （，为了向后兼容性。。。）For backwards compatibility, if DestroyDB is unable to list the database files, Status::OK() will still be returned masking this failure.
```



#### 【func】RepairDB

- （，如果一个DB无法打开，你可以尝试调用这个方法恢复尽可能多的DB内容；。。。）If a DB cannot be opened, you may attempt to call this method to resurrect as much of the contents of the database as possible. Some data may be lost, so be careful when calling this function on a database that contains important information.







### DBImpl：

- father：DB



#### Put：

- 1、使用DB父类的默认实现方法；（Default implementations of convenience methods that subclasses of DB can call if they wish）
- 2、将传入的一个key-value对，放入到 批量写 ；
- 3、调用 @this#Write ，将 批量写 应用到数据库中
- END

#### Delete：

- Ds：与Put流程相同，但是batch内容有差别，类型是删除。



#### Write：

- 【🤢】构建写入信息，加入  双端队列_  尾部， 使用条件变量变量等待，到达队头则继续。
- 多线程标志位检查：如果写入标志位完成，则返回 写入状态
- // May temporarily unlock and wait.
- （this：MakeRoomForWrite，清理内存空间&&可能触发后台任务调度sstable文件整合）



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
    - 【@see memtable.cc#Get】
  - 否则如果：存在不可变表，在其中查找，如果存在，结束查找。
  - 否则：
    - 【🤢🤢🤢】从 当前版本 中查找，并获取 状态。
    - 更新状态标志位 设置为 真；
  - **加锁**

- 如果是 从 当前版本中查找 且 。。。：
  - （func：可能需要调度进行整合）

- 内存表，不可变表，当前版本 的 引用计数-1
- 返回 查找的结果状态；
- END（局部变量销毁，**解锁**）

---

#### **私有属性**：

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
- shutting_down_：原子标准位之DB在被删除
- `background_work_finished_signal_`：后台任务结束信号量；被（互斥量_）保护。
- mem_： 可写的内存数据库。
- `imm_`：冻结的，不可写的内存数据库。（（，或者说是正在被整合的）Memtable being compacted）；被（互斥量_）保护。
- has_imm_：原子标志位，是否存在。（So bg thread can detect non-null `imm_`）
- logfile_：可写文件，日志记录内存数据库。
- `logfile_number_`：日志文件数量；被（互斥量_）保护。
- log_： 日志写入器；
- `seed_`：用于采样（For sampling.）；被（互斥量_）保护。
- // （写入器的队列）Queue of writers.；被（互斥量_）保护。
  - writers_：双端队列（保持每个等待写入的信息）
  - tmp_batch_：临时写入信息

- `snapshots_`：快照链表；被（互斥量_）保护。
- pending_outputs_：（，sstable文件的集合，用于保护其被删除；因为他们正在整合的一部分）Set of table files to protect from deletion because they are part of ongoing compactions.
  - 被（互斥量_）保护。
- background_compaction_scheduled_：标志位，后台整合在调度运行，Has a background compaction been scheduled or is running?
  - 被（互斥量_）保护。

- `manual_compaction_`：手动整合的信息；被（互斥量_）保护。
- `versions_`： 版本集合；被（互斥量_）保护。
- `bg_error_`：【？？？，状态码】 Have we encountered a background error in **paranoid mode**?；被（互斥量_）保护。
- stats_[7（最大整合层级）] ：`stats_[level]` stores the stats for compactions that produced data for the specified "level".
  - 被（互斥量_）保护。

#### MakeRoomForWrite：

- 允许延迟 =  @强制写入 取反
- while TRUE：
  - 如果 后台错误状态码_ 不为 OK：
    - break，返 状态码
  - 如果 允许延迟  且  当前版本L0数量_ 大于等于 【减缓写入数量】：
    - //（，场景：快要接近L0数量的硬限制；目的：1、为了不再硬限制时延迟单个写几秒钟，而上主动延迟1ms而减少延时方差；2、让核给整合线程，其与写日志的共享一个核。） We are getting close to hitting a hard limit on the number of L0 files.  Rather than delaying a single write by several seconds when we hit the hard limit, start delaying each individual write by 1ms to reduce **latency variance**.  Also, <u>this delay hands over some CPU to the compaction thread in case it is sharing the same core as the writer.</u>
    - 解锁_
    - 睡眠 1000us
    - 允许延迟  设置为 false  // Do not delay a single write more than once
    - 加锁_
  - 否则如果   不是@强制写入  且 当前内存表_ 大概内存使用量  小于等于 【单个日志文件大小】
    - 当前内存表有空间写日志（There is room in current memtable）
      - 【？？？为啥】
    - break，退出返回。
  - 如果 冻结内存表_  不为 NULL：
    - // （当前内存表已经被填满，但是前一个还在被整合中，所以要等待）We have filled up the current memtable, but the previous one is still being compacted, so we wait.
    - 日志："Current memtable full; waiting...\n"
    - 后台任务结束信号量_ 等待
  - 当前版本L0数量_  大于等于 【最大L0数量】
    - 日志："Too many L0 files; waiting...\n"
    - 后台任务结束信号量_ 等待
  - 否则：
    - （尝试切换到一个新的内存表，并触发对旧的进行整合）Attempt to switch to a new memtable and trigger compaction of old
    - 【🤢】分配新的文件号，新的可写文件。
    - 删除 日志写入器_ （旧的）
    - 关闭 可写日志文件_ 
      - 如果失败：
        - // We may have lost some data written to the previous log file. Switch to the new log file anyway, but record as a background error so we do not attempt any more writes. We could perhaps attempt to save the memtable corresponding to log file and suppress the error if that works, but that would add more complexity in a critical code path.
        - 后台错误状态码_  设置为 错误码
        - 后台任务结束信号量_ 通知所有人
    - 删除 可写日志文件_
    - 可写日志文件_  设置为新创建的
    - 可写日志文件号_  设置为 新的文件号
    - 日志写入其_  设置为 新的
    - 冻结内存表_  设置为 当前内存表_
    - 当前内存表_ 设置为 新分配的内存表，加 引用
    - @force 设置为 false // Do not force another compaction if have room
    - （func：可能调度进行整合）【🤢🤢🤢】
- // while
- 返回 状态码

```
Ds： 清理内存空间&&可能触发后台任务调度sstable文件整合
Para：
  force： 是否强制写入（允许延迟）
REQUIRES: 
  1、（持有到互斥量_ ）mutex_ is held
  2、（当前线程正在写入器队列头部）this thread is currently at the front of the writer queue
```

#### 【✔】MaybeScheduleCompaction：可能调度整合任务

- REQUIRES： `EXCLUSIVE_LOCKS_REQUIRED(mutex_)`
- 如果 后台整合在调度运行_ 已经进行：
  - pass // Already scheduled
- 否则如果：原子标准位之DB在被删除_ 
  - pass // DB is being deleted; no more background compactions
- 否则如果： bg_error_ 后台出错
  - pass // Already got an error; no more changes
- 否则如果：可变跳表_ 为空 且  手动整合_  为空  且 当前版本不需要整合
  - pass // No work to be done
- 否则：
  - 后台整合在调度运行_   设置为 true
  - 当前环境_  调度（this：BGWork，DB后台任务）

BGWork：call BackgroundCall

BackgroundCall：

- mutex_，函数范围加锁
- 如果：原子标准位之DB在被删除_ 
  - pass // No more background work when shutting down.
- 否则如果： bg_error_ 后台出错
  - pass // No more background work after a background error.
- 否则：
  - （this： BackgroundCompaction）
- 后台整合在调度运行_ 设置为 false（整合任务运行结束）
- // （，前面的整合可能在一个层级产生太多文件，所以如有需要则调度另一个整合任务）Previous compaction may have produced too many files in a level,  so reschedule another compaction if needed.
  - （this：可能调度整合任务）
- 后台任务结束信号量_  通知所有人





#### 【❌】BuildBatchGroup

```
Ds： 将 写入器中的队列 进行一次聚合成批量写，再写入。
Para：
  last_writer：【out】二维指针，设置最后一个写入器
Ret：
  批量写（多个写聚合在一起）
REQUIRES: 
  1、Writer list must be non-empty
  2、First writer must have a non-null batch
```





**CompactionState**：（每一层整合的状态；）Per level compaction stats.  

- 【class】：私有内部类

**Writer**：

- 【class】：私有内部类

**ManualCompaction**：（手动整合的信息）Information for a manual compaction

- 【class】：私有内部类



## dbformat.cc



【静态常量，kMaxSequenceNumber，((0x1ull << 56) - 1) = `0x00ffffffffffffff` ，最大序列号】

- // （，设计：最高位的8个字节空出来，这样就可以和类型字段压缩成1个64字节；为了节省空间；）We leave eight bits empty at the bottom so a type and sequence# can be packed together into 64-bits.

### config：

- 【类型】：名空间
- 【目的】：（，常量组合。）Grouping of constants.  We may want to make some of these parameters set via options.
- 【静态常量，kNumLevels，7，最大的整合层级】
- 【静态常量，kL0_CompactionTrigger，4，（当文件数量到达这个值时，开始L0整合）Level-0 compaction is started when we hit this many files.】
- 【静态常量，kL0_SlowdownWritesTrigger，8，（L0文件数量的软限制，我们可以在此时减缓写入）Soft limit on number of level-0 files.  We slow down writes at this point.】
- 【静态常量，kL0_StopWritesTrigger，12，（L0文件最大数量。这时停止写入）Maximum number of level-0 files.  We stop writes at this point.】
- 【静态常量，kMaxMemCompactLevel，2，？？？】： 
  - Maximum level to which a new compacted memtable is pushed if it does not create overlap.  We try to push to level 2 to avoid the relatively expensive level 0=>1 compactions and to avoid some expensive manifest file operations.  We do not push all the way to the largest level since that can generate a lot of wasted disk space if the same key space is being repeatedly overwritten.

- 【静态常量，kReadBytesPeriod，1048576】



### AppendInternalKey：【func】

```
Ds：将内部key按照如图的方式，追加写到@result尾部。
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

### ExtractUserKey：【func】

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

### 【✔】**ParsedInternalKey** 

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



### 【✔】**InternalKey**：内部key

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



### 【✔】**LookupKey** :

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





## 【⭐】skiplist.h

### **SkipList**：模板跳表

- 【模板类】：（key类型，比较器自定义） `<typename Key, class Comparator>` 
- Note：这个实现的跳表，链是以单向链表的方式实现的。而不是变种的双向链表 + 跳表高度层级。这种实现方式，使得查找最大节点要从头开始往后找



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

KeyIsAfterNode：

```
Para：
  key：
  n：节点，提供被比较值（节点的key）
    Note： 如果 n 为null，则其值是无限大。
Ret：
  1、Return true if key is greater than the data stored in @n
```

FindGreaterOrEqual：

```
Para：
  key：
  prev：【out，二维指针，指向输出节点数组】If prev is non-null, fills prev[level] with pointer to previous node at "level" for every level in [0..max_height_-1].
ret：
  1、Return the earliest node that comes at or after key. 
  2、Return nullptr if there is no such node.
```

FindLessThan：

- 临时变量 x（记录返回值），设置为跳表头部
- level（获取跳表最大高度，往下层level遍历）
- while  True：
  - 获取level层的下一个节点
  - 如果 下一个节点 为null 或者 下一个节点比@key 大：
    - 如果 level已经是第0层，直接返回 x
    - 否则，level --（切换到下一层）
  - 否则， x 更新为下一个节点（x符合 小于 @key）

```
Ds：
Para：
  key：
ret：
  1、Return the latest node with a key < @key.
  2、Return head_ if there is no such node.
```

FindLast：

- 实现方式与上一个完全一致，除去条件判断部分，从最高层的next一直往后找，直到第0层的（next指向null）节点。就是最后一个节点。

```
ret：
  1、Return the last node in the list.
  2、Return head_ if list is empty.
```



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
- key：节点当前的key
- 私有属性：
  - `next_[1]`： 原子模板下的**跳表节点指针** 的柔性数组（，C++的柔性数组：数组的长度等同于节点的高度；）Array of length equal to the node height.  `next_[0]` is lowest level link.
    - 功能：指向下一个节点的高度列表。

> ？ C++是否支持柔性数组，这里需要确认清楚。。。

Next：

- Ds：（链表的访问器 或 修改器；封装在方法里则可以在必要时添加合适的屏障；）Accessors/mutators for links.  Wrapped in methods so we can add the appropriate barriers as necessary.
- Impl： 使用 ‘acquire load' 的方式访问  @n 高度 的下一个节点；

```c
    // Use an 'acquire load' so that we observe a fully initialized
    // version of the returned Node.
	return next_[n].load(std::memory_order_acquire);
```





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

- comparator_：跳表模块的比较器
- refs_：this的引用计数
- arena_：
- table_：跳表；（模板：以编码的key，`char *`， 比较器对解码后的 user key，具体实现使用（内部key比较器））







## version_set

TargetFileSize：返回 @可选项的最大文件大小

MaxGrandParentOverlapBytes：返回  10 * @可选项的最大文件大小

- Ds： Maximum bytes of overlaps in grandparent (i.e., level+2) before we  stop building a single file in a level->level+1 compaction.

ExpandedCompactionByteSizeLimit：返回  25 * @可选项的最大文件大小

- Ds： （，所有整合文件的最大比特数量；。。。）Maximum number of bytes in all compacted files.  We avoid expanding  the lower level file set of a compaction if it would make the  total compaction cover more than this many bytes.

MaxBytesForLevel：

-  Note: the result for level zero is not really used since we set  the level-0 compaction threshold based on number of files.
- Impl：
  - // Result for both level-0 and level-1
  - 结果 = 10 * 1048576.0
  - while @level > 1：
    - 结果 *= 10
    - @level--
  - 返回 结果

MaxFileSizeForLevel:

- //（，当前是固定值；） We could vary per level to reduce number of files?
  - 返回 @可选项的最大文件大小

TotalFileSize：返回 @向量容器“文件元数据” 的大小总和；

### FindFile：

- 使用二分查找法，查找key属于：
  - 使用@内部比较器 比较：
    - // Key at "mid.largest" is < "target".  Therefore all files at or before "mid" are uninteresting.
    - // Key at "mid.largest" is >= "target".  Therefore all files after "mid" are uninteresting.

```
Ds：查找某个第一次出现的文件，@key小于文件的最大key（无法确定在哪一个文件范围内。）
Para：
  icmp：内部比较器
  files：向量容器之文件元数据
    前提：文件的最大key属性，是从小到到大排序的。
  key：
ret： 
   1、所属的数组下标
   2、超过最大下标，说明@key大于所有文件
caller：
	1、给定的文件范围 都是有序的，且无交集的。
```



### AfterFile：

```
ret：
 1、false： null user_key occurs before all keys and is therefore never after *f
 2、true：用户的key 大于 文件的最大key
```

### BeforeFile：

```
ret：
 1、false： null user_key occurs after all keys and is therefore never before *f
 2、true：用户的key 小于 文件的最小key
```



### SomeFileOverlapsRange：

- 如果 有交集：
  - // Need to check against all files
  - 遍历所有的文件：
    - 如果  范围起始key 大于 文件的最大key  或者  范围终止key 小于 文件的最小key：
      - 说明与这个文件无重叠 // on overlap
    - 否则：
      - 返回 true // 有重叠
  - 遍历结束，返回 false
- // Binary search over file list
  - 使用【最大序列号】【插入类型】构建@范围起始key的内容格式key// Find the earliest possible internal key for smallest_user_key
  - 下标（func：查找某个第一次出现的文件，@范围起始key 小于 文件的最大key）

- 如果 下标 越界：
  - 返回 false //beginning of range is after all files, so no overlap.

- @范围终止key   小于  下标文件的最小key： 返回false，无交集
- 否则，返回true，存在交集。

```
Ds：查看某个范围是否存在文件重叠
Para：
  icmp：
  disjoint_sorted_files：标志位，sstable是否为无交集
  files： 向量容器“文件元数据”
  smallest_user_key：范围起始key
  largest_user_key：范围终止key
ret：
  1、true：overlap
  2、false：not 
  
原理图：
  files: [1  10]      [20  50]    [70 100]
    range:        [11   21]
    range:   [9        21]
```



### SaverState：枚举

```c
enum SaverState {
  kNotFound, // 未找到，默认值
  kFound,  // 
  kDeleted,
  kCorrupt,
};
```

### **Saver**：保存器

- state：保存状态
- ucmp：
- user_key：
- value：

### SaveValue：

- 将 ikey解析为内部key
  - 如果失败，arg状态设置为 被破坏
- 否则：
  - 如果解析的用户key  与 保存器的用户key 是相等的：
    - 依据key的类型，设置保存状态。（插入/删除）
    - 如果是插入类型，@v 赋值给 保存器的值。

```
Para：
   arg：【out】 saver指针
   ikey：内部key
   v：value
Caller：
  1、被当做函数指针而进行参数传递，arg也是参数；
```



### **Version**：版本

- Ds：一个版本，代表的是多层级sstable链





#### **GetStats**：读状态

- 【类型：class inner类】
- seek_file：寻址的文件
- seek_file_level：文件所在的层级



#### AddIterators：

- //（，对第0层的所有文件进行合并，因为他们可能存在重叠内容；）Merge all level zero files together since they may overlap
  - 遍历 多层级文件列表数组的第0个元素：
    - @迭代器  入栈  表缓存#创建sstable的迭代器
- // For levels > 0, we can use a concatenating iterator that sequentially walks through the non-overlapping files in the level, opening them lazily.
  - 

```
Ds：（，在@iters里追加放入一个迭代器的序列，而其会产出这个版本合并到一起时的内容；）Append to *iters a sequence of iterators that will yield the contents of this Version when merged together.
Para：
  options：读可选项
  iters： 向量容器之迭代器
    【out】：
REQUIRES: This version has been saved (see VersionSet::SaveTo)
```

#### 【x】Get：

```

```

#### **State**:

- 【函数内部类】
- Ds：记录状态的变量结构体，
- saver：保存器
- stats：读状态
- options：读可选项
- ikey：
- last_file_read：临时变量，最近读文件
- last_file_read_level：临时变量，最近读文件层级
- vset：
- s：状态
- found：标志位，是否匹配。

Match

- 如果 读状态_ 的文件为空 且 最近读文件_ 不为空：
  - // We have had more than one seek for this read.  Charge the 1st file.
  - 。。。

- 状态_ 的最近文件与层级，更新为给定参数；
- 调用 版本_ 里的表缓存的Get方法

```
Ds： 在给定的sstable文件中查找key
type： static
para：
  arg：状态
    【in】：
      state->ikey：需要查找的key
    【out】：
      state->
        saver：保存查找到到的value
        found：记录查找的结果
  level：@f的文件层级
  f：文件元数据；需要在这个sstable文件中查找key
ret： 【caller depend：调用者使用】
  true：（未匹配）继续在其他文件中搜索
  false：（匹配）无需再搜索
```

#### UpdateStats：

```

```

#### RecordReadSample：

```

```





#### 【私有属性】：

- vset_：（，指针：指向这个版本指向的版本集合）VersionSet to which this Version belongs
- `next_`、`prev_`：（，双向循环链表：。。。）Next/Previous version in linked list
- refs_：Number of **live refs** to this version
- 向量容器<文件元数据*> files_ [7 （配置：最大层级）] ： （，sstable文件二维表）  List of files per level
- // Next file to compact based on seek stats.
  - file_to_compact_：【？？？】
  - file_to_compact_level_：【？？？】
- // Level that should be compacted next and its compaction score. Score < 1 means compaction is not strictly needed.  These fields are initialized by Finalize().
  - compaction_score_：【？？？】
  - compaction_level_：【？？？】

```
sstable文件二维表：结构图
  参考：【私有属性】files_
  
              newest                     oldest
       ┌────────────────────────────────────────────────────────┐ 
       │   ┌────────┐    ┌────────┐   ┌────────┐                │ 
level 0│   │sstable ├───►│        ├──►│        ├─► ....         │ 
       │   └────────┘    └────────┘   └────────┘                │ 
       │   ┌────────┐    ┌────────┐   ┌────────┐                │ 
     1 │   │        ├───►│        ├──►│        ├─► ....         │ 
       │   └────────┘    └────────┘   └────────┘                │ 
       │                                                        │ 
       │   ┌────────┐    ┌────────┐   ┌────────┐                │ 
     2 │   │        ├───►│        ├──►│        ├─► ....         │ 
       │   └────────┘    └────────┘   └────────┘                │ 
       │    ......                                              │ 
       └────────────────────────────────────────────────────────┘ 
```



#### 【✔】ForEachOverlapping：

- tmp记录重叠的文件列表
- 遍历 L0层的所有文件（sstable文件二维表_ 的数组第0个；）：// Search level-0 in order from newest to oldest.
  - 如果 @key 大于等于  文件的最小key   且  小于等于 文件的最大key：
    - tmp  入栈 这个文件
- 如果 tmp 不为空：
  - 排序tmp文件列表，按照文件号（newest first）
    - （@func：在给定的sstable文件中查找key）
    - 如果存在：
      - 返回
- 遍历 剩下层级：
  - 文件数为0，continue；
  - 下标（func：FindFile）// Binary search to find **earliest index** whose largest key >= internal_key.
  - 如果 下标  没有越界：
    - 如果  @key  小于 这个下标文件的最小key：
      - 说明这个文件里没有这个key的value，没有查找到；// All of "f" is past any data for user_key
    - 否则：
      - （@func：在给定的sstable文件中查找key）
      - 如果存在：
        - 返回

```
Para：
  user_key：查找key的用户key部分
  internal_key：查找key的内部key部分
  arg：函数指针的参数
    例子：State*
  func：函数指针，在给定的sstable文件中查找key
    例子：static bool Match(void* arg, int level, FileMetaData* f)
  
```

#### NewConcatenatingIterator：



### **LevelFileNumIterator**：

- 【内部类】
- Ds：（，内部迭代器；对于一个给定的 版本/层级 对，产生这个文件所在层级的信息；）An internal iterator.  For a given version/level pair, yields information about the files in the level.  For a given entry, key() is the largest key that occurs in the file, and value() is an 16-byte value containing the file number and file size, both encoded using EncodeFixed64.
- 构造器：将 当前下标_ 设置为 文件个数（marks as invalid）
- 【私有属性】
  - icmp_：
  - flist_：常量向量容器之文件元数据
  - index_：当前下标
  - value_buf_[16] ：【mutable】，值缓冲区中，Backing store for value().  Holds the file number and size.

key：返回 当前下标_ 文件的最大key

value：返回 当前下标_ 文件的 文件号 + 文件大小的EncodeFixed64编码。（内容存放在 值缓冲区_ 中）



### **VersionSet**：版本链



#### LogAndApply：

```
Ds： Apply *edit to the current version to form a new descriptor that  is both saved to persistent state and installed as the new  current version.  Will release *mu while actually writing to the file.

REQUIRES: *mu is held on entry.
REQUIRES: no other thread concurrently calls LogAndApply()

Para：
  edit：
  mu：
```

#### Recover：

- // Recover the last saved descriptor from persistent storage.

#### NeedsCompaction：

- Ds：判断当前版本是否需要整合
- Returns true iff some level needs a compaction.

```c++
  bool NeedsCompaction() const {
    Version* v = current_;
    return (v->compaction_score_ >= 1) || (v->file_to_compact_ != nullptr);
  }
```



#### 【私有属性】

- env_：
- dbname_：
- options_：
- table_cache_：
- icmp_：
- next_file_number_：
- manifest_file_number_： 【？？？】
- last_sequence_：【？？？】
- log_number_：【？？？】
- prev_log_number_：【？？？】，0 or backing store for memtable being compacted
- // Opened lazily
  - descriptor_file_：
  - descriptor_log_：日志写入器
  - dummy_versions_：（版本链双向循环链表的头部）Head of circular doubly-linked list of versions.
  - current_：== `dummy_versions_.prev_`
- compact_pointer_[7 （配置：最大层级）]：【？？？】 （，每一层的key，其在当前层下一个整个开始的位置；1、空字符串 2、有效的内部key；）Per-level key at which the next compaction at that level should start. Either an empty string, or a valid InternalKey.



#### Builder：

- 【内部私有类】
- Ds： A helper class so we can efficiently apply a whole sequence  of edits to a particular state without creating intermediate  Versions that contain full copies of the intermediate state.



### **Compaction**：整合

【私有属性】

- level_：
- max_output_file_size_：
- input_version_：
- edit_：版本编辑
- inputs_[2]  ： 向量容器<文件元数据*>（两个输入的集合）The two sets of inputs
  - // （每个整合从 当前层级_  和下一个层级中读取；）Each compaction reads inputs from "`level_`" and "`level_+1`"
- // （，状态：用于检查有重叠的祖父文件数量）State used to check for number of overlapping grandparent files (parent == level_ + 1, grandparent == level_ + 2)
  - grandparents_：向量容器<文件元数据*> 
  - grandparent_index_：Index in `grandparent_starts_`
  - seen_key_：Some output key has been seen
  - overlapped_bytes_：Bytes of overlap between current output and grandparent files
- // State for implementing IsBaseLevelForKey
  - level_ptrs_[7 （配置：最大层级）] ：`level_ptrs_` holds indices into `input_version_->levels_`: our state is that we are positioned at one of the file ranges for each higher level than the ones involved in this compaction (i.e. for all L >= `level_` + 2).



## 【✔】version_edit

**FileMetaData**：文件元数据

- refs：
- allowed_seeks：Seeks allowed until compaction
- number：文件名的数字号
- file_size：File size in bytes
- smallest：Smallest internal key served by table
- largest：Largest internal key served by table

Tag：【枚举】

- // （尾部数字用于序列化的版本编辑。这些数字是被写入到磁盘并且不应该被改变）Tag numbers for serialized VersionEdit.  These numbers are written to disk and should not be changed.
- 【。。。】

**VersionEdit**：

- 构造器：属性设置为默认值

SetXXX：将对应属性的标志位设置为True，

```
XXX：5个私有属性
```

EncodeTo：

```
Para：
   dst：【out】将this的所有有效的私有属性，都编码输出到上面。
```

DecodeFrom：

```
Para：
  src：上面编码好的内容，解码到 this的属性上。
```

【私有属性】

- comparator_：
- log_number_：
- prev_log_number_：
- next_file_number_：
- last_sequence_：
- has_comparator_：标志位，是否有比较器；
- has_log_number_：标志位，
- has_prev_log_number_：标志位，
- has_next_file_number_：标志位，
- has_last_sequence_：标志位，
- compact_pointers_：`std::vector<std::pair<int, InternalKey>>`  
- deleted_files_：`std::set<std::pair<int, uint64_t>>` ，删除文件的集合；
- new_files_：`std::vector<std::pair<int, FileMetaData>>` ， 新文件的向量容器；



## table_cache

**TableCache**：

NewIterator：

- 调用（func：从表缓存中寻找（已打开）sstable文件，否则打开文件，并将项记录表缓存。）
- 得到sstable对象，创建返回的迭代器。
- 迭代器注册销毁时，对表缓存的去引用，

```
Ds:  Return an iterator for the specified file number (the corresponding  file length must be exactly "file_size" bytes).  If "tableptr" is  non-null, also sets "*tableptr" to point to the Table object  underlying the returned iterator, or to nullptr if no Table object  underlies the returned iterator.  The returned "*tableptr" object is owned  by the cache and should not be deleted, and is valid for as long as the  returned iterator is live.

Para：
  options：
  file_number：
  file_size：
  tableptr： 
     默认值：nullptr
Ret:
  
```

Get：

- （this：FindTable），获得缓存项。
- 如果状态ok：
  - 获取缓存项的中的sstable文件
  - 读取状态（table：InternalGet）从文件中读取key的value
  - 释放缓存项，到淘汰链【？？？】`cache_->Release(handle);`
- 返回读取状态。

```
Ds
Para：
  options：
  file_number：
  file_size：
  k：
  arg：
  handle_result： 函数指针， If a seek to internal key "k" in specified file finds an entry, call (*handle_result)(arg, found_key, found_value).
```

Evict：

```
Ds：Evict any entry for the specified file number
Para：
  file_number
```

### 【私有属性】：

- env_：
- dbname_：
- options_：
- cache_：（key为文件号， value 为 TableAndFile 对象）

### FindTable

- 状态码 = ok
- 以 @文件号 为key 在 缓存_ 中，查找这个 entry
- 如果没有找到：
  - 访问db名 + 文件号的sstable文件
  - 打开sstable文件
  - 如打开状态不ok：
    - // We do not cache error results so that if the error is transient,  or somebody repairs the file, we recover automatically.
  - 否则：
    - 设置 @handle 为  缓存_ 插入 （key为 @文件号， value 为 TableAndFile 对象）
- 返回状态码

```
Ds：从表缓存中寻找（已打开）sstable文件，否则打开文件，并将项记录表缓存。
Para:
  file_number: 文件号
  file_size:
  Cache::Handle ：【out】二维指针
Ret： 状态
```



## builder

BuildTable：

- 创建新的可写文件（文件名 = db名 + （@文件元数据->文件数字号））
- @文件元数据 记录 第一个key
- while 迭代器遍历
  - sstable构建器 添加 key-value
- @文件元数据 记录 最后一个key
- sstable构建器 建造尾部相关元数据
- @文件元数据 记录 文件大小
- 如果状态ok：
  - 文件sync 同步
  - 文件close关闭
- 【。。。】table cache处理。。。
- 如果 状态码 ok 且 文件大小 大于 0：
  - （保持文件，无需动作）keep it
- 否则：
  - 删除文件
- 返回 状态码

```
Ds：（，从@迭代器的内容中构建一个表文件；）Build a Table file from the contents of *iter. 
Para：
  dbname：
  env：
  options：
  table_cache：
  iter：
  meta：
   【out】
     1、 If no data is present in *iter, meta->file_size will be set to  zero, and no Table file will be produced.
     2、成功，则将生成的表，剩余信息填充到这里。
Ret：status
  
```



## write_batch【I】

> （批量写：持有一个更新集合，其会原子性地应用到DB中）WriteBatch holds a collection of updates to apply atomically to a DB.
>
> （，更新内容会以其被添加到批量写的顺序而被更新。例如：如下的批量写，“key” 对应的value会是"v3"；）The updates are applied in the order in which they are added to the WriteBatch.  For example, the value of "key" will be "v3" after the following batch is written:
>
> ```c
> batch.Put("key", "v1");
> batch.Delete("key");
> batch.Put("key", "v2");
> batch.Put("key", "v3");
> ```
>
> Multiple threads can invoke const methods on a WriteBatch without external synchronization, but if any of the threads may call a non-const method, all threads accessing the same WriteBatch must use external synchronization.





**WriteBatch** ：批量写

- 设计分析：
- 接口功能：
  - 添加、删除、清理、估计统计、复制追加、迭代
- 【私有属性】
  - rep_：对于批量加入的key-value，将按照格式编码好后，追加到尾部。

```
rep_ 格式图：
  kv格式：@see batch单个kv格式
┌─────────────────┬──────────┬────────────┬──────────┐
│  sequence num   │ entry num│     kv     │    kv    │
└─────────────────┴──────────┴────────────┴──────────┘
   8字节（小端）     4字节（小端）


```



Put：

- 读取并解码rep_ 的 entry个数，
- 个数++，设置到 rep_中
- 将类型，@key与@value，编码并追加写到 rep_中

```


batch单个kv格式：
INSERT
┌────┬──────┬────────┬─────┬────────┐
│type│ klen │  key   │ vlen│  value │
└────┴──────┴────────┴─────┴────────┘
```

Delete：

- 流程同上，类型不同。

```


单个kv格式图：
 DELETE
 ┌────┬──────┬────────┐             
 │type│ klen │  key   │              
 └────┴──────┴────────┘ 
```



【ok】**Handler**：【抽象接口类】：提供Put、Delete接口。

**WriteBatchInternal**：批量写内部

- Ds：（，功能：提供一些静态方法用于操作批量写对象，而这些方法我们不希望放到公共的 批量写 接口）WriteBatchInternal provides static methods for manipulating a WriteBatch that we don't want in the public WriteBatch interface.
- 【内部类】

Count：Return the number of entries in the batch.

SetCount：Set the count for the number of entries in the batch.

Sequence：Return the sequence number for the start of this batch.

SetSequence：Store the specified number as the sequence number for the start of this batch.

Contents：返回批量写的内容的slice封装

ByteSize：返回批量写的内容的大小

SetContents：字符串assign新的内容。

InsertInto：将批量写的内容，通过迭代的方式，全部插入到内存表；

Append：将一个批量写内容，追加到这个原来的。1、先更新计数 2、再讲源批量写去除头部而追加到目的批量写；

【ok】**MemTableInserter**：

- 【父类】Handler；封装，内存表插入器
- 属性：
  - mem_：内存表
  - sequence_：顺序号

Put、Delete：调用内存表_ 的插入接口（类型参数不同） 顺序号_ ++

## log_format.h：日志格式

文档：`doc/log_format.md`

RecordType：枚举，日志类型

- kZeroType：Zero is reserved for preallocated files
- kFullType：满的
- // for segment
  - kFirstType：分片开始
  - kMiddleType：分片中间
  - kLastType：分片结束

【静态常量，kMaxRecordType，4 = 最后一个类型，最大日志类型的值】

【静态常量，kBlockSize，32768=32KB，日志块的大小】

【静态常量，kHeaderSize，7 = 4 + 2 + 1， 日志头部大小=Header is checksum (4 bytes), length (2 bytes), type (1 byte)】

## log_writer



**Writer**：日志器

- 构造器：可写文件对象
- 【私有属性】：
  - dest_：可写文件对象
  - block_offset_：Current offset in block
  - type_crc_[5（最大日志类型的值 + 1）]：crc32c values for all supported record types.  These are pre-computed to reduce the overhead of computing the crc of the record type stored in the header. 

AddRecord：

- 剩余长度 = @slice 长度
- // （，。。。，如果slice是空的，我们依旧会迭代一次而发出单个长度为0的记录）Fragment the record if necessary and emit it.  Note that if slice is empty, we still want to iterate once to emit a single zero-length record
- 开始标志位 =  true
- do
  - 块剩余空间 = 【日志块大小】 - 块偏移_
  - 如果 剩余长度  小于   7字节【日志块头部大小】：
    - 如果剩余长度大于0，可写文件_ 剩余长度使用 "\x00" 字符填充
    - 块偏移_ 重置为0
  - 可用空间 = 【日志块大小】 - 块偏移_ -  7字节
  - 分片长度 = Min(剩余长度， 可用空间)
  - 结束标志位 = 分片长度 等于 剩余长度
  - 如果  是开始 且 是结束
    - 日志类型 = 满的
  - 否则如果  是开始：
    - 日志类型 = 分片开始
  - 否则如果 是结束：
    - 日志类型 = 分片结束
  - 否则：
    - 日志类型 = 分片中间
  - （func：发出物理日志块，刷到日志文件中）
  - 内容指针 += 分片长度，剩余长度 -= 分片长度，开始标志位 设置 false；
- while 状态ok 且 剩余长度 大于 0

```
Para：
  slice：插入的记录
ret：Status
```

===private

EmitPhysicalRecord：

```
Ds：发出物理日志块，刷到日志文件中
Para：
  t：记录类型
  ptr：内容其实地址
  length：长度
ret： Status


日志项个数图：
  data格式：@see 批量写的_rep格式图
  
   4bytes     2bytes   1byte               
┌───────────┬─────────┬────┬──────────────┐
│ crc32     │ length  │type│     data     │
└───────────┴─────────┴────┴──────────────┘
  little     little
  endian     endian
  - crc32c of type and data[]
```



## log_reader



**Reader**：

- 构造器：
  - 1、Create a reader that will return log records from "*file". "*file" must remain live while this Reader is in use.
  - 2、If "reporter" is non-null, it is notified whenever some data is dropped due to a detected corruption.  "*reporter" must remain live while this Reader is in use.
  - 3、If "checksum" is true, verify checksums if available.
  - 4、The Reader will start reading at the first record located at physical position >= initial_offset within the file.



**Reporter：**

- 【内部类】用于报告错误的接口

Corruption：

```
Ds：Some corruption was detected.  "bytes" is the approximate number of bytes dropped due to the corruption.
Para：
  bytes：
  status：【out】引用修改
```



## Snapshot





## repair




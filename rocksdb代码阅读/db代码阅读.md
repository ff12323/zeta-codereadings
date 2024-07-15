



概念的记录：

- column family（列族）
  - 【？？？】什么东西？
- follower database、leader database
- resume db xxx（重新开始DB的xxx）、outstanding error（正在进行的错误）



第一次的记录：

- ch02：
  - // (，由于这是个非常大的类，函数的定义被划分到几个 xxx文件中）Since it's a very large class, the definition of the functions is divided in several `db_impl_*.cc` files, besides db_impl.cc.





## ch01 db.h

DB：

- // （，DB是一个持久化的，版本化的，有序的key到value的映射。） A DB is a persistent, versioned ordered map from keys to values. A DB is safe for concurrent access from multiple threads without any external synchronization. DB is an **abstract base class** with one primary implementation (DBImpl) and a number of wrapper implementations.

Open：

```
Open：
  Ds：（，打开一个特定名字的数据库用于读/写。。。。）Open the database with the specified "name" for reads and writes. 
  Para：
    options：引用，db的可选项
    name： 引用，db的名字
    dbptr： 二维指针，Stores a pointer to a heap-allocated database *dbptr 
      约束：Caller must delete *dbptr when it is no longer needed.
  Ret： Status
    OK：success
    非OK； error
      1、if the DB is already open (read-write) by another DB object. (This guarantee depends on options.env->LockFile(), which might not provide this guarantee in a custom Env implementation.)
```

Open：

```
Open：
  Ds：Open DB with column families.
  Para：
    db_options ：specify database specific options
    name： 引用，db的名字
    column_families： 
       1、column_families is the vector of all column families in the database, containing column family name and options. You need to open ALL column families in the database. To get the list of column families, you can use ListColumnFamilies().
       2、 The default column family name is 'default' and it's stored in ROCKSDB_NAMESPACE::kDefaultColumnFamilyName.
    handles：
    	If everything is OK, handles will on return be the same size as column_families --- handles[i] will be a handle that you will use to operate on column family column_family[i]. Before delete DB, you have to close All column families by calling DestroyColumnFamilyHandle() with all the handles.
    dbptr： 
    	二维指针，Stores a pointer to a heap-allocated database *dbptr 
  Ret： Status
    OK：success
    非OK； error
      1、if the DB is already open (read-write) by another DB object. (This guarantee depends on options.env->LockFile(), which might not provide this guarantee in a custom Env implementation.)
```



OpenAsFollower

- // （，打开一个数据库作为一个**从数据库**；区别：有着自己的目录而链接到实际文件，并可以容忍。。。？？？；  2、会尝试周期地追上主数据库的主线。。。。）Open a database as a follower. The difference between this and opening as secondary is that the follower database has its own directory with links to the actual files, and can tolarate **obsolete file** deletions by the leader to its own database. Another difference is the follower tries to keep up with the leader by periodically tailing the leader's MANIFEST, and (in the future) memtable updates, rather than relying on the user to manually call TryCatchupWithPrimary().

```
OpenAsFollower：
	Overload： Open as a follower with the default column family

OpenAsFollower：
	Overload： Open as a follower with multiple column families
```





Resume：

- //（，手动地，同步地尝试在一个写失败到下层文件系统而<u>重新开始DB写</u>；） Manually, synchronously attempt to **resume DB** writes after a write failure to the underlying filesystem. See https://github.com/facebook/rocksdb/wiki/Background-Error-Handling

```
Resume：
  Ret：
    OK：if writes are successfully resumed, or there was no outstanding error to recover from.
    error： （，如果是不可恢复的，则返回潜在的写错误）underlying write error if it is not recoverable.
 WART: 
 	Does not mix well with auto-resume. Will return Busy if an auto-resume is in progress, without waiting for it to complete. 
 See：
 	DBOptions::max_bgerror_resume_count and EventListener::OnErrorRecoveryBegin
```



## ch02  db_impl

DBImpl：

- // （，DB是RocksDB的公共接口，而DBImpl是实际的实现类；其是RocksDB核心引擎的入口；所有其它的DB实现，如TransactionDB，BlobDB等，在内部上封装了一个DBImpl）While DB is the public interface of RocksDB, and DBImpl is the actual class implementing it. It's the entrance of the **core RocksdB engine**. All other DB implementations, e.g. TransactionDB, BlobDB, etc, wrap a DBImpl internally.
- // 。。。
- // (，由于这是个非常大的类，函数的定义被划分到几个 xxx文件中）Since it's a very large class, the definition of the functions is divided in several `db_impl_*.cc` files, besides db_impl.cc.

```
DBImpl：
  father： DB
```


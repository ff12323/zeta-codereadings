

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



Open

```
Open:
  Ds： Open the database with the specified "name".
  Caller: should delete *dbptr when it is no longer needed.
  Ret：
  	 1、ores a pointer to a heap-allocated database in *dbptr and returns OK on success. 
  	 2、ores nullptr in *dbptr and returns a non-OK status on error. 
```



Put

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



Delete

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

Get

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

NewIterator：

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

GetSnapshot

```
Ds：（，对于这个快照而创建的迭代器，将都能观察到对当前的DB状态的一个稳定快照）Iterators created with this handle will all observe a stable snapshot of the current DB state. 
Caller：
    The caller must call ReleaseSnapshot(result) when the snapshot is no longer needed.
Ret：快照
  a handle to the current DB state.
```

ReleaseSnapshot：

- Ds：Release a previously acquired snapshot.
- Caller：The caller must not use "snapshot" after this call.





。。。

。。。





DestroyDB

```
Ds： Destroy the contents of the specified database. Be very careful using this method.
Para：
  name：
  options：
Note: （，为了向后兼容性。。。）For backwards compatibility, if DestroyDB is unable to list the database files, Status::OK() will still be returned masking this failure.
```



RepairDB

- （，如果一个DB无法打开，你可以尝试调用这个方法恢复尽可能多的DB内容；。。。）If a DB cannot be opened, you may attempt to call this method to resurrect as much of the contents of the database as possible. Some data may be lost, so be careful when calling this function on a database that contains important information.







### DBImpl：

- father：DB



Get：

- @see：`DB::Get`
- 




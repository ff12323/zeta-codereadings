





MemTable：

- 构造器：（，被引用计数，调用者必须引用一次；）MemTables are reference counted.  The initial reference count is zero and the caller must call Ref() at least once.
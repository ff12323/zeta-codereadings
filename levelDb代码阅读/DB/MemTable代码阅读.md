





**MemTable**：

- 构造器：（，被引用计数，调用者必须引用一次；）MemTables are reference counted.  The initial reference count is zero and the caller must call Ref() at least once.



Get：

```
Ds：
Para：
  key：
  value：【out】返回值
  s： 【out】返回值
Ret：
  1、@key有对应的value，设置到@value，返回true
  2、@key有删除类型的，设置@s为NotFound() error， 返回true
  3、二者都不存在，返回false。
```


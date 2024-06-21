



不懂的概念的记录：

- L0（Level-0 files）
- internal keys（内部key）： ？？？
  - 由  user key + 8bit 组成。
- helper class







ValueType：

- 枚举类型，表示编码后的key是**删除键**/**正常键** 。是嵌入到下盘的结构体中。

ParsedInternalKey ：

- 对于 内部key 进行解析之后，转为对象，包含关键的属性。

InternalKeyComparator ：

- // A comparator for internal keys that uses a specified comparator for the user key portion and breaks ties by decreasing sequence number.

> break ties： the relationship between two people has come to an end

InternalFilterPolicy ：

- // Filter policy wrapper that converts from internal keys to user keys

InternalKey：

- // Modules in this directory <u>should keep internal keys wrapped inside the following class</u> instead of plain strings so that we do not incorrectly use string comparisons instead of an InternalKeyComparator.

LookupKey :

- //  A helper class useful for `DBImpl::Get()`
- 私有属性：
  - `end_ - start_` ： 字符数组，表示一个memtable的key
  - `end_ - kstart_`： 表示一个内部key
  - `end_ - kstart_ - 8`： 表示 userkey

```
  // We construct a char array of the form:
  //    klength  varint32               <-- start_
  //    userkey  char[klength]          <-- kstart_
  //    tag      uint64
  //                                    <-- end_
  // The array is a suitable MemTable key.
  // The suffix starting with "userkey" can be used as an InternalKey.
```


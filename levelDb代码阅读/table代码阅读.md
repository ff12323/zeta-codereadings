



```
extent：the area covered by something.（范围）
```



**Block**：

- 私有属性：
- data_：数据内容的起始地址。
- size_：数据内容的大小。
- restart_offset_：（，**重启数组**？？？在数据中的偏移。）Offset in `data_` of restart array
- owned_：`Block owns data_[]`





## format文件



### **BlockHandle**：块范围

- // （，是一个指针，指向一个文件的存放一个数据块或元数据块的所在范围）BlockHandle is a pointer to the extent of a file that stores a data block or a meta block.

### **Footer** ：尾部

- // （，封装固定的信息，存放在每一个table文件的尾部）Footer encapsulates the fixed information stored at the tail end of every table file.
- 私有属性：
- metaindex_handle_： 元数据块范围
- index_handle_：数据块范围



### **BlockContents**： slice的封装



> kBlockTrailerSize（块尾部大小）：5（1-byte type + 32-bit crc）



ReadBlock：

- （，读取块内容 + 块尾部大小 ）Read the block contents as well as the type/crc footer.
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







## table.cc（接口实现）

**Table类**

### 1、public

Open：

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



### 2、private

- rep_：私有结构体








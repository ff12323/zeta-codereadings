







### **Arena**：

- Ds： 内存范围对象，对于小块的内存分配，统一用块大小管理（可以支持对齐/不对齐的地址），可以避免内存碎片化？而大块的内存直接分配。
- 私有属性：
  - alloc_ptr_：记录分配的状态，多分配的内存，其中剩余的空间的起始地址。
  - alloc_bytes_remaining_：剩余空间的大小。
  - blocks_：块列表，记录分配的起始地址，方便后续释放内存。

Allocate：注意，使用new分配的内存地址，总是对齐的。

- @bytes：字节数
- Ret：
  - （，返回一个指向新分配的内存块的指针，大小为xxx字节）Return a pointer to a newly allocated memory block of "@bytes" bytes.
- // The semantics of what to return are a bit messy if we allow 0-byte allocations, so we disallow them here (we don't need them for our internal use).
  - 内部使用，assert @字节数 > 0

- 如果 @字节数 小于等于 [剩余空间大小]：
  - 从 剩余的 直接分配，并返回 地址。

- 否则剩余空间被丢弃（浪费），返回（func：直接分配内存块）。



AllocateAligned：同上，带上对齐。

- 获取对齐大小，其必须是 2^n，符合公式（1.1）
- 将 [剩余的空间的起始地址] & (对齐 - 1) 得到取余后的不对齐的大小。
- 如果 对齐后的字节数 小于等于 [剩余空间大小]：
  - 【。。。】从 剩余的 对齐开始的地方进行分配。并返回
- 否则 ，返回（直接分配内存块）

$$
x \& (x -1) = 2^{n} , n=0,1,2,..., x \in \Z^{+}  \\
  y \& (2^n -1) = modval\tag{1.1}
$$

===private

AllocateFallback：

- @bytes：分配字节数
- 如果 分配字节数 大于 【静态常量，kBlockSize ，4096，块大小】的1/4：
  - // Object is more than a quarter of our block size.  Allocate it separately to avoid wasting too much space in leftover bytes.
  - 返回（func：直接分配内存块）
- // （当前的块剩余空间可能存在浪费）We waste the remaining space in the current block.
  - （func：直接分配内存块），大小为（**块大小**）
  - 减去实际使用大小，设置[剩余的空间的起始地址] 以及[剩余的空间大小]

- 返回分配的地址

AllocateNewBlock：直接分配内存块。

- @block_bytes：
- new参数指定的内存块，将地址放到 [块列表]中，[内存统计] fetch_add更新。返回分配的地址。





### Varint

Endian-neutral encoding:

- （，固定长度的数组被编码为LSB优先）Fixed-length numbers are encoded with least-significant byte first
- In addition we support variable length "varint" encoding
- Strings are encoded prefixed by their length in varint format



EncodeVarint64：

- 定义一个常量B，其值为128（0b10000000）。这是因为varint编码使用7位来表示数值，最高位用来标识是否还有下一个字节。
- 将dst指针转换为一个uint8_t类型的指针ptr，这样可以直接操作内存中的字节。
- while  v 大于等于 B： 说明varint的字节还有后续。
  - 将v的值与B进行按位或操作（取v的低7位，且MSB置1），然后将结果存储在ptr指向的内存中，并将ptr向后移动一位。
  - 将v右移7位。去除已编码的内容，继续。
- 当v小于B时，将v的值转换为一个uint8_t类型，然后存储在ptr指向的内存中，并将ptr向后移动一位。

```
DS： 对64位整数进行varint64编码
Para：
  dst：varint编码的buffer，起始地址（include）
  v：被编码的，64位无符号整数
Ret：
  varint编码的buffer，结束地址（not include）
```





## 附录A



### 1、《protocol buffers，Base 128 Varints》

（，变长整数。这种编码允许无符号64位整数使用在1字节到10字节之间，小的值使用更少的字节。）Variable-width integers, or *varints*, are at the core of the wire format. <u>They allow encoding unsigned 64-bit integers using anywhere between one and ten bytes, with small values using fewer bytes.</u>

> 使用场景：这种编码方式常用于网络通信和数据存储，特别是在需要高效存储和传输数字数据的场景中。

（，varint的每个字节的MSB是一个**连续标志位**，表示后面一个字节是不是varint的一部分；而低位的7比特是负载；）Each byte in the varint has a *continuation bit* that indicates if the byte that follows it is part of the varint. This is the *most significant bit* (MSB) of the byte (sometimes also called the *sign bit*). The lower 7 bits are a payload; <u>the resulting integer is built by appending together the 7-bit payloads of its constituent bytes.</u>

So, for example, here is the number 1, encoded as ``01`` – it’s a single byte, so the MSB is not set:

```proto
0000 0001
^ msb
```

And here is 150, encoded as ``9601`` – this is a bit more complicated:

```proto
10010110 00000001
^ msb    ^ msb
```

How do you figure out that this is 150? First you drop the MSB from each byte, as this is just there to tell us whether we’ve reached the end of the number (as you can see, it’s set in the first byte as there is more than one byte in the varint). These 7-bit payloads are in little-endian order. Convert to big-endian order, concatenate, and interpret as an unsigned 64-bit integer:

```proto
10010110 00000001        // Original inputs.
 0010110  0000001        // Drop continuation bits.
 0000001  0010110        // Convert to big-endian.
   00000010010110        // Concatenate.
 128 + 16 + 4 + 2 = 150  // Interpret as an unsigned 64-bit integer.
```

Because varints are so crucial to protocol buffers, in protoscope syntax, we refer to them as plain integers. `150` is the same as ``9601``.








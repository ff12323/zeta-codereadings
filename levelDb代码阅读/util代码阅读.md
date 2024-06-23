







Arena：

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






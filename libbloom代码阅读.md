地址：https://github.com/jvirkki/libbloom



布隆过滤器的缺点：

- （，不支持删除操作）bloom filter doesn’t support the delete operation
- false positives rate can’t be reduced to zero

Removing an item from the bloom filter is not supported because there is no possibility to identify the bits that should be cleared. There might be other items that map onto the same bits and clearing any of the bits would introduce the possibility of false negatives.





### 1、结构体

bloom：（，用于追踪一个布隆过滤器的结构体。调用者：需要分配这个内存，并传入下面的函数；。。。）Structure to keep track of one bloom filter.  Caller needs to allocate this and pass it to the functions below. First call for every struct must be to bloom_init().

- // （，这些字段是结构图的公共接口的部分；客户端代码可以读取值，但是不能修改值；）These fields are part of the **public interface of this structure**. Client code may read these values if desired. Client code **MUST NOT** modify any of these.
  - entries：期望的插入项的个数
  - bits：使用的比特位的数量
  - bytes：使用的字节数量（向上取整）
  - hashes：哈希函数的个数
  - error：冲突的概率
- // Fields below are **private to the implementation**. These may go away or change incompatibly at any moment. Client code MUST NOT access or rely on these.
  - ready：初始化标记，初始化完成会设置。
  - major：
  - minor：
  - bpe：【？？？ 需要理解原理】
  - bf：布隆过滤器的内存区域，大小为%bytes 



### 2、API

#### bloom_init2：

- // The filter is initialized with a bit field and number of hash functions according to the computations from the wikipedia entry:  http://en.wikipedia.org/wiki/Bloom_filter
- 如果 `sizeof(unsigned long int)` 小于8字节：
  - error: libbloom will not function correctly ， exit-退出程序。
- 参数的约束值，进行校验。
  - 失败，返回 1
- 设置bpe字段：`log(1/error) / 0.480453013918201`  = 

$$
\ln 2 = 0.693147180559945  \\
\ln^2 2 = 0.480453013918201 \\
\frac{\log_2  { 1 \over \text{error}}}{ \ln^2 2 }
$$

- 设置bites字段： 期望数 * bpe
  - 根据比特数，计算字节数。
- 设置哈希函数的个数： 向上取整（ ln(2) * bpe)
- bf： calloc分配置0内存
  - 如果分配失败，返回 1
- 。。。

```
bloom_init2：
  Ds： 初始化布隆过滤器
  Para：
     bloom  :  Pointer to an allocated struct bloom (see above).
     entries:  （，插入项的期望数量。 最少是1000个）The expected number of entries which will be inserted. Must be at least 1000 (in practice, likely much larger).
     error  :  （， 冲突的概率， 约束值 (0, 1) ）Probability of collision (as long as entries are not exceeded).
  Return:
     0 - on success
     1 - on failure
```





#### bloom_check：

```
bloom_check:
  Ds:  Check if the given element is in the bloom filter. Remember this may return false positive if a collision occurred.
  Parameters:
     bloom  - Pointer to an allocated struct bloom (see above).
     buffer - Pointer to buffer containing element to check.
     len    - Size of 'buffer'.
  Return:
     0 - element is not present
     1 - element is present (or false positive due to collision)
    -1 - bloom not initialized
```



bloom_check_add：

- 未初始化，报错，返回 -1
- 哈希值a  等于 （func：murmurhash2）以 `0x9747b28c` 为seed ，对buffer元素，计算哈希值
- 哈希值b  等于 同上，但是以哈希值a为seed。
- 遍历 哈希函数个数：
  - 比特位下标 = （哈希值a + 哈希值b *  哈希函数下标）取余  比特总数
  - （func：检查  @比特位下标 的值， @chekc），查看是否为1：
    - 是，则 （命中）++
  - 否则：
    - 如果是check，直接 返回 0   （（，check类型，如果1个比特不命中。无需关注其它）Don't care about the presence of all the bits. Just our own.）
- 如果 （命中） 等于 哈希函数个数：
  - 返回 1 （element already in (or collision)）
- 返回 0  （，这里表示 add类型，但是元素不存在，添加进去了）

```
bloom_check_add：
  Ds： 检查 与 添加 功能类似，抽取到1个函数上实现
  Para：
    【upper】：同上
    add：
      0： 表示check
      1： 表示add
  Ret：同上
```



test_bit_set_bit：

- 比特位转位字节位
- `c = buf[byte];`        // expensive memory access
  - 【？？？】为什么这样数组访问内存代价很高？
- 获取比特位，在字符c中的掩码(mask)；
- 检查比特位是否为1：c & mask
  - 是，则 返回 1
- 否：
  - 如果是add，比特位置1
  - 返回 0

```
test_bit_set_bit:
  Ds:  检查某个比特位的值，根据参数值1/或0
  Para：
    buf：
    bit：
    set_bit：
      0： check
      1： add
  Ret：
    1： 比特位是1
    0： 比特位是0
```



#### bloom_add：

```
bloom_add：
  Ds： Add the given element to the bloom filter.
  Parameters:
     bloom  - Pointer to an allocated struct bloom (see above).
     buffer - Pointer to buffer containing element to check.
     len    - Size of 'buffer'.
 Return:
     0 - （，元素不存在，并被添加了）element was not present and was added
     1 - （，1、元素已经被添加进去了  2、可能是发生冲突了）element (or a collision) had already been added previously
    -1 - bloom not initialized
```


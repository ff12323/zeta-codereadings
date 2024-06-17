地址：https://github.com/jvirkki/libbloom





### 1、结构体

bloom：（，用于追踪一个布隆过滤器的结构体。调用者：需要分配这个内存，并传入下面的函数；。。。）Structure to keep track of one bloom filter.  Caller needs to allocate this and pass it to the functions below. First call for every struct must be to bloom_init().

- // （，这些字段是结构图的公共接口的部分；客户端代码可以读取值，但是不能修改值；）These fields are part of the **public interface of this structure**. Client code may read these values if desired. Client code **MUST NOT** modify any of these.
  - entries：
  - bits：
  - bytes：
  - hashes：
  - error：
- // Fields below are **private to the implementation**. These may go away or change incompatibly at any moment. Client code MUST NOT access or rely on these.
  - ready：
  - major：
  - minor：
  - bpe：
  - bf：




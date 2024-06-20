





Slice：

- // （，是一个简单的结构体包含一个指向外部存储的指针，和其大小；Slice的用户必须确保。。。不再使用。。。在外部存储被释放后；）Slice is a simple structure containing a pointer into some **external storage** and a size.  The user of a Slice must ensure that the slice is not used after the corresponding external storage has been deallocated. 
- // Multiple threads can invoke const methods on a Slice without external synchronization, <u>but if any of the threads may call a non-const method, all threads accessing the same Slice must use external synchronization.</u>







​	

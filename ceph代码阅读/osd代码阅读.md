



概念的索引：

- PG：Replica Placement Group



## Dispatcher

**Dispatcher**：消息分发器

- 【类型】：抽象类

ms_can_fast_dispatch：

 The Messenger calls this function to query if you are capable  of "fast dispatch"ing a message. Indicating that you can fast  dispatch it requires that you:

- 1、Handle the Message quickly and without taking long-term contended  locks. (This function is likely to be called in-line with message  receipt.)
- 2、Be able to accept the Message even if you have not yet received  an ms_handle_accept() notification for the Connection it is associated  with, and even if you *have* called mark_down() or received an  ms_handle_reset() (or similar) call on the Connection. You will  not receive more than one dead "message" (and should generally be  prepared for that circumstance anyway, since the normal dispatch can begin,  then trigger Connection failure before it's percolated through your system).  We provide ms_handle_fast_[connect|accept] calls if you need them, under  similar speed and state constraints as fast_dispatch itself.
- 3、Be able to make a determination on fast_dispatch without relying  on particular system state -- the ms_can_fast_dispatch() call might  be called multiple times on a single message; the state might change between  calling ms_can_fast_dispatch and ms_fast_dispatch; etc.

```
@param
  m： The message we want to fast dispatch.
@returns
  True if the message can be fast dispatched; 
  false otherwise.
```






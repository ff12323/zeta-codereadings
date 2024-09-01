











ibv_wq：

>  Work Queue. QP can be created without internal WQs "packaged" inside it,  this QP can be configured to use "external" WQ object as its  receive/send queue.
>
>  WQ associated (many to one) with Completion Queue it owns WQ properties  (PD, WQ size etc).
>
> WQ of type IBV_WQT_RQ:
>
> - Contains receive WQEs, in this case its PD serves as scatter as well.
> - Exposes post receive function to be used to post a list of work requests (WRs) to its receive queue.

- 。。。（字段略）




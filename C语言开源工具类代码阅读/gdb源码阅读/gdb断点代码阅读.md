



### 1.1 断点类型

- 事件点已经被删除
- 普通断点
- 硬件辅助的断点
- until命令断点
- finish命令断点
- 观察点
- 硬件辅助的观察点
- 。。。
- 观察点范围断点：用于探测什么时候一个观察点表达式超出了其范围。这些断点通常不被用户看见
  - 其与观察点之间1：1映射
  - 当相关的观察点被命中时，自动删除2者。
  - 无法被禁止。

```c
enum bptype
  {
    bp_none = 0,		/* Eventpoint has been deleted */
    bp_breakpoint,		/* Normal breakpoint */
    bp_hardware_breakpoint,	/* Hardware assisted breakpoint */
    bp_until,			/* used by until command */
    bp_finish,			/* used by finish command */
    bp_watchpoint,		/* Watchpoint */
    bp_hardware_watchpoint,	/* Hardware assisted watchpoint */
    bp_read_watchpoint,		/* read watchpoint, (hardware assisted) */
    bp_access_watchpoint,	/* access watchpoint, (hardware assisted) */
    bp_longjmp,			/* secret breakpoint to find longjmp() */
    bp_longjmp_resume,		/* secret breakpoint to escape longjmp() */
        
    //...
        
    /* Used to detect when a watchpoint expression has gone out of
       scope.  These breakpoints are usually not visible to the user.

       This breakpoint has some interesting properties:

       1) There's always a 1:1 mapping between watchpoints
       on local variables and watchpoint_scope breakpoints.

       2) It automatically deletes itself and the watchpoint it's
       associated with when hit.

       3) It can never be disabled.  */
    bp_watchpoint_scope,
```


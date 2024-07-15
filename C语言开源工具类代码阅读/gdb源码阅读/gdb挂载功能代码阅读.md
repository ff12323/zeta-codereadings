



名词解释：

- Inferior（下级）：

  - "Inferior" is a general term to mean "something that you are using gdb to debug" -- generally a process or perhaps a kernel running on an emulator or on some other piece of hardware connected on a serial line.

    The term "Inferior debugger" comes up when you are using gdb to debug gdb. That is, you have TWO gdb processes running, one of which (the main gdb) is watching and controlling (setting breakpoints, single stepping, etc) the second (the "inferior debugger"). The inferior debugger is itself probably controlling some other program.

```
(gdb) info inferiors 
  Num  Description       Executable        
* 1    process 12858     /usr/bin/true
```



_initialize_infcmd：初始化gdb命令

- attach_command：通过add_cmd添加  `attach` 命令的入口 + help文档
  - wait_for_inferior：
    - 描述：等待控制从下级返回到调试器。

```
/* Wait for control to return from inferior to debugger.

   If TREAT_EXEC_AS_SIGTRAP is non-zero, then handle EXEC signals
   as if they were SIGTRAP signals.  This can be useful during
   the startup sequence on some targets such as HP/UX, where
   we receive an EXEC event instead of the expected SIGTRAP.

   If inferior gets a signal, we may decide to start it up again
   instead of returning.  That is why there is a loop in this function.
   When this function actually returns it means the inferior
   should be left stopped and GDB should read more commands.  */
```


仓库地址：https://sourceware.org/git/?p=glibc.git;a=summary







## ch01 动态链接器

`elf\dl-usage.c`：

```c
void
_dl_help (const char *argv0, struct dl_main_state *state)
{
  _dl_printf ("\
Usage: %s [OPTION]... EXECUTABLE-FILE [ARGS-FOR-PROGRAM...]\n\
You have invoked 'ld.so', the program interpreter for dynamically-linked\n\
ELF programs.  Usually, the program interpreter is invoked automatically\n\
when a dynamically-linked executable is started.\n\
```







## ch02 pthread

目录：`NPTL`


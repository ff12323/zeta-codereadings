

记录：文档级与使用级的理解虽然很实用。但是不站在源码级的理解上，永远无法窥见全面。（有很多隐含的特性、问题、现象，或者代码路径、思想等等。。。是文档与使用，所无法描述，映射的。只有通过源码去发现。）







## ld

> https://stackoverflow.com/questions/2216540/whats-the-meaning-of-line-in-c-language
>
> It tells the compiler where the following line actually came from. It's usually only the C preprocessor that adds these, for example, when including a file, it tells the compiler (which is basically only seeing one stream of data) that we're looking at a different file.
>
> This may sound strange, but the preprocessor simply inserts the header files where you specify your includes, and the compiler works on the whole thing (all header files concatenated along with your source code), you can check the result of the preprocessor stage if using gcc with `gcc -E myfile.c`. In there you'll notice it adds a `#line` directive whenever you include files, and also whenever it reduces the amount of text fed to the compiler (such as large amounts of comments may be reduced to a single `#line` directive, skipping ahead)
>
> It is also used by other programs, such as bison/yacc to tell you that the problem (if there's a compile problem) is related to your rules-file at a specific line, which the compiler would otherwise be unable to do, as the bison/yacc generates c-files.

```c
  case 4: /* command: NAME opt_name opt_base  */
#line 147 "deffilep.y"
                                       { def_image_name ((yyvsp[-1].id), (yyvsp[0].vma), 0); }
#line 1462 "deffilep.c"
    break;
```

==========

`ld\ldfile.h`：库文件的搜索路径。

```c
/* Structure used to hold the list of directories to search for
   libraries.  */

typedef struct search_dirs {
  /* Next directory on list.  */
  struct search_dirs *next;
  /* Name of directory.  */
  const char *name;
  /* TRUE if this is from the command line.  */
  bool cmdline;
} search_dirs_type;

```




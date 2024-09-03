# zeta-codereadings



AutoHotKey util： convert repo code comment  to  markdown oneline txt

```
^!m::  ; Ctrl+Alt+M  
clipboard := RegExReplace(clipboard,"m)^[\s\*/]+","")
clipboard := RegExReplace(clipboard,"`r`n"," ")
clipboard := RegExReplace(clipboard,"`r"," ")
clipboard := RegExReplace(clipboard,"`n"," ")
SendInput ^v
return
```


```
git config http.proxy 'socks5://127.0.0.1:10808'
git config https.proxy 'socks5://127.0.0.1:10808'
```


代码阅读的目的：

- 基础方面：加强熟悉语法，代码风格，习惯等方面
  - 自身经验浅薄，实践少。弥补认知方面的不足
  - 学习代码在实践中的技巧等
- 浅层次方面：见识真实、复杂的代码是如何布局的，沟通，发展的。认识复杂的数据结构，算法，及其精妙之处。
- 深入方面：从架构，设计的方面去理解、认识代码。由浅入深，不断积累而窥见全貌。

代码阅读笔记的目的：

- 有些仓库不存在代码注释
- 存在代码注释，但是描述的层级较高。具体内部实现的，需要自己注释（除非代码专门有一本书描写）。
- 场景：工作中或是现实，往往已经有完善的代码环境，设施。读代码的占比  远大于  写代码。
  - 业务代码的阅读方式与基础库代码的阅读方式，有时是差别很大的。笔记的方式也会不同

- 演化：
  - 从具体细节，变化为 主线要点
- 不必要性：复杂的


> redis原因：
>
> 为什么需要代码阅读记录？1、因为人与代码仓库、代码作者之间的认知经验差异，导致无法理解代码、注释等。2、代码本身的注释的问题（缺少、错误、不够详尽）3、扩展补充、测试记录、实践发现等内容
>
> - 现象01：有时候自身基础不足，对于代码的注释越看越无法理解，这是代码语言与自然语言不合的体现。

阅读的策略：

- 1、自下而上
- 2、自上而下

阅读的个人经验：

- 1、代码阅读是“量变产生质变”的最令自己感受深刻的地方，自一门生疏的语言到渐渐熟悉，积累的个人实践方法与经验。



更多笔记的规范： （@see 《tmpNoteSpecific.md》）

【🤢🤢🤢】

【🤢：exception】

❌✔

❓🔥

### 1、复杂项目的代码阅读

对于一些实际的项目，会很复杂。程序的内部跳转，关联给人一种错综复杂的感觉。代码本身有着一套自己未曾见过的框架结构，建立在这种框架结构之上的一套复杂的业务，业务本身是一种分布式的复杂逻辑，在关键的、复杂的数据结构背后，又包含着一批不重要的。

在代码量较大时，应该采用怎么样的阅读策略？常用的方法有

- 【基于文件-自底向上阅读】，对于一个模块，先确认模块文件之间的依赖关系，然后按照 自底向上 的方式遍历阅读文件，对于每个文件，都是从开始往下翻阅即可（因为，编写的代码大多都是开始是基础部分，往后面就是调用上面的上层部分）。
  - 遇到的问题：代码中存在废弃代码，无效代码，重复代码时，这种阅读方式难于确认而到浪费时间。
  - 适用场景：自身是第一次接触如此规模、场景的代码时，可以通过这种方式快速而全面的过一遍代码。

对于不熟悉的人来说，会感觉无从下手，难于确定入口。

> 代码阅读笔记的记录困难：
>
> 超多层的层级结构链，内容量很大，这样的内容记录在文本中。有很多缺点：
>
> 1、代码笔记的记录困难，排布组织不方便。
>
> 2、阅读困难，量太大。
>
> 3、代码给人一种抽象、牵涉方面甚多（从上到下）。即便自己选择了一个入口点开始阅读，但无法完全地理解这个部分。因为这不像千百行的代码，独立的一个demo，代码因素很很少，容易直接完全理解。但是代码量上来，代码因素变得很多，即便是一小部分代码，也会有环环相扣的情况，如此发散下去，会发现要完全理解这一个小部分代码，似乎涉及到整个项目以及原理设计等内容。
>
> - 怎么样算完成理解一部分的代码？需要完整理解整个项目，才能算完全理解一部分吗？
> - 代码组件之间的边界情况，无法界定边界使得阅读困难化。
>
> 4、代码映射关系的困难：1、功能特性的入口点在哪里？  2、异步流程分析  3、框架之间跳转  4、关键流程与路径   5、现象与代码之间的映射关系 6、。。。。。
>
> - 对于新手来说，代码映射关系的不确定、不熟悉，会导致边界问题更容易陷入其中，而耗费大量时间无所得。
> - 文档化是代码映射关系的体现，使人易于上手。



延伸问题：在阅读百行、千行、万行、十万、百万行、千万行、亿万行代码时，应该采取怎样的方式呢？我是否有过这样的经验呢？





#### 1.1 实际项目中的代码量

14w行的c代码

```
   |      extension|     total code|  total comment|    total blank|percent|
   -------------------------------------------------------------------------
   |           .cpp|          37971|           3742|           6311|    4.5|
   |             .c|         141691|           8460|          22740|     17|
   |            .py|            428|             81|            104|  0.051|
   |             .h|         489451|         433604|         121152|     59|
```




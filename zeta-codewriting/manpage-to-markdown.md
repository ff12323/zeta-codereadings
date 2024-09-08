



处理流程：

- 1、将html转为markdown文本
- 2、这时markdown文本还需简单处理下
  - 1、以2个换行符分隔md文本为1个块
    - 按照 （场景2：块多层递进）
  - 2、输出为md格式



场景1：块拆分

- 1、拆分为多个块
- 2、首行拆分

流程：

- 遍历块内所有的行：
  - 如果：所有行的x偏移量 与 【首行偏移量】相等，则（转换为单行）。

```
       ip route delete
              delete route
              ip route del has the same arguments as ip route add, but
              their semantics are a bit different.
```

场景2：块多层递进

- 【前偏移量】初始化值为0，【递进层级列表】初始化
- 遍历所有的块：下一轮循环前记录【前偏移量】
  - 如果块的【首行偏移量】是0
    - continue
  - 如果【前偏移量】为0
    - 则记录【首行偏移量】，并标记递进层级为1。
    - 【递减层级列表】重置。
    - 并记录第一个【递进层级列表】
  - 查看块是否可以拆分，是：
    - 是则拆分，按照（场景1：块拆分）流程处理，并标记每一个块的递进层级。
  - 比较【前偏移量】。
    - 如果 大于 ：则记录之，并递进层级++
    - 如果 等于：则递进层级不变。
    - 如果 小于：
      - 反向搜索【递减层级列表】
      - 找到第一个相同【偏移量】的，记录为相等的递进层级。

```
       ip route replace
              change or add new one
              
              nexthop NEXTHOP
                     the nexthop of a multipath route.  NEXTHOP is a
                     complex value with its own syntax similar to the
                     top level argument lists:

                             via [ FAMILY ] ADDRESS - is the nexthop
                             router.
```

问题：

1、如何处理单个块内的层级多变的异常场景呢？

- 假设不存在，先忽略。

```
       ip route replace
              change or add new one
    ip-----------------------------
```

2、块的层级间递进过多的场景。

直接处理是不行的。可以假设一个虚拟块，填充进去。（暂时没必要）

```

【】【】【】

    【】【】【】
    
    		【】【】【】
    	
    	【】【】【】
    
    【】【】【】
```

3、无法判断出是否存在层级关系。

如下：例子1实际上应该是1行（或者2行，但是第一行的后一个部分要与下一行合并），而例子2则是2行。

- 暂不处理，不好判断，手动修改。

```
       basic  Filter packets based on an ematch expression. See
              tc-ematch(8) for details.
              
       tc -g class show dev eth0
           Shows classes as ASCII graph on eth0 interface. 
```

4、表格处理不了










对人而言，要实现一个目标（完成一个项目、实现一个功能、......等），要从一个人所处的状态来分析。

- 1、高态：已经实现过更复杂的，所以实现很简单
- 2、等态：已经实现过同等难度的，所以实现不困难
- 3、次级态：处于次一级状态，实现起来困难，但是可以达到。
- 4、低态：自身层级存在差距，迁越实现困难极大，差距越多则越困难。

> 迭代的过程：为了达到等态，需要先达到次级态，如果未达到，则把次级态当做等态，递归之；
>
> 自身经历：
>
> 1、我经历过很多次低态，比如上手一个困难的工作。而自身基础不足，导致进展缓慢而长时间无法做好基础的功能内容（1、无法理解等态间交流的基本概念与话语  2、无法理解基础的场景）。更不必说更复杂的内容。
>
> 2、只有认识到真正复杂的事物，才能算真正接近真实的世界，才能真正地面对现实与真正的困难。认识不是仅有理论认识，还有实践认识。




博客：NUMA 处理器与进程绑定

- https://harrychen.xyz/2022/05/08/numa-processor-and-cpu-binding/



ch13：

- 第一次：
  - The default mempool handler is ring based.
  - X86 architecture, performance can be greatly improved by adding a specific padding between objects. The objective is to ensure that the beginning of each object starts on a different channel and rank in memory so that all channels are **equally loaded**.
- 不懂的概念：
  - DRAM or DDR3 channels、DIMM
  - **ranks** on any DIMM：the number of independent sets of DRAMs that can be accessed for the full data bit-width of the DIMM.
  - Arm systems、Modern Arm Interconnects、SN-F (memory channel) 
  - compare-and-set (CAS)、per-**lcore** local cache
  - data plane


 英语生词：

```
logarithm ： 对数
ternary  ： 三
```



功能：（，对数的底为整数2的计算）Integer base 2 logarithm calculation





ilog2：log base 2 of 32-bit or a 64-bit unsigned value- 

- @n: parameter
- （，支持常量的底数为2的log计算 -  这个可以用于从常量数据进行初始化全局变量；因此有着大量的三元操作符的构建）constant-capable log of base 2 calculation - this can be used to initialise global variables from constant data, hence the massive ternary operator construction
- selects the appropriately-sized optimised version depending on sizeof(n)

> 【？？？】

```c
#define ilog2(n) \
( \
	__builtin_constant_p(n) ?	\
	((n) < 2 ? 0 :			\
	 63 - __builtin_clzll(n)) :	\
	(sizeof(n) <= 4) ?		\
	__ilog2_u32(n) :		\
	__ilog2_u64(n)			\
 )
```


 英语生词：

```
logarithm ： 对数
ternary  ： 三
```



功能：（，对数的底为2的整数计算）Integer base 2 logarithm calculation



### 1、ilog2

ilog2：log base 2 of 32-bit or a 64-bit unsigned value

- @n: parameter
- （，支持常量的底数为2的log计算 -  这个可以用于从常量数据进行初始化全局变量；因此有着大量的三元操作符的构建）constant-capable log of base 2 calculation - this can be used to initialise global variables from constant data, hence the massive ternary operator construction
  - 整数参数是常量：
    - 如果小于2（等于1，理论上不能为0），则其整数对数必定为0；
    - 否则，将参数转为64位（unsigned long long），计算其从右边到左边最高位为1的（总比特位数）。由于是二级制数，且计算的是log 2的向下取整，（总比特位数）正好是其值。
- （，如果不是常量，则依据其大小，选择合适的优化版本进行计算）selects the appropriately-sized optimised version depending on sizeof(n)
  - 。。。


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



#### 1.1 __builtin_constant_p

GCC

Built-in Function: `int` **__builtin_constant_p** `(exp)`

（，功能：判断表达式是否为编译时的一个常量，因此GCC可以执行与这个值相关的表达式执行常量折叠；）You can use the built-in function `__builtin_constant_p` to determine if the expression exp is known to be constant at compile time and hence that GCC can perform constant-folding on expressions involving that value. The argument of the function is the expression to test. The expression is not evaluated, side-effects are discarded. The function returns the integer 1 if the argument is known to be a compile-time constant and 0 if it is not known to be a compile-time constant. <u>Any expression that has side-effects makes the function return 0. A return of 0 does not indicate that the expression is *not* a constant, but merely that GCC cannot prove it is a constant within the constraints of the active set of optimization options.</u>

（，常量折叠是一个编译器相关的优化，被很多现代编译器所使用。）Constant folding and constant propagation are related compiler optimizations used by many modern compilers

You typically use this function in an embedded application where memory is a critical resource. If you have some complex calculation, you may want it to be folded if it involves constants, but need to call a function if it does not. For example:

```c
#define Scale_Value(X)      \
  (__builtin_constant_p (X) \
  ? ((X) * SCALE + OFFSET) : Scale (X))
```

You may use this built-in function in either a macro or an inline function. However, if you use it in an inlined function and pass an argument of the function as the argument to the built-in, GCC never returns 1 when you call the inline function with a string constant or compound literal (see [Compound Literals](https://gcc.gnu.org/onlinedocs/gcc/Compound-Literals.html)) and does not return 1 when you pass a constant numeric value to the inline function unless you specify the -O option.

You may also use `__builtin_constant_p` in initializers for static data. For instance, you can write

```c
static const int table[] = {
   __builtin_constant_p (EXPRESSION) ? (EXPRESSION) : -1,
   /* … */
};
```

This is an acceptable initializer even if EXPRESSION is not a constant expression, including the case where `__builtin_constant_p` returns 1 because EXPRESSION can be folded to a constant but EXPRESSION contains operands that are not otherwise permitted in a static initializer (for example, `0 && foo ()`). GCC must be more conservative about evaluating the built-in in this case, because it has no opportunity to perform optimization.

#### 1.2 __builtin_clzll

一共有三个函数，分别适用于不同的输入类型。

```c
int __builtin_clz (unsigned int x)
//Returns the number of leading 0-bits in x, starting at the most significant bit position. If x is 0, the result is undefined.

int __builtin_clzl (unsigned long)
//Similar to __builtin_clz, except the argument type is unsigned long.

int __builtin_clzll (unsigned long long)
//Similar to __builtin_clz, except the argument type is unsigned long long.
```

这个函数作用是返回输入数二进制表示从最高位开始(左起)的连续的0的个数；如果传入0则行为未定义。三个不同的函数分别用于unsigned int，unsigned long以及unsigned long long。

===

```c
int __builtin_clzl(unsigned long x) {
    for (int i = 0; i != 64; ++i)
        if (x >> 63 - i & 1) 
            return i;
}

int __builtin_clzl(unsigned long x) {
    int r = 0;
    if (!(x & 0xFFFFFFFF00000000)) r += 32, x <<= 32;
    if (!(x & 0xFFFF000000000000)) r += 16, x <<= 16;
    if (!(x & 0xFF00000000000000)) r += 8,  x <<= 8;
    if (!(x & 0xF000000000000000)) r += 4,  x <<= 4;
    if (!(x & 0xC000000000000000)) r += 2,  x <<= 2;
    if (!(x & 0x8000000000000000)) r += 1,  x <<= 1;
    return r;
}
```


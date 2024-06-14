

### 1、结构体设计

hlist_head ：用于散列表，表示列表头（数组中的一项）

- first：哈希空间的一个项的属性，执行冲突链的第一个节点。

```c
struct hlist_head {
	struct hlist_node *first;
};
```

> 设计意义：
>
> `hash_table`的列表头仅存放一个指针,也就是`first`指针,指向的是对应链表的头结点,没有`tail`指针,也就是指向链表尾节点的指针,这样的考虑是为了节省空间——尤其在`hash bucket`(数组size)很大的情况下可以节省一半的指针空间。

hlist_node用于散列表，表示列表头所在双向链表中的某项

- next：指向冲突链的下一个节点
- pprev：二维指针，指向前一个节点的next指针（实际等同于next指针，指向前一个节点），但是由于列表头的节点类型不同，故而用这个办法统一。

```c
struct hlist_node {
	struct hlist_node *next, **pprev;
};
```



![img](.\.img\hashtable_hlist.drawio.svg)





### 2、工具和宏

#### DEFINE_HASHTABLE：

- name：数组变量名
- bits：数组大小为 2^{bits}
  - 通过指定初始化器，将所有的列表头元素的指针，初始化指向NULL

```c
#define DEFINE_HASHTABLE(name, bits)						\
	struct hlist_head name[1 << (bits)] =					\
			{ [0 ... ((1 << (bits)) - 1)] = HLIST_HEAD_INIT }
```

> GNU语法：designated initializer
>
> - https://stackoverflow.com/questions/47202557/what-is-a-designated-initializer-in-c
>
> The Designated Initializer came up since the ISO C99 and is a different and more dynamic way to initialize in C when initializing `struct`, `union` or an `array`.
>
> The biggest difference to standard initialization is that you don't have to declare the elements in a fixed order and you can also omit element.
>
> GNU Guide：
>
> - Standard C90 requires the elements of an initializer to appear in a fixed order, the same as the order of the elements in the array or structure being initialized.
> - In ISO C99 you can give the elements in random order, specifying the array indices or structure field names they apply to, and GNU C allows this as an extension in C90 mode as well
>
> 1. Array Index
>
> **Standard Initialization**
>
> ```c
>   int a[6] = { 0, 0, 15, 0, 29, 0 };
> ```
>
> **Designated Initialization**
>
> ```c
>   int a[6] = {[4] = 29, [2] = 15 }; // or
>   int a[6] = {[4]29 , [2]15 }; // or
>   int widths[] = { [0 ... 9] = 1, [10 ... 99] = 2, [100] = 3 };
> ```



#### 哈希表对象的大小

HASH_SIZE：

- 给定变量名称，

```c
#define HASH_SIZE(name) (ARRAY_SIZE(name))
```

ARRAY_SIZE：

- （获取arr数组中元素的格式）get the number of elements in array @arr
- @arr: （被获取大小的数组）array to be sized
  - 通过 sizeof 获取数组对象的总大小  除  单个元素大小 而得到

```c
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))
```

__must_be_array：

- &a[0] degrades to a pointer: a different type from an array

```c
#define __must_be_array(a)	BUILD_BUG_ON_ZERO(__same_type((a), &(a)[0]))
```

BUILD_BUG_ON_ZERO：

- （，如果条件成立则强制出现编译错误。）Force a compilation error if condition is true, but also produce a result (of value 0 and type int), so the expression can be used e.g. in a structure initializer (or where-ever else comma expressions aren't permitted). 

```c
#ifdef __CHECKER__
#define BUILD_BUG_ON_ZERO(e) (0)
#else /* __CHECKER__ */
#define BUILD_BUG_ON_ZERO(e) ((int)(sizeof(struct { int:(-!!(e)); })))
#endif /* __CHECKER__ */
```






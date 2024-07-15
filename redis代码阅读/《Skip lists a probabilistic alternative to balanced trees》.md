论文信息：《Skip lists: a probabilistic alternative to balanced trees》
作者：William Pugh  
地址：https://15721.courses.cs.cmu.edu/spring2017/papers/07-oltpindexes1/pugh-skiplists-cacm1990.pdf



## 第0章 自我记录



伪代码：

- 跳表特性：有序性，是一种有序链表

```
SkipListInsert(list, key):
  pre = findLessThan(list, key)
  if pre == NULL
    return "key exist..."
  next = pre->next
  lv = randomLevel()
  nd = allocNode(lv)
  alterLinks(pre, nd)
  alterLinks(nd, next)


SkipListSearch(list, key):  //搜索  ❌❌❌（写的垃圾）
  endList = list
  while 1:
    lv = list.lv
    skey = list.key
    if sKey == key:
      return list
    else if sKey < key:
      list = skipList_Choose(list)
      if list == NULL:
        return NULL
    else if sKey > key: 
      if endList == list:
        return NULL
      endList = skipList_ChoosePre(list)
      if endList == NULL || endList == list:
        return NULL
      return SkipListSearch(endList, key)
```



感想：

- 读论文的好处在于领域专业性与前沿知识。是极致深度的一种体现，其专业术语、独到创新的概念、特有的经验实践性、不断更新交流。是传统书籍所不能及也。

## ch01 

> （， 跳表使用概率均衡而不是严格地采用的均衡。。。。）Skip lists are a data structure that can be used in place of balanced trees. Skip lists use probabilistic balancing rather than strictly enforced balancing and as a result the algorithms for insertion and deletion in skip lists are much simpler and significantly faster than equivalent algorithms for balanced trees.

Binary trees can be used for representing abstract data types such as dictionaries and ordered lists. They work well when the elements are inserted in a random order. Some sequences of operations, such as inserting the elements in order, produce degenerate data structures that give very poor performance. If it were possible to randomly permute the list of items to be inserted, trees would work well with high probability for any input sequence. In most cases queries must be answered on-line, so randomly permuting the input is impractical. Balanced tree algorithms re-arrange the tree as operations are performed to maintain certain balance conditions and assure good performance.

Skip lists are a probabilistic alternative to balanced trees. Skip lists are balanced by consulting a random number generator. Although skip lists have bad worst-case performance, no input sequence consistently produces the worst-case performance (much like quicksort when the pivot element is chosen randomly). It is very unlikely a skip list data structure will be significantly unbalanced (e.g., for a dictionary of more than 250 elements, the chance that a search will take more than 3 times the expected time is less than one in a million). Skip lists have balance properties similar to that of search trees built by random insertions, yet do not require insertions to be random.

Balancing a data structure probabilistically is easier than explicitly maintaining the balance. For many applications, skip lists are a more natural representation than trees, also leading to simpler algorithms. The simplicity of skip list algorithms makes them easier to implement and provides significant constant factor speed improvements over balanced tree and self-adjusting tree algorithms. Skip lists are also very space efficient. They can easily be configured to require an average of $1 {1\over3}$ pointers per element (or even less) and do not require balance or priority information to be stored with each node.

### （跳表）SKIP LISTS  










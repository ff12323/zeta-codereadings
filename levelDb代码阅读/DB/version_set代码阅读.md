









Version：版本

- 私有属性：
  - vset_：（，指针：指向这个版本指向的版本集合）VersionSet to which this Version belongs
  - `next_`、`prev_`：（，双向循环链表：。。。）Next/Previous version in linked list
  - refs_：Number of **live refs** to this version










### [704. 二分查找](https://leetcode.cn/problems/binary-search/)

> think：
>
> 为了完成这个题目，编写的代码是为了达到对  所有的输入，都产生正确的输出。这样的代码称为 “完全代码”
>
> 但是由于自身的问题，导致编写出的代码 只对一部分输入，产生正确的输出；而剩余的是错误的（BUG）。称为 “半完全代码”
>
> - 场景：错误数量越少，越接近“完全代码”
>
> 而有的代码是所有输入（或是绝大部分，可以显而易见地看到错误），产生错误的输出；称为“未实现代码”
>
> ===
>
> 那么问题就出现了，自己应该如何识别出混杂在"完全代码"中 的 “半完全代码” ？如何发现自己编写的 “半完全代码”？
>
> 更复杂的问题：如何有意地修改 将 “完全代码” <==> “半完全代码” ？
>
> ===
>
> 更复杂的问题：“完全代码” 可以存在非常多种方式的解决思路与办法，全面地了解、认识、思考都是一件较为复杂、耗时的事情。
>
> 

#### 代码问题01：

1、未考虑到边界情况，比如 `[0, 2]`，target = 2

- mid = 0，导致while的判断条件无法进去，而直接取第0个值当做目标比较；

```c
// 错误代码
int search(int* nums, int numsSize, int target) {
    int left = 0;
    int right = numsSize - 1;
    int mid = (left + right)/2 ;
    while(mid > left) {
        if(nums[mid] > target){
            right = mid;
        }

        if(nums[mid] <= target) {
            left = mid;
        }
        mid = (left + right)/2;
    };
    if( nums[mid] == target) 
        return mid ;
    else
        return -1;
}
```

#### 缺陷分析与改进：

- 1、采用mid的计算，[][] left与right是2个相邻的下标，mid的计算公式的取整而只能取到一个边界的值；
- 2、错误2修改
- 状态：OK

```c
int search(int* nums, int numsSize, int target) {
    int left = 0;
    int right = numsSize - 1;
    int mid = (left + right)/2 ;
    while(left<=right-1) {
        if(nums[mid] > target){
            right = mid;
        }
        else if(nums[mid] < target) {
            left = mid;
        }
        else {
            return mid;
        }
        
        if(mid == right - 1) {  //错误2：  left == right - 1 ;  [-1,0,5] ， target = -1，输出下标 -1
            mid = right;  // 由于 mid的计算是向下取整，所以处于left 与 right 相邻的情况下， left在上面已经不匹配，直接尝试right
            break;
        }else {
            mid = (left + right )/2;
        }
    };
    if( nums[mid] == target) 
        return mid ;
    else
        return -1;
}
```

#### 他人的方法

优点#01：

- 1、实际使用求中间mid索引建议用这种方法：int mid = left + (right-left)/2; 可以防止left+right溢出（超出整数范围）
-  2、对于left 和 right下标修改，原先的范围过大，mid已经比较过了。移动一个位置可以简化循环的判断条件；

如果 left == right（已经往前移动到right）：

- right < target：left再往前，大于right
- right > target：right往后，小于left

```c
int search(int* nums, int numsSize, int target) {
    int left = 0;
    int right = numsSize - 1;
    while(left <= right) {
        int mid = left + (right - left)/2;
        if(nums[mid] < target) {
            left = mid + 1;  
        }else if (nums[mid] > target) {
            right = mid - 1;
        }else {
            return mid;
        }
    }
    return -1;
}
```



#### 我的变种方法

修改点：

- 1、while循环中 只需要比较1次，但是退出时，不确定 right 和 mid 哪个满足条件；

```c
int search(int* nums, int numsSize, int target) {
    int left = 0;
    int right = numsSize - 1;
    int mid;
    while(left < right) {
        mid = (right+left) / 2;
        if(nums[mid] < target) {
            left = mid + 1;  
        }else {
            right = mid;
        }
    }
    
    if( nums[mid] == target) 
        return mid ;
    else if (nums[right] == target) 
       return right;
    else
        return -1;
}
```








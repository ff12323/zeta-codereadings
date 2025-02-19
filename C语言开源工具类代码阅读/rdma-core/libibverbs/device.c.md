



## 1、ibv_get_device_list

```
LATEST_SYMVER_FUNC(ibv_get_device_list, 1_1, "IBVERBS_1.1",
		   struct ibv_device **,
		   int *num)
{
```

### 调用栈实现

ibv_get_device_list

- ibverbs_get_device_list
  - find_sysfs_devs_nl（Fetch the list of IB devices and **uverbs** from **netlink**）
    - nl_send_simple：`libnl` 库中存在一个特殊的接口用来发送简单的消息。这个函数接收 `netlink` 消息类 型、可选的 `netlink` 消息标志以及可选的数据缓冲区和数据长度。
      - `RDMA_NLDEV_CMD_SYS_GET`：`linux/include/uapi/rdma /rdma_netlink.h`

### 【问题】接口宏定义的原因？

1. **`LATEST_SYMVER_FUNC`**：
   - 这是一个宏，用于定义一个带有符号版本控制的函数。
   - 它的作用是生成一个函数的实现，并将该函数与特定的版本号绑定。
2. **`ibv_get_device_list`**：
   - 这是函数的名称，表示我们要定义的函数是 `ibv_get_device_list`。
3. **`1_1`**：
   - 这是函数的版本号，表示这个函数属于 `libibverbs` 库的 1.1 版本。
4. **`"IBVERBS_1.1"`**：
   - 这是符号版本字符串，用于在动态链接时标识函数的版本。
   - 当程序运行时，动态链接器会根据这个字符串找到正确的函数实现。
5. **`struct ibv_device \**` 和 `int \*num`**：
   - 这是函数的返回类型和参数类型。
   - `struct ibv_device **` 表示函数返回一个指向 `ibv_device` 结构体指针的指针。
   - `int *num` 是一个输出参数，用于返回设备数量。
6. **`{ .... }`**：
   - 这是函数的具体实现代码。

------

**符号版本控制的作用**

在动态库（如 `libibverbs`）中，符号版本控制的主要目的是：

1. **兼容性**：
   - 当库的开发者更新库时，可能会修改函数的实现。
   - 通过符号版本控制，旧版本的应用程序仍然可以链接到旧版本的函数实现，而新版本的应用程序可以使用新版本的函数。
2. **多版本共存**：
   - 同一个库中可以存在多个版本的函数实现。
   - 例如，`ibv_get_device_list` 可能有 1.0 和 1.1 两个版本，分别对应不同的实现。
3. **避免冲突**：
   - 如果没有符号版本控制，当库更新时，旧版本的应用程序可能会因为函数实现的变化而崩溃。

------

如何理解这段代码？

1. **动态库的版本管理**：

   - 动态库（如 `.so` 文件）可以包含多个版本的函数。
   - 每个版本的函数都有一个唯一的符号版本字符串（如 `"IBVERBS_1.1"`）。

2. **函数的版本绑定**：

   - 这段代码将 `ibv_get_device_list` 的实现绑定到 `"IBVERBS_1.1"` 版本。
   - 当应用程序链接到 `libibverbs` 时，动态链接器会根据符号版本字符串找到正确的函数实现。

3. **代码生成**：

   - `LATEST_SYMVER_FUNC` 宏会展开为实际的函数定义，并添加符号版本信息。

   - 例如，它可能会生成类似以下的代码：

     ```
     __asm__(".symver ibv_get_device_list, ibv_get_device_list@IBVERBS_1.1");
     struct ibv_device **ibv_get_device_list(int *num) {
         // 函数实现
     }
     ```

------

初学者如何理解？

1. **动态库和版本控制**：
   - 动态库是一个独立的代码文件（如 `.so` 文件），可以被多个程序共享。
   - 当库更新时，可能会出现多个版本的函数。
2. **符号版本控制的作用**：
   - 它确保旧程序可以继续使用旧版本的函数，而新程序可以使用新版本的函数。
3. **代码的作用**：
   - 这段代码定义了一个函数 `ibv_get_device_list`，并将其绑定到 `"IBVERBS_1.1"` 版本。
   - 这样，当程序运行时，动态链接器会确保调用的是正确版本的函数。

------

示例

假设 `libibverbs` 有两个版本的 `ibv_get_device_list`：

- 1.0 版本：`ibv_get_device_list@IBVERBS_1.0`
- 1.1 版本：`ibv_get_device_list@IBVERBS_1.1`

当应用程序链接到 `libibverbs` 时：

- 如果应用程序需要 1.0 版本的函数，动态链接器会调用 `ibv_get_device_list@IBVERBS_1.0`。
- 如果应用程序需要 1.1 版本的函数，动态链接器会调用 `ibv_get_device_list@IBVERBS_1.1`。

------

总结

- 这段代码的作用是定义一个带有符号版本控制的函数 `ibv_get_device_list`。
- 符号版本控制确保动态库的更新不会破坏现有程序的兼容性。
- 对于初学者来说，可以将其理解为一种“函数版本管理”机制，确保不同版本的函数可以共存并被正确调用。
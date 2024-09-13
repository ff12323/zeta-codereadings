

Doxgen格式注释：

> From the doxygen [documentation](http://www.doxygen.nl/manual/commands.html#cmdlink), `\link` is used to refer to objects like a file, class or member and takes a reference to one of these as its first argument. For example, if I wanted to refer to a class method `func` in the class `myClass`, I would use
>
> ```
> \link myClass::func link text ... \endlink
> ```



## platform

### 1、接口

> 》：
>
> 多平台兼容适配的基础操作函数：
>
> - 一些基础的系统调用
> - 创建临时文件，获取路径名，判断路径名，等功能



## buffer

### 1、接口

> 》：
>
> buffer是一个多区域划分的内存空间的表示，记录起始地址，偏移地址，实际起始地址等位置信息。
>
> gc_arena：记录gc分配内存的链表。可以包含特殊的内存（有自定义释放函数）
>
> 字符串相关：character class
>
> 字符串处理函数
>
> buffer读取，输出工具函数



## comp

### 1、接口

> 》：
>
> compress_options：记录压缩算法类型与压缩标记。
>
> 支持的压缩算法：lzo，lz4（通过编译宏，进行选择性支持）
>
> 压缩回话上下文



## options

### 1、接口





## crypto_backend

1、接口

> 》：
>
> cipher、message digest



## openvpn

### 1、接口

【structure，context】：

> Contains all state information for one tunnel.
>
> This structure represents one VPN tunnel. It is used to store state information related to a VPN tunnel, but also includes process-wide data, such as configuration options.
>
> The [Structure of VPN tunnel state storage](https://build.openvpn.net/doxygen/tunnel_state.html) related page describes how this structure is used in client-mode and server-mode.

- options：Options loaded from command line or configuration file.
- first_time：True on the first iteration of OpenVPN's main loop.
- mode：Role of this context within the OpenVPN process.
  - CM_P2P：standalone point-to-point session or client
  - CM_TOP：top level of a multi-client or point-to-multipoint server
  - CM_TOP_CLONE：clone of a CM_TOP context for one thread
  - CM_CHILD_UDP，CM_CHILD_TCP：child context of a CM_TOP or CM_THREAD
- gc：Garbage collection arena for allocations done in the scope of this context structure.
- es：Set of environment variables.
- net_ctx：Networking API opaque context
- sig：Internal error signaling object.
- plugins：List of plug-ins.
- plugins_owned：Whether the plug-ins should be cleaned up when this context is cleaned up.
- did_we_daemonize：Whether demonization has already taken place.
- persist：
- // （持久化上下文）Persistent context
  - c0：Level 0 context
  - c1：Level 1 context
  - c2：Level 2 context

### 2、实现



tunnel_point_to_point：

```
Ds：Main event loop for OpenVPN in client mode, where only one VPN tunnel is active.
Para：
  c：The context structure of the single active VPN tunnel.
```

- 设置上下文的模式为点对点模式；
- 



## forward

### 1、接口

encrypt_sign：

（，处理一个将会被发送通过一个VPN隧道的数据通道包。）Process a data channel packet that will be sent through a VPN tunnel.

This function controls the processing of a data channel packet which will be sent through a VPN tunnel to a 【remote OpenVPN peer】. It's general structure is as follows:

- Check that the client authentication has succeeded; if not, drop the packet.
- If the `comp_frag` argument is true:
  - Call `lzo_compress()` of the [Channel Compression module](https://build.openvpn.net/doxygen/group__data__control.html) to (possibly) compress the packet.
  - Call `fragment_outgoing()` of the [Channel Fragmentation module](https://build.openvpn.net/doxygen/group__data__control.html) to (possibly) fragment the packet.
- Activate the **Channel Crypto module** to perform security operations on the packet.
  - Call `tls_pre_encrypt()` to choose the appropriate security parameters for this packet.
  - Call `openvpn_encrypt()` to encrypt and HMAC signed the packet.
  - Call `tls_post_encrypt()` to prepend the one-byte OpenVPN header and do some TLS accounting.
- Place the resulting packet in `c->c2.to_link` so that it can be sent over the external network interface to its remote destination by the [External Multiplexer](https://build.openvpn.net/doxygen/group__external__multiplexer.html).



### 2、实现



encrypt_sign：

```

```


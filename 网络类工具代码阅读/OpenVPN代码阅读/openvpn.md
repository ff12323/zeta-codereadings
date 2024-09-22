

Doxgen格式注释：

> From the doxygen [documentation](http://www.doxygen.nl/manual/commands.html#cmdlink), `\link` is used to refer to objects like a file, class or member and takes a reference to one of these as its first argument. For example, if I wanted to refer to a class method `func` in the class `myClass`, I would use
>
> ```
> \link myClass::func link text ... \endlink
> ```



## ch0 记录

概念：

- schedule：（一个随机化的二叉搜索树）random **treap** binary tree



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



## env_set

### 1、接口

> 》：
>
> env_set：gc内存块链 + 环境遍历列表



## crypto_backend

1、接口

> 》：
>
> cipher、message digest



## crypto_openssl



## crypto

### @file 

（数据通道密码学模块）Data Channel Cryptography Module

@addtogroup data_crypto Data Channel Crypto module

Crypto packet formats
The Data Channel Crypto module supports a number of crypto modes and configurable options. The actual packet format depends on these options. A Data Channel packet can consist of:

- \b Opcode, one byte specifying the packet type (see @ref network_protocol
"Network protocol").
- \b Peer-id, if using the v2 data channel packet format (see @ref network_protocol "Network protocol").
- \b HMAC, covering the ciphertext IV + ciphertext. The HMAC size depends on the \c \-\-auth option. If \c \-\-auth \c none is specified, there is no HMAC at all.
- \b Ciphertext \b IV. The IV size depends on the \c \-\-cipher option.
- \b Packet \b ID, a 32-bit incrementing packet counter that provides replay protection.
- \b Timestamp, a 32-bit timestamp of the current time.
- \b Payload, the plain text network packet to be encrypted (unless
encryption is disabled by using \c \-\-cipher \c none). The payload might
already be compressed (see @ref compression "Compression module").

This section does not discuss the opcode and peer-id, since those do not depend on the data channel crypto. See @ref network_protocol "Network protocol" for more information on those.

\e Legenda 
<tt>[ xxx ]</tt> = unprotected 
<tt>[ - xxx - ]</tt> = authenticated 
<tt>[ * xxx * ]</tt> = encrypted and authenticated

<b>CBC data channel cypto format</b> 

In CBC mode, both TLS-mode and static key mode are supported. The IV consists of random bits to provide unpredictable IVs. 

<i>CBC IV format:</i> 

<tt> [ - random - ] </tt> 

<i>CBC data channel crypto format in TLS-mode:</i> 

<tt> [ HMAC ] [ - IV - ] [ * packet ID * ] [ * packet payload * ] </tt> 

<i>CBC data channel crypto format in static key mode:</i> 

<tt> [ HMAC ] [ - IV - ] [ * packet ID * ] [ * timestamp * ]
[ * packet payload * ] </tt>

<b>CFB/OFB data channel crypto format</b> 

CFB and OFB modes are only supported in TLS mode. In these modes, the IV consists of the packet counter and a timestamp. If the IV is more than 8 bytes long, the remaining space is filled with zeroes. The packet counter may not roll over within a single TLS sessions. This results in a unique IV for each packet, as required by the CFB and OFB cipher modes.

<i>CFB/OFB IV format:</i> 

<tt>   [ - packet ID - ] [ - timestamp - ] [ - opt: zero-padding - ] </tt>

<i>CFB/OFB data channel crypto format:</i> 

<tt>   [ HMAC ] [ - IV - ] [ * packet payload * ] </tt>

<b>GCM data channel crypto format</b> 

GCM modes are only supported in TLS mode.  In these modes, the IV consists of the 32-bit packet counter followed by data from the HMAC key.  The HMAC key can be used as IV, since in GCM and CCM modes the HMAC key is not used for the HMAC.  The packet counter may not roll over within a single TLS sessions. This results in a unique IV for each packet, as required by GCM.


The HMAC key data is pre-shared during the connection setup, and thus can be omitted in on-the-wire packets, saving 8 bytes per packet (for GCM and CCM).


In GCM mode, P_DATA_V2 headers (the opcode and peer-id) are also authenticated as Additional Data.

<i>GCM IV format:</i> 

<tt>   [ - packet ID - ] [ - HMAC key data - ] </tt>

<i>P_DATA_V1 GCM data channel crypto format:</i> 

<tt>   [ opcode ] [ - packet ID - ] [ TAG ] [ * packet payload * ] </tt>

<i>P_DATA_V2 GCM data channel crypto format:</i> 

<tt>   [ - opcode/peer-id - ] [ - packet ID - ] [ TAG ] [ * packet payload * ] </tt>

<b>No-crypto data channel format</b> 

In no-crypto mode (\c \-\-cipher \c none is specified), both TLS-mode and
static key mode are supported. No encryption will be performed on the packet,
but packets can still be authenticated. This mode does not require an IV.

<i>No-crypto data channel crypto format in TLS-mode:</i> 

<tt> [ HMAC ] [ - packet ID - ] [ - packet payload - ] </tt> 

<i>No-crypto data channel crypto format in static key mode:</i> 

<tt> [ HMAC ] [ - packet ID - ] [ - timestamp - ] [ - packet payload - ] </tt>

### 1、接口

> 》：
>
> SHA256 digests
>
> HMAC：哈希运算消息认证码（Hash-based Message Authentication Code）
>
> 





## proto

### 1、接口

> TUN：TUN, namely network TUNnel, simulates a network layer device and operates in layer 3 carrying IP packets.
>
> TAP：TAP, namely network TAP, simulates a link layer device and operates in layer 2 carrying Ethernet frames.

openvpn_ethhdr：

> a regular Ethernet II header：
>
> （，IP和以太网协议结构体。为了兼容性，OpenVPN需要其自己定义这些结构体，并且名字已经被调整避免与原生结构体冲突；）IP and Ethernet protocol structs.  For portability, OpenVPN needs its own definitions of these structs, and names have been adjusted to avoid collisions with native structs.

- dest：6字节大小；（目的以太网地址）destination ethernet addr
- source：6字节大小；（源以太网地址）source ethernet addr
- proto：（包类型ID字段）packet type ID field
  - IPv4 protocol
  - IPv6 protocol
  - ARP protocol
  - 802.1Q protocol

openvpn_8021qhdr：

> an Ethernet II  header with additional IEEE 802.1Q tagging.

- dest：
- source：
- tpid：802.1Q Tag Protocol Identifier
- pcp_cfi_vid：
  - PCP
  - CFI
  - VID
- proto



openvpn_arp：

- mac_addr_type：
- proto_addr_type：
- mac_addr_size：
- proto_addr_size：
- arp_command：0x0001 for ARP request, 0x0002 for ARP reply
- mac_src：6字节的（源IP对应的）MAC源地址
- ip_src：源IP地址
- mac_dest：6字节的（目的IP对应的）MAC目的地址
- ip_dest：目的IP地址



openvpn_iphdr：IPv4头部

- version_len：
- tos：
- tot_len：
- id：
- frag_off：
- ttl：
- protocol：
  - IGMP协议
  - TCP协议
  - UDP协议
  - ICMPv6协议
- check：
- saddr：IPv4源地址
- daddr：IPv6源地址
- // （可选项起始地址）The options start here.
  - 注：应该是变长内存申请

openvpn_ipv6hdr：IPv6 header

- version_prio：
- flow_lbl：
- payload_len：
- nexthdr：
- hop_limit：
- saddr：IPv6源地址
- daddr：IPv6目的地址



openvpn_icmp6hdr：ICMPv6 header

- icmp6_type：
- icmp6_code：
- icmp6_cksum：
- icmp6_dataun[4]：



openvpn_udphdr：UDP header

- source：源端口号
- dest：目的端口号
- len：消息长度
- check：



openvpn_tcphdr：TCP header, per RFC 793.

- source：source port
- dest：destination port
- seq：sequence number
- ack_seq：acknowledgement number
- doff_res：
- flags：1个字节的标志位，共8种标志：FIN、SYN、RST、PSH、ACK、URG、ECE、CWR
- window：
- check：
- urg_ptr：



ip_tcp_udp_hdr：

- ip：ipv4
- u：联合体
  - tcp：
  - udp









## dco

 Data Channel Offload (DCO)







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


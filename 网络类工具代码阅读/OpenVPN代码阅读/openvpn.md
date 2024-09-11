

Doxgen格式注释：

> From the doxygen [documentation](http://www.doxygen.nl/manual/commands.html#cmdlink), `\link` is used to refer to objects like a file, class or member and takes a reference to one of these as its first argument. For example, if I wanted to refer to a class method `func` in the class `myClass`, I would use
>
> ```
> \link myClass::func link text ... \endlink
> ```





## openvpn

### 1、接口

【structure，context】：

> Contains all state information for one tunnel.
>
> This structure represents one VPN tunnel. It is used to store state information related to a VPN tunnel, but also includes process-wide data, such as configuration options.
>
> The [Structure of VPN tunnel state storage](https://build.openvpn.net/doxygen/tunnel_state.html) related page describes how this structure is used in client-mode and server-mode.



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


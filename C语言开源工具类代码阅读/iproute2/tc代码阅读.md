



学习目的：分布式环境下，进行测试时，需要模拟这些场景。



普通情况：

网口故障、网口恢复

```
ifconfig eth0 down
ifconfig eth0 up
```



tc：

- 网络丢包  
- 网络时延
  - 故障：`tc qdisc add dev eth0 root netem delay 1500ms`
  - 恢复：` sudo tc qdisc del dev eth0 root netem delay 1500ms`
- 波动时延  
- 循环注入时延
- 网络错包 
- 网络乱序 
- 带宽限制 






bpf_attr：联合体（可以表示其中一个的匿名结构体）

- 匿名结构体1： anonymous struct used by `BPF_MAP_CREATE` command
  - map_type：one of enum `bpf_map_type`
  - key_size：size of key in bytes
  - value_size：size of value in bytes
  - max_entries：max number of entries in a map
  - map_flags：`BPF_MAP_CREATE` related flags defined above. 
  - inner_map_fd：fd pointing to the inner map
  - numa_node：numa node (effective only if `BPF_F_NUMA_NODE` is set). 
  - map_name[16U（宏：`BPF_OBJ_NAME_LEN`）]： map名字的字符串
  - map_ifindex：ifindex of netdev to create on
  - btf_fd：fd pointing to a BTF type data 
  - btf_key_type_id：BTF type_id of the key
  - btf_value_type_id：BTF type_id of the value
  - btf_vmlinux_value_type_id：BTF type_id of a kernel-struct stored as the map value 
  - map_extra：Any per-map-type extra fields 
    - `BPF_MAP_TYPE_BLOOM_FILTER` - （，最低4比特位表示哈希函数的数量（如果是0，布隆过滤器默认使用5个哈希函数；））the lowest 4 bits indicate the number of hash functions (if 0, the bloom filter will default to using 5 hash functions).
- 匿名结构体2： anonymous struct used by `BPF_MAP_*_ELEM` commands
  - 【。。。】

- 【。。。】




问题1

```
 pthread_creat: Processor affinity.  Like scheduling policy, this could fail if the user  does not have the necessary privileges.  So we only spew a warning if  pthread_create fails with EPERM.  A computer has at least one CPU.
 
 How can I check the linux system have the necessary privileges? how to debug it when return EPERM?
```

问题2：在root用户下运行程序 `$ ./a.out`，与 sudo运行程序 `$sudo ./a.out` 有什么差别？二者是否等价？



### 问题记录

####  1、【OK】pthread_create 返回EPERM

- https://stackoverflow.com/questions/9313428/getting-eperm-when-calling-pthread-create-for-sched-fifo-thread-as-root-on-lin

```
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
priority != -1:
pthread_attr_getinheritsched
  PTHREAD_INHERIT_SCHED == get
    get = PTHREAD_EXPLICIT_SCHED
 pthread_attr_setinheritsched get
 pthread_attr_setschedpolicy  SCHED_RR

    max_pri = sched_get_priority_max(policy);
    min_pri = sched_get_priority_min(policy);
 struct sched_param param;
   param.sched_priority = xxx
 pthread_attr_setschedparam

 pthread_create: use pre set attr
```

> 类似问题：pthread_create priority thread returns EPERM when run as root
>
> - https://github.com/coreos/bugs/issues/410
>
> 1、`sysctl -w kernel.sched_rt_runtime_us=-1`
>
> 2、cgroup的问题
>
> you need to move your shell into the default cpu:/ group which does permit **priority scheduling**.
>
> - `cgclassify -g cpu:/ $$`
> - `echo $$ > /sys/fs/cgroup/cpu/tasks`
> - `cat /proc/$$/cgroup`
>

失败节点：

```
[user@user]# cat /proc/$$/cgroup
12:memory:/system.slice/sshd.service
11:devices:/system.slice/sshd.service
10:rdma:/
9:freezer:/system.slice/sshd.service
8:perf_event:/
7:pids:/system.slice/sshd.service
6:cpu,cpuacct:/system.slice/sshd.service
5:blkio:/system.slice/sshd.service
4:cpuset:/
3:hugetlb:/
2:net_cls,net_prio:/
1:name=systemd:/system.slice/sshd.service
```

解决：发现仅仅设置cpu的cgroups不够，依然存在cgroup的问题。运行如下命令才最终解决。

```
echo $$ > /sys/fs/cgroup/cpu/tasks
echo $$ > /sys/fs/cgroup/cpuacct/tasks
echo $$ > /sys/fs/cgroup/cpu,cpuacct/tasks
echo $$ > /sys/fs/cgroup/cpuset/tasks
```


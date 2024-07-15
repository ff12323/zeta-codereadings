https://github.com/axboe/fio

```
tiresome：Something that's tiresome is extremely dull or annoying, like the tiresome lecture your whole class gets every time some forgets to turn in their homework.

tailored ：(of clothing) custom-made
```



（， 。。。 测试一个指定的负载；）Fio was originally written to save me the hassle of writing special test case programs when I wanted to test a specific workload, either for performance reasons or to find/reproduce a bug. The process of writing such a test app can be tiresome, especially if you have to do it often. Hence I needed a tool that would be able to simulate a given I/O workload without resorting to writing a tailored test case again and again.

（，一个测试工作负载是很难去定义的；。。。）A test work load is difficult to define, though. There can be any number of processes or threads involved, and they can each be using their own way of generating I/O. You could have someone dirtying large amounts of memory in a memory mapped file, or maybe several threads issuing reads using asynchronous I/O. fio needed to be flexible enough to simulate both of these cases, and many more.

Fio spawns a number of threads or processes doing a particular type of I/O action as specified by the user. fio takes a number of global parameters, each inherited by the thread unless otherwise parameters given to them overriding that setting is given. The typical use of fio is to write a job file matching the I/O load one wants to simulate.

`fio.c`

- 入口的main函数：

```c
/*
 * fio - the flexible io tester
 *
 * Copyright (C) 2005 Jens Axboe <axboe@suse.de>
 * Copyright (C) 2006-2012 Jens Axboe <axboe@kernel.dk>
 *
 * The license below covers all files distributed with fio unless otherwise
 * noted in the file itself.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#include "fio.h"

int main(int argc, char *argv[], char *envp[])
{
	int ret = 1;

	compiletime_assert(TD_NR <= TD_ENG_FLAG_SHIFT, "TD_ENG_FLAG_SHIFT");

	if (initialize_fio(envp))
		return 1;

#if !defined(CONFIG_GETTIMEOFDAY) && !defined(CONFIG_CLOCK_GETTIME)
#error "No available clock source!"
#endif

	if (fio_server_create_sk_key())
		goto done;

	if (parse_options(argc, argv))
		goto done_key;

	/*
	 * line buffer stdout to avoid output lines from multiple
	 * threads getting mixed
	 */
	setvbuf(stdout, NULL, _IOLBF, 0);

	fio_time_init();

	if (nr_clients) {
		set_genesis_time();

		if (fio_start_all_clients())
			goto done_key;
		ret = fio_handle_clients(&fio_client_ops);
	} else
		ret = fio_backend(NULL);

done_key:
	fio_server_destroy_sk_key();
done:
	deinitialize_fio();
	return ret;
}
```





`diskutil.h`：

从sys目录下，读取到的磁盘使用率：

> ```
> $ cat /sys/block/sda/stat 
>  1099600      178 121034518   485912  7244222  6310539 304593064 17910272        0 13900664 18390740
> ```
>
> 

```c
/*
 * Disk utilization as read from /sys/block/<dev>/stat
 */
struct disk_util_stat {
	uint8_t name[FIO_DU_NAME_SZ];
	struct disk_util_stats s;
};
```











**使用记录**



```
$: cat tmp_test.fio
; -- start job file including.fio --
[global]
filename=/tmp/test
filesize=1m
include glob-include.fio

[test]
rw=randread
bs=4k
time_based=1
runtime=10
include test-include.fio
; -- end job file including.fio -

user@user-vm:~/github/fio-master$ ./fio tmp_test.fio
fio: unable to open 'glob-include.fio' job file
Error 1 while parsing include file glob-include.fio
fio: unable to open 'test-include.fio' job file
Error 1 while parsing include file test-include.fio
test: (g=0): rw=randread, bs=(R) 4096B-4096B, (W) 4096B-4096B, (T) 4096B-4096B, ioengine=psync, iodepth=1
fio-3.35
Starting 1 process
test: Laying out IO file (1 file / 1MiB)
Jobs: 1 (f=1): [r(1)][100.0%][r=45.4MiB/s][r=11.6k IOPS][eta 00m:00s]
test: (groupid=0, jobs=1): err= 0: pid=5629: Sun Aug  6 16:42:46 2023
  read: IOPS=11.7k, BW=45.6MiB/s (47.8MB/s)(456MiB/10001msec)
    clat (usec): min=56, max=2215, avg=83.88, stdev=22.42
     lat (usec): min=56, max=2215, avg=84.02, stdev=22.44
    clat percentiles (usec):
     |  1.00th=[   59],  5.00th=[   76], 10.00th=[   78], 20.00th=[   80],
     | 30.00th=[   81], 40.00th=[   81], 50.00th=[   81], 60.00th=[   82],
     | 70.00th=[   83], 80.00th=[   84], 90.00th=[   91], 95.00th=[  106],
     | 99.00th=[  153], 99.50th=[  192], 99.90th=[  347], 99.95th=[  429],
     | 99.99th=[  758]
   bw (  KiB/s): min=43592, max=50016, per=100.00%, avg=46748.63, stdev=1616.28, samples=19
   iops        : min=10898, max=12504, avg=11687.16, stdev=404.07, samples=19
  lat (usec)   : 100=93.88%, 250=5.91%, 500=0.18%, 750=0.02%, 1000=0.01%
  lat (msec)   : 2=0.01%, 4=0.01%
  cpu          : usr=0.11%, sys=49.51%, ctx=116786, majf=0, minf=10
  IO depths    : 1=100.0%, 2=0.0%, 4=0.0%, 8=0.0%, 16=0.0%, 32=0.0%, >=64=0.0%
     submit    : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     complete  : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     issued rwts: total=116758,0,0,0 short=0,0,0,0 dropped=0,0,0,0
     latency   : target=0, window=0, percentile=100.00%, depth=1

Run status group 0 (all jobs):
   READ: bw=45.6MiB/s (47.8MB/s), 45.6MiB/s-45.6MiB/s (47.8MB/s-47.8MB/s), io=456MiB (478MB), run=10001-10001msec

Disk stats (read/write):
  sda: ios=115431/9, sectors=923448/80, merge=0/1, ticks=8085/2, in_queue=8087, util=99.10%

```

将配置的块大小修改为8K：带宽提升了接近一倍。

```
test: Laying out IO file (1 file / 50MiB)
Jobs: 1 (f=1): [r(1)][100.0%][r=83.4MiB/s][r=10.7k IOPS][eta 00m:00s]
test: (groupid=0, jobs=1): err= 0: pid=5675: Sun Aug  6 16:47:20 2023
  read: IOPS=10.7k, BW=83.9MiB/s (88.0MB/s)(839MiB/10001msec)
    clat (usec): min=61, max=2770, avg=90.94, stdev=16.69
     lat (usec): min=61, max=2771, avg=91.10, stdev=16.70
    clat percentiles (usec):
     |  1.00th=[   81],  5.00th=[   84], 10.00th=[   85], 20.00th=[   86],
     | 30.00th=[   87], 40.00th=[   88], 50.00th=[   88], 60.00th=[   89],
     | 70.00th=[   91], 80.00th=[   94], 90.00th=[   98], 95.00th=[  105],
     | 99.00th=[  137], 99.50th=[  165], 99.90th=[  289], 99.95th=[  367],
     | 99.99th=[  486]
   bw (  KiB/s): min=84848, max=86992, per=100.00%, avg=86038.84, stdev=557.68, samples=19
   iops        : min=10606, max=10874, avg=10754.84, stdev=69.70, samples=19
  lat (usec)   : 100=92.18%, 250=7.67%, 500=0.14%, 750=0.01%
  lat (msec)   : 4=0.01%
  cpu          : usr=0.06%, sys=53.30%, ctx=107366, majf=0, minf=11
  IO depths    : 1=100.0%, 2=0.0%, 4=0.0%, 8=0.0%, 16=0.0%, 32=0.0%, >=64=0.0%
     submit    : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     complete  : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     issued rwts: total=107392,0,0,0 short=0,0,0,0 dropped=0,0,0,0
     latency   : target=0, window=0, percentile=100.00%, depth=1

Run status group 0 (all jobs):
   READ: bw=83.9MiB/s (88.0MB/s), 83.9MiB/s-83.9MiB/s (88.0MB/s-88.0MB/s), io=839MiB (880MB), run=10001-10001msec

Disk stats (read/write):
  sda: ios=104982/10, sectors=1679712/88, merge=0/1, ticks=8141/4, in_queue=8145, util=99.09%

```









# 附录A：文档



## 1.9. Running fio

（，运行fio是最简单的部分--你只要**给出job文件当做参数**就可以了）Running fio is normally the easiest part - you just give it the job file (or job files) as parameters:

```
$ fio [options] [jobfile] ...
```

（，多个job文件：顺序地运行这些文件；）and it will start doing what the *jobfile* tells it to do. You can give more than one job file on the command line, <font color="#F9DDA2">fio will serialize the running of those files.</font> Internally that is the same as using the [`stonewall`](https://fio.readthedocs.io/en/latest/fio_doc.html#cmdoption-arg-stonewall) parameter described in the parameter section.

If the job file contains only one job, you may as well just give the parameters on the command line. The command line parameters are identical to the job parameters, with a few extra that control global parameters. For example, for the job file parameter [`iodepth=2`](https://fio.readthedocs.io/en/latest/fio_doc.html#cmdoption-arg-iodepth), the mirror command line option would be [`--iodepth 2`](https://fio.readthedocs.io/en/latest/fio_doc.html#cmdoption-arg-iodepth) or [`--iodepth=2`](https://fio.readthedocs.io/en/latest/fio_doc.html#cmdoption-arg-iodepth). You can also use the command line for giving more than one job entry. For each [`--name`](https://fio.readthedocs.io/en/latest/fio_doc.html#cmdoption-arg-name) option that fio sees, it will start a new job with that name. Command line entries following a [`--name`](https://fio.readthedocs.io/en/latest/fio_doc.html#cmdoption-arg-name) entry will apply to that job, until there are no more entries or a new [`--name`](https://fio.readthedocs.io/en/latest/fio_doc.html#cmdoption-arg-name) entry is seen. This is similar to the job file options, where each option applies to the current job until a new [] job entry is seen.

fio does not need to run as root, except if the files or devices specified in the job section requires that. Some other options may also be restricted, such as memory locking, I/O scheduler switching, and decreasing the nice value.

（，从标准输入中读取当做job文件；）If *jobfile* is specified as `-`, the job file will be read from standard input.

## 1.10. How fio works

（，第一步使得fio取模拟一个期望的I/O负载；）The first step in getting fio to simulate a desired I/O workload, is writing a job file describing that specific setup. A job file may contain any number of threads and/or files – the typical contents of the job file is a *global* section defining shared parameters, and one or more job sections describing the jobs involved. When run, fio parses this file and sets everything up as described. If we break down a job from top to bottom, it contains the following basic parameters:

[I/O type](https://fio.readthedocs.io/en/latest/fio_doc.html#i-o-type)

> （，当对文件发出I/O时需要定义一种IO模式； 顺序还是随机？读写是否混合？缓存IO还是直接IO？）Defines the I/O pattern issued to the file(s). We may only be reading sequentially from this file(s), or we may be writing randomly. Or even mixing reads and writes, sequentially or randomly. Should we be doing buffered I/O, or direct/raw I/O?

===

> 参考：https://lwn.net/Articles/806980/
>
> （，缓存IO：通过内存的页缓存；其相对容易使用并可以在数据被访问多次时产生巨大性能收益； 直接IO：在用户空间和存储设备之间直接进行；）Linux offers two modes for file I/O: buffered and direct. Buffered I/O passes through the kernel's page cache; it is relatively easy to use and can yield significant performance benefits for data that is accessed multiple times. Direct I/O, instead, goes straight between a user-space buffer and the storage device.

[Block size](https://fio.readthedocs.io/en/latest/fio_doc.html#block-size)

> （，当发出I/O的时候是多大的一个块？这个可能是一个单个值，也可以是一个范围大小值）In how large chunks are we issuing I/O? This may be a single value, or it may describe a range of block sizes.

[I/O size](https://fio.readthedocs.io/en/latest/fio_doc.html#i-o-size)

> （，多大的数据将会被读取或写入）How much data are we going to be reading/writing.

[I/O engine](https://fio.readthedocs.io/en/latest/fio_doc.html#i-o-engine)

> How do we issue I/O? We could be memory mapping the file, we could be using regular read/write, we could be using splice, async I/O, or even SG (SCSI generic sg).

[I/O depth](https://fio.readthedocs.io/en/latest/fio_doc.html#i-o-depth)

> （，如果是异步的，则多大的一个队列深度需要进行维护？）If the I/O engine is async, how large a queuing depth do we want to maintain?

[Target file/device](https://fio.readthedocs.io/en/latest/fio_doc.html#target-file-device)

> How many files are we spreading the workload over.

[Threads, processes and job synchronization](https://fio.readthedocs.io/en/latest/fio_doc.html#threads-processes-and-job-synchronization)

> （，如何分布工作负载？。。。）How many threads or processes should we spread this workload over.

The above are the basic parameters defined for a workload, in addition there’s a multitude of parameters that modify other aspects of how this job behaves.

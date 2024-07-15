





项目构建

### 1、安装依赖

运行：`./configure`

- 问题：缺少依赖的包。
- ftp://gcc.gnu.org/pub/gcc/infrastructure/
- http://gcc.gnu.org/install/prerequisites.html

```
checking for the correct version of gmp.h... no
configure: error: Building GCC requires GMP 4.2+, MPFR 2.4.0+ and MPC 0.8.0+.
Try the --with-gmp, --with-mpfr and/or --with-mpc options to specify
their locations.  Source code for these libraries can be found at
ftp://gcc.gnu.org/pub/gcc/infrastructure/.  See also
http://gcc.gnu.org/install/prerequisites.html for additional info.  If
you obtained GMP, MPFR and/or MPC from a vendor distribution package,
make sure that you have installed both the libraries and the header
files.  They may be located in separate packages.

```

> 记录：
>
> 1、
>
> ```
> wget http://gcc.gnu.org/pub/gcc/infrastructure/gmp-6.1.0.tar.bz2
> wget http://gcc.gnu.org/pub/gcc/infrastructure/mpfr-3.1.4.tar.bz2
> wget http://gcc.gnu.org/pub/gcc/infrastructure/mpc-1.0.3.tar.gz
> wget http://gcc.gnu.org/pub/gcc/infrastructure/isl-0.16.1.tar.bz2
> ```
>
> 2、StackOverflow
>
> - https://stackoverflow.com/questions/9253695/building-gcc-requires-gmp-4-2-mpfr-2-3-1-and-mpc-0-8-0
>
> Inside the gcc directory, do this command:
>
> ```c
> ./contrib/download_prerequisites
> ```
>
> After that script, GMP, MPFR, and MPC will be ready to use. Continue with `./configure`.

文档：`gcc-7.3.0/INSTALL/prerequisites.html`

Several support libraries are necessary to build GCC, some are required, others optional. While any sufficiently new version of required tools usually work, library requirements are generally stricter. Newer versions may work in some cases, but it's safer to use the exact versions documented. We appreciate bug reports about problems with newer versions, though. If your OS vendor provides packages for the support libraries then using those packages may be the simplest way to install the libraries.

- GNU Multiple Precision Library (GMP) version 4.3.2 (or later)

  Necessary to build GCC. If a GMP source distribution is found in a subdirectory of your GCC sources named gmp, it will be built together with GCC. Alternatively, if GMP is already installed but it is not in your library search path, you will have to configure with the --with-gmp configure option. See also --with-gmp-lib and --with-gmp-include. The in-tree build is only supported with the GMP version that download_prerequisites installs. 

- MPFR Library version 2.4.2 (or later)

  Necessary to build GCC. It can be downloaded from http://www.mpfr.org/. If an MPFR source distribution is found in a subdirectory of your GCC sources named mpfr, it will be built together with GCC. Alternatively, if MPFR is already installed but it is not in your default library search path, the --with-mpfr configure option should be used. See also --with-mpfr-lib and --with-mpfr-include. The in-tree build is only supported with the MPFR version that download_prerequisites installs. 

- MPC Library version 0.8.1 (or later)

  Necessary to build GCC. It can be downloaded from http://www.multiprecision.org/. If an MPC source distribution is found in a subdirectory of your GCC sources named mpc, it will be built together with GCC. Alternatively, if MPC is already installed but it is not in your default library search path, the --with-mpc configure option should be used. See also --with-mpc-lib and --with-mpc-include. The in-tree build is only supported with the MPC version that download_prerequisites installs. 

- isl Library version 0.15 or later.

  Necessary to build GCC with the Graphite loop optimizations. It can be downloaded from [ftp://gcc.gnu.org/pub/gcc/infrastructure/](ftp://gcc.gnu.org/pub/gcc/infrastructure/). If an isl source distribution is found in a subdirectory of your GCC sources named isl, it will be built together with GCC. Alternatively, the --with-isl configure option should be used if isl is not installed in your default library search path.



文档：`gcc/doc/install.texi`

```
@item MPFR Library version 2.4.2 (or later)

Necessary to build GCC@.  It can be downloaded from
@uref{http://www.mpfr.org/}.  If an MPFR source distribution is found
in a subdirectory of your GCC sources named @file{mpfr}, it will be
built together with GCC.  Alternatively, if MPFR is already installed
but it is not in your default library search path, the
@option{--with-mpfr} configure option should be used.  See also
@option{--with-mpfr-lib} and @option{--with-mpfr-include}.
The in-tree build is only supported with the MPFR version that
download_prerequisites installs.

@item MPC Library version 0.8.1 (or later)

Necessary to build GCC@.  It can be downloaded from
@uref{http://www.multiprecision.org/}.  If an MPC source distribution
is found in a subdirectory of your GCC sources named @file{mpc}, it
will be built together with GCC.  Alternatively, if MPC is already
installed but it is not in your default library search path, the
@option{--with-mpc} configure option should be used.  See also
@option{--with-mpc-lib} and @option{--with-mpc-include}.
The in-tree build is only supported with the MPC version that
download_prerequisites installs.
```



### 2、编译报错（gcc 7.3 +  GLIBC 2.35-0ubuntu3 + 编译：gcc11）

#### 1、xg++: internal compiler error: Killed (program cc1plus)

- 基本原因：使用 `make -j32`，但是系统资源不足。导致内部失败。
- 根本原因：【TODO】，需要OS与底层知识。

```
e  -o cp/cp-gimplify.o -MT cp/cp-gimplify.o -MMD -MP -MF cp/.deps/cp-gimplify.TPo ../.././gcc/cp/cp-gimplify.c
xg++: internal compiler error: Killed (program cc1plus)
Please submit a full bug report,
with preprocessed source if appropriate.
See <https://gcc.gnu.org/bugs/> for instructions.
make[3]: *** [Makefile:2217: i386.o] Error 4
make[3]: *** Waiting for unfinished jobs....
make[3]: Leaving directory '/home/user/github/gcc-7.3.0/host-x86_64-pc-linux-gnu/gcc'
make[2]: *** [Makefile:4637: all-stage3-gcc] Error 2
make[2]: Leaving directory '/home/user/github/gcc-7.3.0'
make[1]: *** [Makefile:26372: stage3-bubble] Error 2
make[1]: Leaving directory '/home/user/github/gcc-7.3.0'
make: *** [Makefile:942: all] Error 2

```

> StackOverflow：
>
> - https://stackoverflow.com/questions/30887143/make-j-8-g-internal-compiler-error-killed-program-cc1plus
>
> Most likely that is your problem. Running make -j 8 runs lots of process which use more memory. <font color="#F9DDA2">The problem above occurs when your system runs out of memory. In this case rather than the whole system falling over, the operating systems runs a process to score each process on the system</font>. The one that scores the highest gets killed by the operating system to free up memory. If the process that is killed is cc1plus, gcc (perhaps incorrectly) interprets this as the process crashing and hence assumes that it must be a compiler bug. But it isn't really, the problem is the OS killed cc1plus, rather than it crashed.
>
> If this is the case, you are running out of memory. So run perhaps make -j 4 instead. This will mean fewer parallel jobs and will mean the compilation will take longer but hopefully will not exhaust your system memory.



#### 2、configure-target-libsanitizer] Error 1

```sh
libtool: compile:  /home/user/github/gcc-7.3.0/host-x86_64-pc-linux-gnu/gcc/xgcc -B/home/user/github/gcc-7.3.0/host-x86_64-pc-linux-gnu/gcc/ -B/usr/local/x86_64-pc-linux-gnu/bin/ -B/usr/local/x86_64-pc-linux-gnu/lib/ -isystem /usr/local/x86_64-pc-linux-gnu/include -isystem /usr/local/x86_64-pc-linux-gnu/sys-include -DHAVE_CONFIG_H -I../.././libatomic/config/x86 -I../.././libatomic/config/posix -I../.././libatomic -I. -Wall -Werror -pthread -g -O2 -MT exch_8_.lo -MD -MP -MF .deps/exch_8_.lo.Ppo -DN=8 -c ../.././libatomic/exch_n.c -o exch_8_.o >/dev/null 2>&1
/bin/bash ./libtool --tag=CC   --mode=compile /home/user/github/gcc-7.3.0/host-x86_64-pc-linux-gnu/gcc/xgcc -B/home/user/github/gcc-7.3.0/host-x86_64-pc-linux-gnu/gcc/ -B/usr/local/x86_64-pc-linux-gnu/bin/ -B/usr/local/x86_64-pc-linux-gnu/lib/ -isystem /usr/local/x86_64-pc-linux-gnu/include -isystem /usr/local/x86_64-pc-linux-gnu/sys-include    -DHAVE_CONFIG_H -I../.././libatomic/config/x86 -I../.././libatomic/config/posix -I../.././libatomic -I.    -Wall -Werror  -pthread -g -O2 -MT fadd_8_.lo -MD -MP -MF .deps/fadd_8_.lo.Ppo -DN=8  -c -o fadd_8_.lo ../.././libatomic/fadd_n.c
libtool: compile:  /home/user/github/gcc-7.3.0/host-x86_64-pc-linux-gnu/gcc/xgcc -B/home/user/github/gcc-7.3.0/host-x86_64-pc-linux-gnu/gcc/ -B/usr/local/x86_64-pc-linux-gnu/bin/ -B/usr/local/x86_64-pc-linux-gnu/lib/ -isystem /usr/local/x86_64-pc-linux-gnu/include -isystem /usr/local/x86_64-pc-linux-gnu/sys-include -DHAVE_CONFIG_H -I../.././libatomic/config/x86 -I../.././libatomic/config/posix -I../.././libatomic -I. -Wall -Werror -pthread -g -O2 -MT fadd_8_.lo -MD -MP -MF .deps/fadd_8_.lo.Ppo -DN=8 -c ../.././libatomic/fadd_n.c  -fPIC -DPIC -o .libs/fadd_8_.o
yes
libtool: compile:  /home/user/github/gcc-7.3.0/host-x86_64-pc-linux-gnu/gcc/xgcc -B/home/user/github/gcc-7.3.0/host-x86_64-pc-linux-gnu/gcc/ -B/usr/local/x86_64-pc-linux-gnu/bin/ -B/usr/local/x86_64-pc-linux-gnu/lib/ -isystem /usr/local/x86_64-pc-linux-gnu/include -isystem /usr/local/x86_64-pc-linux-gnu/sys-include -DHAVE_CONFIG_H -I../.././libatomic/config/x86 -I../.././libatomic/config/posix -I../.././libatomic -I. -Wall -Werror -pthread -g -O2 -MT fadd_8_.lo -MD -MP -MF .deps/fadd_8_.lo.Ppo -DN=8 -c ../.././libatomic/fadd_n.c -o fadd_8_.o >/dev/null 2>&1


checking for a thread-safe mkdir -p... /usr/bin/mkdir -p
checking for gawk... gawk
checking whether make sets $(MAKE)... yes
checking for style of include used by make... GNU

configure: error: source directory already configured; run "make distclean" there first

/bin/bash ./libtool --tag=CC   --mode=compile /home/user/github/gcc-7.3.0/host-x86_64-pc-linux-gnu/gcc/xgcc -B/home/user/github/gcc-7.3.0/host-x86_64-pc-linux-gnu/gcc/ -B/usr/local/x86_64-pc-linux-gnu/bin/ -B/usr/local/x86_64-pc-linux-gnu/lib/ -isystem /usr/local/x86_64-pc-linux-gnu/include -isystem /usr/local/x86_64-pc-linux-gnu/sys-include    -DHAVE_CONFIG_H -I../.././libatomic/config/x86 -I../.././libatomic/config/posix -I../.././libatomic -I.    -Wall -Werror  -pthread -g -O2 -MT fsub_8_.lo -MD -MP -MF .deps/fsub_8_.lo.Ppo -DN=8  -c -o fsub_8_.lo ../.././libatomic/fsub_n.c

make[1]: *** [Makefile:19091: configure-target-libsanitizer] Error 1
make[1]: *** Waiting for unfinished jobs....
```



#### 3、make clean失败

> ***\*Recursive variable `XXX' references itself (eventually). Stop.\****
>
> *make* 的变量“ *XXX* ”（递归展开式）在替换展开时，引用它自身。无论对于直接展开式变量（通过 *:=* 定义的）或追加定义（ *+=* ），这都是不允许的

```sh
/bin/bash: line 1: test: !=: unary operator expected
rm -f libgcc_tm.h libgcc.map
Makefile:183: *** Recursive variable 'AR' references itself (eventually).  Stop.
make[1]: Leaving directory '/home/user/github/gcc-7.3.0/x86_64-pc-linux-gnu/libgcc'
make: *** [Makefile:21971: clean-stage1-target-libgcc] Error 2
user@user-vm:~/github/gcc-7.3.0$


```

#### 4、（✔）fatal error: sys/ustat.h: No such file

- 参考 ：https://stackoverflow.com/questions/56096060/how-to-fix-the-gcc-compilation-error-sys-ustat-h-no-such-file-or-directory-i
- 根本原因：使用的glibc版本太新，但是编译 gcc7.3需要旧版本的gcc。

```sh
user@user-vm:~/github/gcc2/gcc-7.3.0$ ldd --version
ldd (Ubuntu GLIBC 2.35-0ubuntu3) 2.35
Copyright (C) 2022 Free Software Foundation, Inc.
```

> The release notes for glibc 2.28 say this:
>
> > - The obsolete function `ustat` is no longer available to newly linked binaries; the headers `<ustat.h>` and `<sys/ustat.h>` have been removed. This function has been deprecated in favor of `fstatfs` and `statfs`.
>
> In order to build GCC 5.5, you have a few options:
>
> - Build on an older distribution which uses glibc 2.27 or earlier.
> - Backport [the fix to the Go frontend](https://gcc.gnu.org/viewcvs/gcc?view=revision&revision=261896) to GCC 5. (`ustat` was added to Go only by accident, its deprecation predates the start of Go development, and perhaps even Google as a company.)
> - Disable the gccgo frontend using a suitable `--enable-languages=` argument to the `configure` invocation.

```sh
../.././libcilkrts/runtime/config/x86/cilk-abi-vla.c  -fPIC -DPIC -o .libs/cilk-abi-vla.o
../../.././libsanitizer/sanitizer_common/sanitizer_platform_limits_posix.cc:157:10: fatal error: sys/ustat.h: No such file or directory
 #include <sys/ustat.h>
          ^~~~~~~~~~~~~
compilation terminated.
make[4]: *** [Makefile:523: sanitizer_platform_limits_posix.lo] Error 1
make[4]: Leaving directory '/home/user/github/gcc2/gcc-7.3.0/x86_64-pc-linux-gnu/libsanitizer/sanitizer_common'
make[3]: *** [Makefile:467: all-recursive] Error 1
make[3]: Leaving directory '/home/user/github/gcc2/gcc-7.3.0/x86_64-pc-linux-gnu/libsanitizer'
make[2]: *** [Makefile:337: all] Error 2
make[2]: Leaving directory '/home/user/github/gcc2/gcc-7.3.0/x86_64-pc-linux-gnu/libsanitizer'
make[1]: *** [Makefile:19161: all-target-libsanitizer] Error 2
make[1]: *** Waiting for unfinished jobs....
```

解决办法：

- https://github.com/gcc-mirror/gcc/commit/71b55d45e4304f5e2e98ac30473c581f58fc486b

```
libsanitizer: Use pre-computed size of struct ustat for Linux
Cherry-pick compiler-rt revision 333213:

<sys/ustat.h> has been removed from glibc 2.28 by:

commit cf2478d53ad7071e84c724a986b56fe17f4f4ca7
Author: Adhemerval Zanella <adhemerval.zanella@linaro.org>
Date:   Sun Mar 18 11:28:59 2018 +0800

    Deprecate ustat syscall interface

This patch uses pre-computed size of struct ustat for Linux.

	PR sanitizer/85835
	* sanitizer_common/sanitizer_platform_limits_posix.cc: Don't
	include <sys/ustat.h> for Linux.
	(SIZEOF_STRUCT_USTAT): New.
	(struct_ustat_sz): Use SIZEOF_STRUCT_USTAT for Linux.

From-SVN: r260684
```



#### 5、（✔）宏错误：‘assertion_failed’ is negative

- 根本原因：在对系统的 `ipc_perm`的mode成员，与 `__sanitizer_ipc_perm`的mode成员大小进行。通过编译器数组的，大小比较时。一个是int类型，一个是short类型。
- 解决办法：把libsanitizer的结构体定义，改为当前系统的一样。。。

```sh
In file included from ../../.././libsanitizer/sanitizer_common/sanitizer_platform_limits_posix.cc:191:0:
../../.././libsanitizer/sanitizer_common/sanitizer_internal_defs.h:261:72: error: size of array ‘assertion_failed__1151’ is negative
     typedef char IMPL_PASTE(assertion_failed_##_, line)[2*(int)(pred)-1]
                                                                        ^
../../.././libsanitizer/sanitizer_common/sanitizer_internal_defs.h:255:30: note: in expansion of macro ‘IMPL_COMPILER_ASSERT’
 #define COMPILER_CHECK(pred) IMPL_COMPILER_ASSERT(pred, __LINE__)
                              ^~~~~~~~~~~~~~~~~~~~
../../.././libsanitizer/sanitizer_common/sanitizer_platform_limits_posix.h:1461:3: note: in expansion of macro ‘COMPILER_CHECK’
   COMPILER_CHECK(sizeof(((__sanitizer_##CLASS *) NULL)->MEMBER) == \
   ^~~~~~~~~~~~~~
../../.././libsanitizer/sanitizer_common/sanitizer_platform_limits_posix.cc:1151:1: note: in expansion of macro ‘CHECK_SIZE_AND_OFFSET’
 CHECK_SIZE_AND_OFFSET(ipc_perm, mode);
 ^~~~~~~~~~~~~~~~~~~~~
```

====宏展开与根本原因。

```c
#if !defined(__aarch64__) || !SANITIZER_LINUX || __GLIBC_PREREQ (2, 21)
/* On aarch64 glibc 2.20 and earlier provided incorrect mode field.  */
CHECK_SIZE_AND_OFFSET(ipc_perm, mode);
#endif

//展开为:

#define CHECK_SIZE_AND_OFFSET(CLASS, MEMBER)                       \
  COMPILER_CHECK(sizeof(((__sanitizer_##CLASS *) NULL)->MEMBER) == \
                 sizeof(((CLASS *) NULL)->MEMBER));                \
  COMPILER_CHECK(offsetof(__sanitizer_##CLASS, MEMBER) ==          \
                 offsetof(CLASS, MEMBER))

//展开为:

#define COMPILER_CHECK(pred) IMPL_COMPILER_ASSERT(pred, __LINE__)
#define IMPL_PASTE(a, b) a##b
#define IMPL_COMPILER_ASSERT(pred, line) \
    typedef char IMPL_PASTE(assertion_failed_##_, line)[2*(int)(pred)-1]

typedef char assertion_failed__1151[2 * 
                                    (int)
                                    (
                                        sizeof(((__sanitizer_ipc_perm *)__null)->mode) 
                                     == 
                                        sizeof(((ipc_perm *)__null)->mode)
                                    )
                                    - 1];

typedef char assertion_failed__1151[2 * (int)(__builtin_offsetof(__sanitizer_ipc_perm, mode) == __builtin_offsetof(ipc_perm, mode)) - 1]
    
//=======根本原因：
struct ipc_perm
{
  __key_t __key;				/* Key.  */
  __uid_t uid;					/* Owner's user ID.  */
  __gid_t gid;					/* Owner's group ID.  */
  __uid_t cuid;					/* Creator's user ID.  */
  __gid_t cgid;					/* Creator's group ID.  */
  __mode_t mode;				/* Read/write permission.  */  // typedef unsigned int __mode_t , 4字节
  unsigned short int __seq;			/* Sequence number.  */
  unsigned short int __pad2;
  __syscall_ulong_t __glibc_reserved1;
  __syscall_ulong_t __glibc_reserved2;
};


  struct __sanitizer_ipc_perm {
    int __key;
    int uid;
    int gid;
    int cuid;
    int cgid;
    unsigned short mode;   // 2字节
    unsigned short __pad1;
    unsigned short __seq;
    unsigned short __pad2;
```



### 3、编译-libsanitizer

配置的问题：

- 错误：./config.status: config-ml.in: No such file or directory

```sh
checking for tirpc/rpc/xdr.h... no
configure: creating ./config.status
config.status: creating Makefile
config.status: creating libsanitizer.spec
config.status: creating libbacktrace/backtrace-supported.h
config.status: creating interception/Makefile
./config.status: line 2751: ./../../config-ml.in: No such file or directory
user@user-vm:~/github/gcc-7.3.0/libsanitizer$

```

make失败：

- 错误：No rule to make target 'all'.  Stop

```sh
user@user-vm:~/github/gcc-7.3.0/libsanitizer$ make
cd . && /bin/bash ./config.status config.h
config.status: creating config.h
config.status: config.h is unchanged
make "AR_FLAGS=" "CC_FOR_BUILD=" "CFLAGS=-g -O2" "CXXFLAGS=-g -O2" "CFLAGS_FOR_BUILD=" "CFLAGS_FOR_TARGET=" "INSTALL=/usr/bin/install -c" "INSTALL_DATA=/usr/bin/install -c -m 644" "INSTALL_PROGRAM=/usr/bin/install -c" "INSTALL_SCRIPT=/usr/bin/install -c" "JC1FLAGS=" "LDFLAGS=" "LIBCFLAGS=" "LIBCFLAGS_FOR_TARGET=" "MAKE=make" "MAKEINFO=/bin/bash /home/user/github/gcc-7.3.0/missing --run makeinfo " "PICFLAG=" "PICFLAG_FOR_TARGET=" "SHELL=/bin/bash" "RUNTESTFLAGS=" "exec_prefix=/usr/local" "infodir=/usr/local/share/info" "libdir=/usr/local/lib" "prefix=/usr/local" "includedir=/usr/local/include" "AR=ar" "AS=as" "LD=/usr/bin/ld -m elf_x86_64" "LIBCFLAGS=" "NM=/usr/bin/nm -B" "PICFLAG=" "RANLIB=ranlib" "DESTDIR=" all-recursive
make[1]: Entering directory '/home/user/github/gcc-7.3.0/libsanitizer'
Making all in sanitizer_common
make[2]: Entering directory '/home/user/github/gcc-7.3.0/libsanitizer/sanitizer_common'
make[2]: *** No rule to make target 'all'.  Stop.
make[2]: Leaving directory '/home/user/github/gcc-7.3.0/libsanitizer/sanitizer_common'
make[1]: *** [Makefile:467: all-recursive] Error 1
make[1]: Leaving directory '/home/user/github/gcc-7.3.0/libsanitizer'
make: *** [Makefile:337: all] Error 2

```



# 附录：各种各样

## 一、《GCC Wiki：Installing GCC》

- https://gcc.gnu.org/wiki/InstallingGCC

```
$PWD/../gcc-7.3.0/configure --prefix=$HOME/GCC-7.3.0 --enable-languages=c,c++
```

This page is intended to offer guidance to avoid some common problems when installing GCC, the official installation docs are in the [Installing GCC](http://gcc.gnu.org/install/index.html) section of the main GCC documentation. N.B. those installation docs refer to the development trunk, the installation instructions for released versions are included in the release sources.

For most people the easiest way to install GCC is to install a package made for your operating system. The GCC project does not provide pre-built binaries of GCC, only source code, but all GNU/Linux distributions include packages for GCC. The BSD-based systems include GCC in their ports collections. For other operating systems the [Installing GCC: Binaries](http://gcc.gnu.org/install/binaries.html) page lists some third-party sources of GCC binaries.

If you cannot find suitable binaries for your system, or you need a newer version than is available, you will need to build GCC from source in order to install it.

**Building GCC**

Many people rush into trying to build GCC without reading the [installation docs](http://gcc.gnu.org/install/index.html) properly and make one or more of these common mistakes:

- do not run `./configure` from within the source directory, this is not supported. You need to run configure from outside the source directory, in a separate directory created for the build (this is a [FAQ](https://gcc.gnu.org/wiki/FAQ#configure))
- if GCC links dynamically to the GMP, MPFR or MPC support libraries then the relevant shared libraries must be in the dynamic linker's path, both when building gcc *and* when using the installed compiler (this is also a [FAQ](https://gcc.gnu.org/wiki/FAQ#configure_suffix))

### Support libraries

See [Installing GCC: Prequisites](http://gcc.gnu.org/install/prerequisites.html) for the software required to build GCC. If you do not have the GMP, MPFR and MPC support libraries already installed as part of your operating system then there are two simple ways to proceed, and one difficult, error-prone way. For some reason most people choose the difficult way. The easy ways are:

- If it provides sufficiently recent versions, use your OS package management system to install the support libraries in standard system locations. For Debian-based systems, including Ubuntu, you should install the packages `libgmp-dev`, `libmpfr-dev` and `libmpc-dev`. For RPM-based systems, including Fedora and SUSE, you should install `gmp-devel`, `mpfr-devel` and `libmpc-devel` (or `mpc-devel` on SUSE) packages. The packages will install the libraries and headers in standard system directories so they can be found automatically when building GCC.
- Alternatively, after extracting the GCC source archive, simply run the `./contrib/download_prerequisites` script in the GCC source directory. That will download the support libraries and create symlinks, causing them to be built automatically as part of the GCC build process. Set `GRAPHITE_LOOP_OPT=no` in the script if you want to build GCC without ISL, which is only needed for the optional Graphite loop optimizations.

The difficult way, which is **not recommended**, is to download the sources for GMP, MPFR and MPC, then configure and install each of them in non-standard locations, then configure GCC with `--with-gmp=/some/silly/path/gmp --with-mpfr=/some/silly/path/mpfr --with-mpc=/some/silly/path/mpc`, then be forced to set `LD_LIBRARY_PATH=/some/silly/path/gmp:/some/silly/path/mpfr:/some/silly/path/mpc/lib` in your environment forever. This is silly and causes major problems for anyone who doesn't understand how dynamic linkers find libraries at runtime. Do not do this. If building GCC fails when using any of the `--with-gmp` or `--with-mpfr` or `--with-mpc` options then you probably shouldn't be using them.

### Configuration

See [Installing GCC: Configuration](http://gcc.gnu.org/install/configure.html) for the full documentation. A major benefit of running *srcdir*`/configure` from outside the source directory (instead of running `./configure`) is that the source directory will not be modified in any way, so if your build fails or you want to re-configure and build again, you simply delete everything in the *objdir* and start again.

For example, configuring and building GCC 4.6.2 (with support for C, C++, Fortran and Go) should be as simple as:

```sh
tar xzf gcc-4.6.2.tar.gz
cd gcc-4.6.2
./contrib/download_prerequisites
cd ..
mkdir objdir
cd objdir
$PWD/../gcc-4.6.2/configure --prefix=$HOME/GCC-4.6.2 --enable-languages=c,c++,fortran,go
make
make install
```

The `make` step takes a *long* time. If your computer has multiple processors or cores you can speed it up by building in parallel using `make -j 2` (or a higher number for more parallelism).

If your build fails and your configure command has lots of complicated options you should try removing options and keep it simple. Do not add lots of configure options you don't understand, they might be the reason your build fails.

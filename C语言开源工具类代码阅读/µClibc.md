+项目信息：uClibc 
开始时间：2001年1月  最新版本：2012年5月
地址：https://uclibc.org/



## 第0章 自我记录

第一次&&重要的索引：

- 1.2：实现一个具有兼容性的C语言的库对跨越很多领域的知识都具有一定的要求！（Implementing a compliant C library requires knowledge across many areas）



## 第1章 官方文档

### 1.2 Specifications

<font color="#F9DDA2">Implementing a compliant C library requires knowledge across many areas</font>. Here you can find relevant specifications that'll help you in your quest. If you know of other things that could be useful, please [let us know](https://uclibc.org/lists.html)!

These are standards to be aware of; generally uClibc aims to be compliant with the latest POSIX standard.

- [POSIX 1003.1 2008 (Issue 7)](http://pubs.opengroup.org/onlinepubs/9699919799/)
- [POSIX 1003.1 2004 (Issue 6)](http://pubs.opengroup.org/onlinepubs/009695399/)
- [SystemV ABI](https://uclibc.org/docs/SysV-ABI.pdf)
- [SystemV Interface (volume 1a)](https://uclibc.org/docs/SysV-Interface-vol1a.pdf)
- [SystemV Interface (volume 1b)](https://uclibc.org/docs/SysV-Interface-vol1b.pdf)
- [SystemV Interface (volume 2)](https://uclibc.org/docs/SysV-Interface-vol2.pdf)

These are documents related to the common [ELF](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format) format.

- [ELF (version 1.2)](https://uclibc.org/docs/elf.pdf)
- [ELF 64-bit](https://uclibc.org/docs/elf-64-gen.pdf)
- [ELF Generic ABI](http://www.sco.com/developers/gabi/)
- [Thread Local Storage (TLS)](https://uclibc.org/docs/tls.pdf) (note: also contains alpha ia64 s390 s390x sh x86 x86-64)

These are architecture-specific ELF documents.

- [psABI ARM](https://uclibc.org/docs/psABI-arm.pdf)
- [psABI Blackfin](https://docs.blackfin.uclinux.org/doku.php?id=toolchain:application_binary_interface)
- [psABI Intel386 (x86)](https://uclibc.org/docs/psABI-i386.pdf)
- [psABI Itanium (ia64)](https://uclibc.org/docs/psABI-ia64.pdf)
- [psABI m68k (8bit & 16bit)](https://uclibc.org/docs/psABI-m8-16.pdf)
- [psABI mips](https://uclibc.org/docs/psABI-mips.pdf)
- [psABI PA-RISC (HP/PA)](https://uclibc.org/docs/psABI-parisc.pdf)
- [psABI PowerPC](https://uclibc.org/docs/psABI-ppc.pdf) [TLS](https://uclibc.org/docs/tls-ppc.txt)
- [psABI PowerPC64](https://uclibc.org/docs/psABI-ppc64.pdf) [TLS](https://uclibc.org/docs/tls-ppc64.txt)
- [psABI s/390](https://uclibc.org/docs/psABI-s390.pdf)
- [psABI s/390x](https://uclibc.org/docs/psABI-s390x.pdf)
- [psABI SuperH](https://uclibc.org/docs/psABI-sh.txt)
- [psABI SPARC](https://uclibc.org/docs/psABI-sparc.pdf)
- [psABI AMD64 (x86_64)](https://uclibc.org/docs/psABI-x86_64.pdf)

These are general architecture related reference manuals.

- [Wikibooks: Assembly languages](https://en.wikibooks.org/wiki/Subject:Assembly_languages)
- [Alpha](http://h18002.www1.hp.com/cpq-alphaserver/technology/literature/alphaahb.pdf) [(General)](http://h18002.www1.hp.com/alphaserver/technology/chip-docs.html)
- [ARM](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.set.architecture/index.html)
- [Blackfin](http://www.analog.com/static/imported-files/processor_manuals/Blackfin_pgr_rev2.0.pdf) [(General)](http://www.analog.com/en/products/landing-pages/001/blackfin-manuals.html)
- [PA-RISC (HP/PA)](http://h21007.www2.hp.com/dspp/tech/tech_TechByTypePage_IDX/1,4690,40106-0,00.html)
- [m68k](http://www.freescale.com/files/archives/doc/ref_manual/M68000PRM.pdf)
- [mips](http://www.mips.com/products/product-materials/processor/mips-architecture/)
- [PowerPC](http://www.power.org/resources/downloads/PowerISA_V2.06B_V2_PUBLIC.pdf) [(more)](http://www.ibm.com/developerworks/systems/library/es-archguide-v2.html)
- [s/390](http://lars.nocrew.org/computers/processors/ESA390/dz9zr002.pdf)
- [SPARC](http://www.sparc.org/specificationsDocuments.html)
- x86 [AMD](http://developer.amd.com/documentation/guides/pages/default.aspx) [Intel](http://www.intel.com/products/processor/manuals/)
- [xtensa](http://www.tensilica.com/products/literature-docs/documentation/xtensa-isa-databook.htm)

### 1.3 邮件列表

https://lists.uclibc.org/pipermail/uclibc/






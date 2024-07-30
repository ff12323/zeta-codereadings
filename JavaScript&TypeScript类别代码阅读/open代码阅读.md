

1、问题，无法打开NeatReader

现象：执行powershell命令，无法打开NeatReader软件。需要理解下powershell的原理与查看文档；但是pdf软件可以打开

```javascript

import open, {openApp, apps} from './index.js';

let ret = await open("D:\\math.epub", {
	"app": {
		"name": "C:\\Program Files (x86)\\NeatReader\\NeatReader.exe",
	}}
)

ret = await open("D:\\math.pdf", {
	"app": {
		"name": "C:\\Program Files (x86)\\Foxit Software\\Foxit PDF Reader\\FoxitPDFReader.exe",
	}}
)

```

### vscode-open

在Windows平台上，本质上是通过powershell执行打开的程序。

- 参数通过bese64，utf16le 进行编码；
- 如下解码的内容是 `Start "file:///d:/pdf/设备驱动#/电机控制学习/现代电机控制技术.pdf"`

```
[
  "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell",
  "-NoProfile",
  "-NonInteractive",
  "-ExecutionPolicy",
  "Bypass",
  "-EncodedCommand",
  "UwB0AGEAcgB0ACAAIgBmAGkAbABlADoALwAvAC8AZAA6AC8AcABkAGYALwC+iwdZcZqoUiMALwA1dTpnp2M2UmZbYE4vALBz4041dTpnp2M2UoBiL2cuAHAAZABmACIA",
]
```

> 实质命令：在cmd窗口上我们可以执行如下命令，使用系统默认应用打开对应类型的问题。
>
> `C:/Windows/System32/WindowsPowerShell/v1.0/powershell Start "file:///d:/test.pdf"`
>
> 现在的问题是目录名带上 #号，打开就会报错：
>
> ``C:/Windows/System32/WindowsPowerShell/v1.0/powershell Start "file:///d:/test#/test.pdf"``

问题结论：是自己传入的的路径有问题，不应该使用URL解码后的路径。

- 原因：\#字符在URL（规范）中有特殊的含义，表示URL的结束。因此，当你尝试打开一个包含#字符的文件路径时，PowerShell会将#及其后面的所有字符视为URL的一部分，从而导致找不到文件的错误 ？

使用：`Start "file:///d:/test%23/test.pdf"`

问题扩展：实际上还有更多的URL预留字符，但是操作系统对文件命名是允许的字符。会出现冲突，导致文件打不开。

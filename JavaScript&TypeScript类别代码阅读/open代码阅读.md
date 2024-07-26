

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


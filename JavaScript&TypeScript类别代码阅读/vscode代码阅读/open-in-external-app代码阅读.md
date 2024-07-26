地址：https://github.com/tjx666/open-in-external-app


```json
{
    "openInExternalApp.openMapper": [
        {
          // represent file extension name
          "extensionName": "html",
          // the external applications to open the file which extension name is html
          "apps": [
            // openCommand can be shell command or the complete executable application path
            // title will be shown in the drop list if there are several apps
            {
              "title": "chrome",
              // On MacOS, openCommand should be 'Google Chrome.app'
              "openCommand": "C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe"
            },
            {
              "title": "firefox",
              // On MacOS, openCommand should be 'Firefox Developer Edition.app'
              "openCommand": "C:\\Program Files\\Firefox Developer Edition\\firefox.exe",
              // open in firefox under private mode
              "args": ["-private-window"]
            }
          ]
        },
        {
          "extensionName": "epub",
          // apps can be Object array or just the command you can access from shell
          "apps": [
            {
                "title": "Neat", 
                "openCommand": "C:\\Windows\\System32\\cmd.exe",
                "args": [
                  "/C",
                  "start",
                  "C:\\Program Files (x86)\\NeatReader\\NeatReader.exe",
                  "${file}",
                  "&",
                  ""
                ]
            }
          ]
        },
        {
          "extensionName": "pdf",
          "apps": [
            {
                "title": "foxit",
                "openCommand": "C:\\Program Files\\Foxit Software\\Foxit PDF Reader\\FoxitPDFReader.exe",
                "args": ["${file}"]
            }
          ]
        },
      ]
}
```



问题记录：

- 1、只要路径带有非 ascii 字符就打不开，因为扩展使用的 API vscode.env.openExternal 本身不支持。

vscode接口：

- 读`setting.json`文件的配置项：`vscode.workspace .getConfiguration() .get('key')`
  - `openInExternalApp.openMapper` ：打开外部app的配置映射
  - `openInExternalApp.enableLog`：是否记录日志
- 窗口显示错误信息：`vscode.window.showErrorMessage`
- `vscode.env.remoteName`
- 打开日志文件：`vscode.window.createOutputChannel`
- 将外部文件在vscode打开：`vscode.env.openExternal(vscode.Uri.file('C:/测试.md'));`
  -  https://github.com/microsoft/vscode/issues/88273




## Logger

- 初始化输出：
  - vscode的API，打开：`Open in External App.log` 的日志文件



## 【接口文件】index.d.ts



ExternalAppConfig：打开某个应用的具体配置

- title：标题，字符串
- openCommand：可选项，字符串（打开文件的exe路径）
- args：可选项，字符串数组
- isElectronApp：可选项，标志位之是否打开Electron的App
- shellCommand：可选项，字符串。通过shell命令执行打开；（Linux环境下执行？）

ExtensionConfigItem：

- id：
- extensionName：字符串，或字符串数组；（记录打开文件的后缀名）
- apps：字符串（直接执行）；【接口：具体配置】数组（可以同时配置多个打开的应用方式）



## config

功能：配置项的有效性校验、读取vscode配置文件



getExtensionConfig

- 类型：【export】
- 读插件的vscode配置项：
- 如果有：
  - （func： 校验配置项）
  - 如果有错误：
    - 。。。
    - 返回空数组 
- 否则 返回空数组；



## open

导包：

- `import _open from 'open';`
  - 目的：将依赖的**项目open包**所提供的open接口，改为 `_open` ，尽在内部使用；因为要提供本项目的open接口
- `export const exec = promisify(_exec);`
  - `import { exec as _exec } from 'node:child_process';`
  - 异步化包装node的子进程命令；

openByPkg：通过open包打开文件，打开选项是可选项；

- INFO：open file by open pkg, options:\n （json格式化@打开选项的字符串）
- 返回 _open(@文件路径，@打开选项)

openByBuiltinApi：【async】

- INFO：open file by vscode builtin api
- // https://github.com/microsoft/vscode/issues/88273
- 返回 vscode中打开@文件路径

open：

- 类型：【export】
- INFO：opened file is: xxx
- 如果 @app是字符串：
  - 通过 open包打开文件
- 否则如果： @app不为null 且是 object对象
  - 如果@app是electron类型：
    - 通过vscode内置api打开
  - 如果@app存在shell命令：
    - 解析命令。。。
    - INFO：open file by shell command: （解析后的命令）
    - try：
    - （call：exec 命令）
    - catch：
    - vscode窗口提示错误：`open file by shell command failed, execute: 命令`
    - INFO：记录具体error
  - 如果@app存在打开文件的exe路径：
    - 解析参数。。。
    - 通过 open包打开文件
- 否则如果：是远程WSL环境：
  - 通过 open包打开文件，无可选项
- 否则：（？出现错误的情况）
  - 通过vscode内置api打开

```
Para：
  filePath： 打开文件的路径
  appConfig：可选项，（类型：字符串 或者 （打开某个应用的具体配置））
```



## openInExternalApp



openInExternalApp：

- 类型：【async】、【export】 异步接口函数
- Impl：
- // if run command with command plate, uri is undefined, fallback to activeTextEditor
- 如果 vscode的远程环境是 WSL环境：`vscode.env.remoteName == 'wsl'`
  - await wslToWindows：（？：从WSL环境转到Windows环境吗？）
- when there is configuration map to it's extension, use [open](https://github.com/sindresorhus/open) except for configured appConfig.isElectronApp option

```
Para：
  uri：
  configItemId：
  isMultiple： 是否有多个app配置
    default：false
ret：
  promise
```














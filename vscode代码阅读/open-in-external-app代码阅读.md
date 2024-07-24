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
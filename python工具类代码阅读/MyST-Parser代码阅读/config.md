







MdParserConfig：

（用于MD解析器的配置可选项）Configuration options for the Markdown Parser.

> Note in the sphinx configuration these option names are prepended with ``myst_``

配置属性：

- commonmark_only：X
  - help：Use strict CommonMark parser
- gfm_only：X
  - help：Use strict **Github Flavoured Markdown** parser
- 。。。
- title_to_header：
  - help：Convert a `title` field in the front-matter to a H1 header
- heading_anchors：
  - help：（，？）Heading level depth to assign **HTML anchors**

> Anchor tags in HTML code are HTML elements used to create hyperlinks in webpages.  Example： `<a></a>`

- 。。。
- dmath_allow_labels：true
  - help：Parse `$$...$$ (label)`
- dmath_allow_space：true
  - Allow initial/final spaces in `$ ... $`
- dmath_allow_digits：true
  - Allow initial/final digits `1$ ...$2`
- dmath_double_inline：X
  - help：Parse inline `$$ ... $$`
- update_mathjax：true
  - help：Update sphinx.ext.mathjax configuration to ignore `$` delimiters
- 。。。



merge_file_level：

```
Ds：（，将文件配置更新到全局配置Merge the file-level topmatter with the global config.
Para：
  config：（默认全局配置）Global config.
  topmatter：Topmatter from the file.
  warning：Function to call with a warning (type, message).
returns：
  （更新后的MD解析配置）A new config object
```

read_topmatter：

- Ds：Read the (optional) YAML topmatter from a source string.（This is identified by the first line starting with `---`, then read up to a terminating line of `---`, or `...`.）
- Impl：
  - 格式校验，第一行必须是 `---` 开头 ，否则返回 None；并跳过
  - 遍历剩余行：
    - 如果是  `---`, or `...` 开头，break结束
    - 否则对行进行 右部strip，加入列表
  - 返回 `yaml.safe_load`( 换行符连接的yaml格式文本 )

```
Para：
   source: The source string to read from
     类型：字符串 或 行分割的字符串迭代器
Ret：
   The topmatter： yaml配置字典

```






# CadTools


2026/3/16  
前年将 AutoCAD 用作工作时，我就简单研究了下 ObjectARX，本来想根据工作需要开发工具提升效率的，后面一直搁置了。最近几天又想起来，就开始动工了。  这个插件是为 AutoCAD Mechanical 版本开发的，其它版本的 AutoCAD 可能无法使用。  
![alt text](doc/image1.png)  

可以嵌入 CAD 菜单栏中  
![alt text](doc/image4.png)

## 使用方法

AutoCAD 处于打开图纸状态下，执行`APPLOAD`命令。  
切换到插件文件所在路径下，双击插件文件加载。  
可能提示“安全性 - 未签名的可执行文件”，点击`始终加载`，则下次启动不再提示。  
![alt text](doc/image2.png)  
  
设置随 AutoCAD 自启动：  
点击页面上公文包图标下的`内容`按钮，点击`添加`将插件文件选入，即设为自启动。  
![alt text](doc/image3.png)

## 功能说明

详见[功能说明](doc/FunctionDescription.md)

## 开发

### 测试环境

软件：  
* Visual Studio Community 2026
* AutoCAD Mechanical 2026  
  
SDK：  
* ObjectARX SDK 2026  
* AutoCAD Mechanical SDK 2026  

编译标准：  
* C++23  

ObjectARX 环境配置参考：https://blog.iyatt.com/?p=21187  
AutoCAD Mechanical SDK 环境配置参考：https://blog.iyatt.com/?p=23776  
Visual Studio 2026 配置 Wizards 2026 参考：https://blog.iyatt.com/?p=24263  

### 项目开发策略

【0】紧跟最新版本的原则。  
本插件的定位是个人效率提升工具，当新版本 AutoCAD Mechanical 及 SDK 发布后，我将切换最新版进行测试。由于个人精力有限，不再针对旧版本进行维护。  
【1】本项目深度使用 AI 开发。  
我主要使用 Google Gemini 辅助，前期框架构建时，我个人参与度较高。框架搭建好后，新增功能基本由 AI 编写代码，我提供实现思路和做技术方案决策，生成代码后由我进行功能测试。AI 生成代码存在错误的，我提供 API 文档和给予纠正引导。    
【2】优先使用基于 `wchar_t` 的宽字符及宽字符串。字符串类优先使用 `AcString`，在与 MFC 交互时使用 `CString`。`AcString` 转字符串常量使用 `constPtr()` 方法。字面值常量统一使用 `L` 前缀，如 `L"这是一个字面值"`。  
【3】涉及具体语言的字符串（如中文字符串）全部存储到 String Table 中，命名规则：  
* 标签：IDS_LBL_标签名称
* 命令描述：IDS_CMD_命名全名
* 格式化字符串： _FMT 结尾
* 错误消息：IDS_ERR_错误名称
* 使用提示：IDS_TIP_提示名称
* 警告消息：IDS_WARN_警告名称
* 输出消息：IDS_MSG_消息名称
* 属性值：IDS_VAL_属性名称
* 操作提示：IDS_PROMPT_操作名称
* 标题：IDS_TITLE_标题名称
* 文件过滤器：IDS_FILTER_文件类型名
* 文件名：IDS_FILE_文件名  

【4】代码风格：  
* 由向导程序生成的文件中原本的代码风格不进行修改，仅自行编写的内容控制风格。
* 折行括号。大括号必须独立成行。
* 禁止单行控制流。if、else、、while、for 后的执行部分必须独立成行，必须加大括号。

【5】模块化和扩展名：由向导生成的代码和 MFC 类实现不改模块（对模块的兼容性很差），其它增加的实现采用模块。模块声明使用 `.ixx` 后缀，内联模块使用 `-inline.ixx` 后缀，传统头文件使用 `.hpp` 后缀，创建项目时生成的 `.h` 文件不修改扩展名，源文件统一使用 `.cpp` 后缀。  

### 许可证

本项目采用 [MIT 许可证](LICENSE) 进行许可。  

#### 第三方许可

本项目集成了以下第三方库：
* nlohmann/json (3.12.0) - 采用 MIT 协议。详见其 [GitHub 仓库](https://github.com/nlohmann/json)。
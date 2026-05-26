# CadTools


2026/3/16  
前年将 AutoCAD 用作工作时，我就简单研究了下 ObjectARX，本来想根据工作需要开发工具提升效率的，后面一直搁置了。最近几天又想起来，就开始动工了。  这个插件是为 AutoCAD Mechanical 版本开发的，其它版本的 AutoCAD 可能无法使用。  
![alt text](doc/image1.png)  

可以嵌入 CAD 菜单栏中  
![alt text](doc/image4.png)

---

2026/5/10  
从 AutoCAD Mechanical 2026 转到 AutoCAD Mechanical 2027 开发。  

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
* AutoCAD Mechanical 2027  
  
SDK：  
* ObjectARX SDK 2027  
* AutoCAD Mechanical SDK 2027  

编译标准：  
* C++23  

工具：  
* GNU gettext tools for Windows：https://github.com/vslavik/gettext-tools-windows  
* poedit: https://github.com/vslavik/poedit  

环境配置：  
ObjectARX 环境配置参考：https://blog.iyatt.com/?p=24522  
AutoCAD Mechanical SDK 环境配置参考：https://blog.iyatt.com/?p=24583  
启用 vcpkg 参考：https://blog.iyatt.com/?p=24690  
vcpkg 包搜索：https://vcpkg.io/en/packages  

### 项目开发策略

【0】紧跟最新版本的原则。  
本插件的定位是个人效率提升工具，当新版本 AutoCAD Mechanical 及 SDK 发布后，我将切换最新版进行测试。由于个人精力有限，不再针对旧版本进行维护。  
【1】本项目深度使用 AI 开发。  
【2】本项目中优先使用宽字符串。    
【3】字面值 `_()` 括起来，如 `_(L"hello world")`，便于导出词条和统一翻译。  
【4】代码风格：  
* 由向导程序生成的文件中原本的代码风格不进行修改，仅自行编写的内容控制风格。
* 折行括号。大括号必须独立成行。
* 禁止单行控制流。if、else、、while、for 后的执行部分必须独立成行，必须加大括号。  

【5】模块化和扩展名：由向导生成的代码和 MFC 类实现不改模块（对模块的兼容性很差），其它增加的实现采用模块。模块声明使用 `.ixx` 后缀，内联模块使用 `-inline.ixx` 后缀，传统头文件使用 `.hpp` 后缀，创建项目时生成的 `.h` 文件不修改扩展名，源文件统一使用 `.cpp` 后缀。  

### 许可证

本项目采用 [MIT 许可证](LICENSE) 进行许可。  

#### 第三方许可与用途声明

本项目集成了以下第三方组件，特此说明其用途及许可协议：

| 组件名称 | 用途说明 | 协议类型 | 官方/源码地址 |
| :--- | :--- | :--- | :--- |
| **nlohmann/json** | 用于处理配置文件的 JSON 序列化与解析。 | MIT | [GitHub](https://github.com/nlohmann/json) |
# CadTools

2026/3/16  
前年将 AutoCAD 用作工作时，我就简单研究了下 ObjectARX，本来想根据工作需要开发工具提升效率的，后面一直搁置了。最近几天又想起来，就开始动工了。现在集成了几个简单功能：  
①将选中的实体图层属性全部转为 ByLayer。比如从三维软件投影出来的二维图的图层属性通常不是 ByLayer，这样就和图层设置不一致。这里就实现了一次性批量设置颜色、线型、线宽为 ByLayer，这样就和图层一致了。  
②固化标注和恢复关联标注。标注是关联到实体的，实体尺寸变化，标注文本的名义尺寸会跟随变化。有时候需要对标注的图形进行缩放等处理，会影响标注文本变化，先进行固化设置成当前实体的尺寸，就不会变化了。同样也可以恢复关联标注，让标注文本跟随实体变化，只是手动编辑的内容和公差等内容会丢失，目前还难以从固化的文本中解析出谁是名义尺寸，只能全清空，变成默认的关联实体的仅名义尺寸。  
③在标注的前和后添加文本。设置好前后要添加的文本，可以连续点选快速添加。比如加工中有些设置参考尺寸。  
  
## 使用命令

* yxHelp：查看基本信息
* yxSetByLayer 或 yxSBL：设置选中实体的颜色、线型、线宽为 ByLayer
* yxDimensionFix 或 yxDF：固定标注
* yxDimensionResume 或 yxDR：恢复关联标注
* yxAddSurroundingCharsForDimension 或 yxASCFD：在标注前后添加指定符号（各自指定）
* yxRemoveSurroundingCharsForDimension 或 yxRSCFD：在标注前后移除指定符号（各自指定），可以只移除前或后，不移除的不填。  
![alt text](doc/image.png)
* yxSetBasicBox 或 yxSBB：为标注设置理论尺寸框
* yxUnsetBasicBox 或 yxUBB：为标注取消理论尺寸框
* yxSetRefDim 或 yxSRD：为标注设置参考尺寸括号
* yxUnsetRefDim 或 yxURD：为标注取消参考尺寸括号

## 测试环境

* Visual Studio 2022  
* ObjectARX 2026  
* AutoCAD Mechanical 2026  
  
开发环境配置可以参考我的博客：https://blog.iyatt.com/?p=21187  


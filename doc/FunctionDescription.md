## 前言

### 规范使用增强尺寸标注

涉及尺寸相关的功能请确保标注的尺寸是使用的增强尺寸标注，否则本工具可能无法按预期工作。  
主要针对尺寸公差有要求，不要手填极限偏差的 \S 代码、对称偏差 %%P 代码。  
如下图，偏差上下限都要通过增强尺寸标注填写，然后通过方式选择是极限偏差还是对称偏差。  
![alt text](image-3.png)

## “批量”和“立即”

本工具中涉及选择实体操作的命令，均提供了“批量”和“立即”两种选择模式：  
* “批量”允许一次性选多个实体，并一起进行处理。可以先选中一批实体再执行命令，选择“批量”则会使用已选中的实体进行操作。也允许先执行命令，选择“批量”再去选要处理的实体。  
* “立即”则是选中一个实体处理一个实体，点击即执行。  
![alt text](选择模式.png)  

“批量”模式后还有一个“排序模式”，对于有空间顺序要求的操作会启用。比如提取数据到 CSV 是有顺序需求的，按正常阅读的顺序：从左到右，从上往下。这种顺序的操作称为“右下”，可以理解为“逐行处理，每行先向右，一行处理完，再向下换行”。  
英语中“左”为“left”，“右”为“right”，“上”为“up”，“下”为“down”，分别取首字母用作方向标识。  
共有 8 种顺序和一种无顺序：  
![alt text](排序模式.png)

## yxSetByLayer

将实体的颜色、线型、线宽设置为 ByLayer。  
  
下面的几条直线处于非 ByLayer 状态。  
![alt text](非ByLayer实体.png)  

执行命令操作后，颜色、线型、线宽都改为了 ByLayer。  
![alt text](设置ByLayer后.png)

## yxChainSelection

链式选择，连选。  
执行命令后选择线条，可以将视野范围内与选择线条相连的线条都选中。  
![alt text](image.png)

## yxDimensionSolidify

尺寸固化。  
使用机械版标注的尺寸中，尺寸值是跟随被标注对象动态更新的，如下图中<>括号部分。  
![alt text](image-1.png)  
尺寸固化的作用是按当前尺寸值、尺寸精度将值填入替换<>数值部分，这样尺寸中就不存在动态更新值，而是固定的尺寸文本，标注的尺寸不会跟随实体缩放而更新值。  
如下图固化后，尺寸内容就是纯文本，没有自动的<>字段了。  
![alt text](image-2.png)

## yxDimensionReslink

尺寸恢复关联。  
这个命令的功能是试图恢复被 yxDimensionSolidify 固化的尺寸，重新添加<>字段，以支持动态更新值。当前只能保证尺寸值、公差被恢复，其它手动编辑内容无法恢复。  

## yxDimensionTolerancePrecision

设置尺寸和公差的精度，即小数位数。  
可同时设置或设置其中一种，当值为 -1 时则不修改，值允许范围为 0 到 8。  
![alt text](image-4.png)

## yxAddSurroundingCharsForDimension

给尺寸添加前后缀。  
可以同时添加前后缀或只添加其中一种，支持 AutoCAD 的 GDT 字体，可以通过 Windows 的字符映射表查看 GDT 字体。  
![alt text](image-5.png)  
![alt text](image-6.png)  

GDT 字体实际使用的 ASCII 字符存储，对话框中勾选 `GDT 字体` 后按 GDT 字体显示和插入。  
![alt text](image-7.png)  

批量插入后的效果：  
![alt text](image-8.png)  

## yxRemoveSurroundingCharsForDimension

给尺寸移除前后缀。  
这是 yxAddSurroundingCharsForDimension 命令的逆操作。  

## yxSetBasicBox

设置理论尺寸框。  
![alt text](image-9.png)

## yxUnsetBasicBox

取消理论尺寸框。  

## yxSetRefDim

设置参考尺寸括号。  
![alt text](image-10.png)

## yxUnsetRefDim

取消参考尺寸括号。  

## yxInsertBalloonNumberBlockWithStartNumber

插入指定起始标号的气泡号。  
气泡号的圆半径等于序号文字高度。序号文字高度（实现显示的高度）=TEXTSIZE变量值×注释比例缩放值，比如当前 TEXTSIZE 值为 3.5，然后注释比例为 1:2，即缩放值为 2，那么实际显示的序号文字高度就是 7。  
![alt text](image-11.png)

只是实际设置大小是通过注释性设置的，这个气泡号是注释性的。即以 TEXTSIZE 作为气泡号序号文字的基本高度，再结合当前的注释比例控制大小。  
气泡号是以块的形式创建的，在某一图纸首次插入气泡号的时候，序号文字的基本高度就被定为插入时的 TEXTSIZE 大小了，后面插入时会直接沿用首次插入时创建的块定义，所以后续 TEXTSIZE 值修改也无法修改气泡号的基本高度大小。 只是通过修改注释比例，可以控制实际显示大小。     
![alt text](image-12.png)

## yxUpdateBalloonNumberBlock

更新气泡号。  
对于已经插入的气泡号，可以使用这个命令修改气泡号值。  
比如下面的气泡号，我希望批量按从右到左逐行递增，从最下行往上递增，从 1 开始。  
![alt text](image-13.png)  

批量选择待处理的气泡号后，排序模式选左上（逐行向左，并按行向上）  
![alt text](image-14.png)  

气泡号就被批量修改如下：  
![alt text](image-15.png)  

## yxBalloonNumberOffset

气泡号偏置。  
批量对已插入的气泡号加或减一个数（偏置值），偏置值为负数时执行减操作，如果减后小于 0 则设为 0，即气泡号序号不允许为负数。  
![alt text](image-16.png)  

## yxBalloonNumberFilter

气泡号筛选。  
筛选符号为 2 个，如对话框中的使用提示，支持中文全角和英文半角符号，避免来回切换输入法语言。只有等于和不等于判断允许气泡号的序号为任意字符，即可以不是数字，涉及大于小于判断的必须是数字。  
下面示例是筛选大于 107 的气泡号。  
![alt text](image-17.png)  

符合筛选目标的气泡号会被选中。  
![alt text](image-18.png)

## yxCheckBalloonNumberMaxMin

查找气泡号的最大和最小序号。  
找到的目标会选中，且在底部提示历史中显示。    
![alt text](image-19.png)

## yxCheckDuplicateBalloonNumbers

查找重复的气泡号。  
![alt text](image-20.png)  

## yxCheckBalloonNumberBreakpoints

查找气泡号断点。  
结果显示在提示历史中，这里缺失了 102、106、107
![alt text](image-21.png)

## yxExtractAnnotations

提取标注到 CSV 文件。  
支持提取尺寸、多行文本、单行文本、几何公差。  
![alt text](image-22.png)

## yxCloneText

文本复制。  
支持从多行/单行文本复制到多个多行/单行文本，如果被复制目标存在字段，可以原样复制字段过去。  
![alt text](image-23.png)  

比如这里将左边的含字段的多行文本复制给其它文本。  
![alt text](image-24.png)  

## yxIntersect

将两条直线修剪至交点。  
这是封装的 AutoCAD 的圆角命令，即创建半径为 0 的圆角。这样可以实现将未相交的直线延伸到相交，或者已经相交的直线修剪掉多余的部分。  
![alt text](image-25.png)  

![alt text](image-26.png)  

## yxImportCsvToMTextMatrix

从 CSV 文件导入生成矩阵多行文本。  
这里使用前面导出的标注文件。然后列宽设置为 100，列步长要错开重叠，就加 1 间隔，填 101。现在实际显示行高为 7（TEXTSIZE 为 3.5，注释比例为 1:2，即实际显示高度为 3.5×2=7），行间距也加 1，即行步长填 8。  
![alt text](image-27.png)  

选择插入位置。  
![alt text](image-28.png)

## yxSpatialTableExplorer

按空间位置提取文本到 CSV（非标准表格提取）。  
工作非常依赖参数设置，容差值就是多大的距离认为是不同行或不同列。  
这里示例设置列容差 8，行容差 3.5  
![alt text](image-29.png)  
导出的结果  
![alt text](image-30.png)  

## yxPasteClipImage

将剪贴板中的图像数据保存到文件并粘贴为光栅图像。  
比如截图或从微信、QQ等右键复制的图像数据，在复制后执行命令，检测到剪贴板中有图像数据会弹出文件保存对话框，路径默认是在图纸所在路径，保存后调用粘贴命令插入光栅图像。  
![alt text](image-31.png)  

把上一张截图顺便插进图纸文件了。  
![alt text](image-32.png)  

## yxForceRemoveImage

强制删除光栅图像。  
插入光栅图像后，在图纸上可见的图像是引用，它还有一个定义，这个定义对象会占用图片文件。即使将图纸中所有的可见光栅图像删除，依然占用着文件，正常要删除，需要先关闭当前图纸文件后才能操作。  
这个功能就是用来删除光栅图像并解除占用后删除图片文件的。  
![alt text](image-33.png)  
![alt text](image-34.png)

## yxLocateDrawing

在文件资源管理器中打开当前图纸文件。  
用于快速定位到文件。  
![alt text](image-35.png)

## yxCreateIntersectionPoints

创建曲线（及延长线）的交点。  
![alt text](image-36.png)  

## yxImeAutoSwitch

输入法语言自动切换设置。  
启用后，当尝试输入命令而语言为非英语时，会自动触发 `Shift` 按键进行语言切换。间隔时间用于限制两次连续切换的间隔。  
![alt text](image-37.png)

## yxDialogMiddleClickToOk

对话框中鼠标中键映射确定按钮设置。  
类似 Siemens NX（UG）的鼠标中键功能，在对话框中，单机鼠标中键就是确定，不需要去点确定按钮或者按回车键。  
![alt text](image-38.png)

## yxCmdMiddleClickToEnter

命令中鼠标中键映射回车键设置。  
命令执行的过程中，可能涉及输入后确认，选择目标实体后确认，正常需要按回车键或空格键，这里直接把中键映射称回车键，鼠标随时点击。  
间隔是用于识别行为的阈值，鼠标中键长按可以用于拖动图纸，如果中键按下到放开的时间超过设置的间隔时间，那么就不执行映射到回车键。只有按下到放开的时间差小于设置的间隔时间，才认为需要映射到回车键。  
![alt text](image-39.png)

# yxSetLanguage

设置语言。  
如果没有匹配的翻译文件，则实际依然会显示中文。  
比如设置美式英语 `en_US`，则插件同目录下需要存在匹配词条的 `.\locales\en_US\LC_MESSAGES\messages.mo` 文件。  
![alt text](image-40.png)
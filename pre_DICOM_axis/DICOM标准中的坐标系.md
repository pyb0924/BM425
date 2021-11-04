# DICOM标准中的坐标系

## Prefix

### 方向余弦

<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/d/d4/Direction_cosine_vector.svg/1920px-Direction_cosine_vector.svg.png" alt="img" style="zoom: 20%;" />
$$
\begin{alignedat}{2}\alpha &{}=\cos a={\frac {\mathbf {v} \cdot \mathbf {e} _{x}}{\Vert \mathbf {v} \Vert }}&&{}={\frac {v_{x}}{\sqrt {v_{x}^{2}+v_{y}^{2}+v_{z}^{2}}}},\\\beta &{}=\cos b={\frac {\mathbf {v} \cdot \mathbf {e} _{y}}{\Vert \mathbf {v} \Vert }}&&{}={\frac {v_{y}}{\sqrt {v_{x}^{2}+v_{y}^{2}+v_{z}^{2}}}},\\\gamma &{}=\cos c={\frac {\mathbf {v} \cdot \mathbf {e} _{z}}{\Vert \mathbf {v} \Vert }}&&{}={\frac {v_{z}}{\sqrt {v_{x}^{2}+v_{y}^{2}+v_{z}^{2}}}}.\end{alignedat}
$$



## DICOM Tag

| Tag Name                  | (Group,Element) | VR   | Example                      | Remark |
| ------------------------- | --------------- | ---- | ---------------------------- | ------ |
| Patient position | (0018,5100) | CS | HFS |  |
| Image Position (Patient)  | (0020,0032)     | DS   | -0\\1\\-0\\-0\\-0\\-1 | 前三个数为x轴的方向余弦，后三个数为y轴的方向余弦 |
| Image Orientation (Patient) | (0020,0037)     | DS   | -24\\-199.166672\\199.166672 | 图像左上角的x,y,z坐标,单位是mm |
| Slice Thickness           | (0018,0050)     | DS   | 8 | 单位为mm |
| Slice Location            | (0020,1041)     | DS   | -24 | 单位为mm |

<img src="C:\Users\pyb0924\AppData\Roaming\Typora\typora-user-images\image-20211104210930540.png" alt="image-20211104210930540" style="zoom: 67%;" />

<img src="https://img-blog.csdnimg.cn/20210104100425838.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0VyaWNvaGU=,size_16,color_FFFFFF,t_70" alt="img" style="zoom: 80%;" />

## 坐标系

#### 患者坐标系

![在这里插入图片描述](https://img-blog.csdnimg.cn/20210606193122466.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2xpdXNoYW8xMDMxMTc3,size_16,color_FFFFFF,t_70,#pic_center)

- X轴正方向：从右（right）指向左（left）
- Y轴正方向：从前胸（anterior）指向后背（posterior）、
- Z轴正方向：从脚底（foot）指向头顶（head）

## Example



### 成像平面的确定

- 成像平面由Image Orientation Patient决定
- 常见的成像平面：
  - (1,0,0,0,1,0)：横断面（横截面，X-Y平面）
  - (0,1,0,0,0,-1)：矢状面（Y-Z平面）
  - (1,0,0,0,0,-1)：冠状面（X-Z平面）
- 一般的成像平面：
  - e.g. (1,0,0,0,0.132256,−0.991215)

<img src="https://img-blog.csdn.net/20170312203025276?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvenNzdXJlcWg=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast" alt="这里写图片描述" style="zoom: 67%;" />



### Reference

1. [zssure](https://zssure.blog.csdn.net/). DICOM世界观·第一章 坐标系统. [https://blog.csdn.net/zssureqh/article/details/61636150](https://blog.csdn.net/zssureqh/article/details/61636150)
2. Wikipedia contributors. (2021, October 28). Direction cosine. In *Wikipedia, The Free Encyclopedia*. Retrieved 13:13, November 4, 2021, from https://en.wikipedia.org/w/index.php?title=Direction_cosine&oldid=1052251529


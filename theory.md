# FOC理论

为了更精准的控制电机（转子），需精确控制定子所产生的磁场

为了精确控制定子所产生的磁场，需采集各相电流进行闭环控制

由于各相电流是三相交流电，需要通过clark 及 park变换，变成一个定值再输入到PID

PID输出的一个控制量（定值）后需通过反park及SVPWM技术控制三相逆变器，从而实现磁场的精确控制



## BLDC模型：

![image-20230819152847484](.\img\image-20230819152847484.png)



# open loop

设定一个uQ，uD值和一个变化theta角度，用于产生一个随theta旋转的电场，从而驱动转子



## 反PARK变换

将uQ和uD和转子角度，通过RevPark得到矢量（uAlpha,uBeta）
$$
uAlpha = cosθ * uD - sinθ* uQ
$$

$$
uBeta =  sinθ* uD  + cosθ * uQ
$$

矢量（uAlpha,uBeta）即下图中的Vδ

![](img\扇区1.jpg)

接下来需要利用PWM生成一个space vector Vδ

## SVPWM:

根据三相线圈的导通情况，可得到八个矢量，

![相电压](img\相电压.png)

其中V₀(000)和V₇(111)不产生扭矩，其余矢量将平面空间划分成六个扇区，



因各相合成的最大矢量所扫过的区域是一个正六边形![](img\六边形.png)

为实现转子更平稳的转动，需进行圆滑处理，将最大的合成矢量控制在√3/2*最大相电压
$$
V <= √3/3*Udc
$$
![](img\SVPWM1.png)

各矢量长度等于最大驱动电压2/3Udc，即当PWM占空比最大时，Vx = 2/3Udc



![](img\微信图片_20231119120036.png)

判定矢量V所在扇区

![](img\扇区判断1.png)

![](img\扇区判断2.png)

![](img\扇区判断3.png)
![](img\扇区判断4.png)

扇区内相邻两个非零向量和零向量的作用时间的计算

![](img\扇区1.jpg)

如在扇区一, 设Ts为PWM周期，T4、T6为PWM占空比，母线电压Udc：
$$
Uα = (T4 / Ts) * |V4| + (T6/Ts) * |V6| * cos(Π/3)
$$

$$
Uβ = (T6/Ts) * |V6| * sin(Π/3)
$$

因为有：
$$
|V4| = |V6| = 2/3 * Udc
$$
V4和V6在一个开关周期内的作用时间可以计算如下：
$$
T4 = (√3 * Ts/2 * Udc) * (√3 * Uα-Uβ)
$$

$$
T6 = (√3 * Ts/Udc) * uBeta
$$

为了在其他扇区中重复利用在第一个扇区得到的上述结论并简化表达式，我们可以定义三个变量。
$$
X = √3 * Ts / Udc * uBeta 
$$

$$
Y = √3 * Ts/Udc* (√3 * uAlpha/2 + uBeta/2)
$$

$$
Z =√3 * Ts/Udc * (-√3 * uAlpha/2 + uBeta/2)
$$

![](img\扇区计算.png)

可采用中心对齐PWM，形成如上图的波形，方便进行电流采样，亦可减小mosfet开关频率：

![微信图片_20230823202743](img\微信图片_20230823202743.jpg)



### ![微信图片_20230823223547](img\微信图片_20230823223547.jpg)

## 电流采样

三分流电阻式：三个采样电阻接在低位晶体管后![微信图片_20230823223557](img\微信图片_20230823223557.jpg)

所有低位晶体管都导通时同时进行采样，设三相采样的电流分别为Iu, Iv, Iw：Iu + Iv + Iw = 0

根据不同的扇区，有一相低电平时间最短，如扇区1，U相时间最短，扇区2：V相，扇区  3: W相

将最短的舍弃，根据其余两相和公式Iu + Iv + Iw = 0 求出第三相

# close loop

## with sensor

![image-20230819151459418](img\image-20230819151459418.png)

通过检测的三相电流和角度θ，进行clark及park变换后得到线性量iD，iQ

将iD，iQ和期望iQref, iDref输入PID，得到uQ, uD，继而输入上面的open loop从而实现电流（扭矩）闭环控制

实际应用中更多的是控制电机的位置或转速，需要将PID控制进行级联

### clark 变换

> 匀速旋转中的电机，检测到的相电流是间隔120度的正弦波

![image-20230819154553362](img\image-20230819154553362.png)

Ia, Ib,Ic对应于BLDC的三相电流方向，分解到α和β上：![image-20230819154701698](img\image-20230819154701698.png) 


$$
Iα =  Ia - Ib/2 - Ic/2
$$

$$
Iβ = √3 / 2 * Ib  - √3 / 2  * Ic
$$

由
$$
Ia + Ib + Ic = 0
$$
==》  																		
$$
Iα = 3Ia/2
$$

$$
Iβ = √3Ib + √3Ia/2
$$

> 实际电路中采集两路电流即可 	



Iα 方向与某一相重合，Iβ与Iα垂直，是相对于定子固定的两个矢量

设Id 和 Iq是互相垂直的随转子旋转的两个矢量，d轴和磁场同方向，随外部磁场转动，q轴与d轴垂直

![image-20230819162146579](img\image-20230819162146579.png)

设θ为电角度：

如上图，当定子经过六次换相后（100->110->010->011->001->101->100)，转子转一周，即电（磁）走完一周，转子也转一周，此时定子电角度等于转子的机械角度

而实际使用BLDC是定子多极对数的，如下图则是一个 24 / 3 = 8极对数，若该电机（定子）电磁走完一周，转子转动360/8=45°

需将传感器测得的转子（机械）角度转化为电角度：   θ = 转子角度 * 极对数

![多极对数电机](img\未命名1692969436.png)



### PARK 变换

得到Iα, Iβ后结合电角度θ，进行park变换：

![image-20230819160045849](img\image-20230819160045849.png)

![image-20230819155329820](img\image-20230819155329820.png)



电流方向即电压方向，电压方向即磁场方向，iD不产生扭矩，控制目标值为0



## PID

得到iQ和iD及转子角度θ及转速w后



## Sensorless

实际应用中，在考虑成本、可靠性等因素下，舍去电机传感器

需用软件建立观测器，对位置或速度进行估算，进行闭环控制

![](G:\workSpace\bldc\img\无感FOC.png)

### 状态观测器

状态观测器根据系统的输入输出来估计系统的状态，即根据已知量去估算未知量

![](G:\workSpace\bldc\img\状态观测器1.webp)

建立用于描述真实电机的数学模型，理论上如果建立的数学模型足够精确，当真实电机与电机数学模型具有相同的输入时，其必有相同的输出。此时我们就可以通过建立的电机数学模型来获得我们所需要的电机状态量。但上述系统会存在如下问题：

1、系统抗干扰能力差。电机是一个高阶非线性、强耦合、多变量的复杂系统，一旦存在外部干扰或负载突变，此时的观测器很难响应正确的输入，以致输出错误或者系统崩溃；

2、误差的存在导致观测器的输出不准确。误差的来源有系统误差和测量误差。系统误差是建模不准确所带来的误差，我们所建立的数学模型与实际的电机不可能完全相同，会存在建模误差；观测器的输入与真实输入的误差，观测器的输入是通过测量得来，会存在测量误差。

为了解决上述问题，我们对状态观测器增加反馈，通过反馈来不断的修正状态观测器的输出，使状态观测器尽可能的接近真实电机，如下图所示：

![](G:\workSpace\bldc\img\状态观测器1.webp)
**反馈方式与修正方式的不同产生了不同种类的状态观测器**

### 滑膜状态观测器

表贴式PMSM数学模型

![](G:\workSpace\bldc\img\surfaceMountPMSMmathModel.png)

其中psi （三叉戟）为磁链  L为电感  E为反向电动势  w为角速度   p = d/dt微分运算

![](G:\workSpace\bldc\img\观测器θ解算.jpg)

![](G:\workSpace\bldc\img\SMO计算z.webp)

![](G:\workSpace\bldc\img\SMO计算z图示.webp)

![](G:\workSpace\bldc\img\SMO反向电动势解算1.jpg)

![](G:\workSpace\bldc\img\SMO解算θ.webp)

![](G:\workSpace\bldc\img\传统SMO整体框图.webp)

![](G:\workSpace\bldc\img\改进SMO.webp)
$$
Eα  = z+mEα
$$

$$
Eβ  = z+mEβ
$$

根据求得的z即可得到反向电动势E

![](G:\workSpace\bldc\img\改进SMO图示.webp)

总结：通过对电机进行数学建模（未知量为反向电动势），并根据实际运行的输入和输出，通过反馈的方式解算出未知量（使未知量收敛），得到反向电动势进而算出θ

### 龙伯格观测器

电机数学模型：

![](G:\workSpace\bldc\img\电机数学模型.jpg)

![](G:\workSpace\bldc\img\龙伯格观测器引入.png)

其中左下角为现代控制理论中的状态空间方程

![](G:\workSpace\bldc\img\状态空间方程1.jpg)

其中右下角为传递函数，用状态空间方程：

![](G:\workSpace\bldc\img\状态空间方程2.jpg)

Z1(t) 为位移量，Z2(t)为速度量， U(t)为f(t)力量

![](G:\workSpace\bldc\img\状态空间方程3.jpg)

其中Z(t)是速度和加速度，Y(t)是速度，X(t)是f(t)力量

![](G:\workSpace\bldc\img\电机状态空间方程.jpg)







用状态空间表达式的形式建立用于描述电机的数学模型，并引入反馈。通过反馈不断的修正龙伯格观测器的状态，从而使观测器不断的接近真实电机

龙伯格观测器的结构如下图所示：

![](G:\workSpace\bldc\img\龙伯格结构.webp)

![](G:\workSpace\bldc\img\龙伯格一般表达式.webp)





​	



























因为没有传感器，采用速度环开环，电流环闭环的模式来启动电机，即流频比I/F控制















## 总结

根据电角度和电流的采样，用PID对电机的三相的电流进行闭环控制

电流产生磁场，（定子）电角度用于给磁场定向，即Field Oriented Control

电流环用于扭矩的控制，可用速度环和位置（角度）环实现串级PID，适应更多的应用场景 



参考链接

[SVPWM]: https://blog.csdn.net/michaelf/article/details/94013805
[观测器]: https://zhuanlan.zhihu.com/p/626503697

[现代控制理论]: https://zhuanlan.zhihu.com/p/57051153
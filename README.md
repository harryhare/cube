
____________________
##程序说明

###环境

windows,vs2008

###语言
c++

###图形

opengl,glut

###运行时，请在环境中增加，否则可能会影响操作流畅性
PATH=$(SolutionDir)glut\

###其他
程序第一次运行时，会在当前目录下生成一些数据文件，由于有大量计算所以启动可能比较慢<br/>
以后的运行，会直接加载这些文件，所以会快很多<br/>
____________________

##功能说明

###键盘控制说明：
数字键2-9——选择对应阶的魔方<br/>
s —— 自动还原魔方（2阶为最优解，3阶可能非最优解，不过一般少于30个face move，4阶及以上为模拟人工还原）<br/>
r —— 重置魔方<br/>
R —— 随机魔方 (该随机一定可解)<br/>
t —— 透明魔方<br/>
###鼠标控制说明
拖拽魔方——魔方对应层旋转<br/>
拖拽黑色背景——魔方整体旋转<br/>
____________________
##自动还原算法

###2阶还原
由于2阶魔方的状态数较少，所以把每种状态的深度，和当前的旋转动作，做成表，直接查找即可
###3阶还原
two phase算法，具体参考<br/>
http://kociemba.org/cube.htm

###n阶还原
模拟人工还原
____________________

##随机打乱
根据[魔方的状态数公式](http://www.speedcubing.com/chris/cubecombos.html)
打乱算法理论上可以到达所有可解状态

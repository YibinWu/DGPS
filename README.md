# Pseudorange Differential GPS

An C version implementation of the real-time pseudorange differential GPS algorithm based on the iterative least square. The navigation data, including observations, ephemeris, and etc., are obtained from the OEMSTAR board by serial communication. The differential correction information is also obatined from another OEMSTAR board by serial communication. Refering to the reference *OEMSTAR-Reference Manual* for the data formation. Please feel free to contact me if you want a version with detailed Chinese comments or have any questions. ybwu@whu.edu.cn

一个基于迭代最小二乘的实时伪距差分GPS定位的C语言版本的实现。导航数据，包括观测值，星历等均通过串口通信从NoVatel OEMSTAR板卡获取。差分改正信息同样通过串口从另一个板卡中获取。参考reference文件夹中的 OEMSTAR-Reference Manual 文件获取数据格式的相关信息。参考其他文件获得代码实现的相关信息。如果你想要一个有详细中文注释的版本，或者有任何问题，欢迎联系我。
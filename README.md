# logcat  
logcat是一个微型跨平台日志应用，它包括两部分：electron app和各平台编程接口。electron app用来打印日志，它与各平台的客户端通过udp协议通信。打印效果大致如下图：   
![image](https://github.com/limall/logcat/raw/master/forReadme/image1.png) 

简要教程链接
-------
https://www.jianshu.com/p/ec0cc4cd58c7  

特性
----
* 打印端使用electron框架  
* 打印端和客户端使用udp协议通信,可以接收远程日志。客户端可设置识别码(logid)，打印端可对识别码进行过滤 
* level和tag的概念与android的logcat一致  
* 通过Search输入框，可对日志的信息主体（对应上图界面中的内容栏）进行关键词搜索和过滤。  

适用平台
-------
* 打印端的接口使用udp协议，所以只要满足它的传输协议，打印端在linux、mac os、windows上都能用的
* 客户端的编程接口，即各个平台的传输协议的封装。这里只提供了win32和node.js的版本，win32版本可直接使用到cocos2d的项目中。lua需要使用者自己封装与c++的交互


# Graduation-Project
毕业设计：车载温湿度、定位信息采集器。基于Nanopi-neo开发板上的linux系统，读取模块中的温湿度、GPS信息，解析处理成JSON数据，再借助无线网络模块发送至服务器

车载信息采集器通过温湿度传感器、定位模块采集车厢环境及位置信息并借助网络发送至服务器（服务器没有实现）

硬件部分选用以全志 H3 为核心的 Nanopi NEO 开发板为主控、DHT11用于采集温湿度、Air530用于获取定位信息，无线网络适配器 150M 连接网络。 
在此基础上，于开发板所烧写的系统固件中展开相应的软件开发工作，上传的代码则为软件部分。

dht -相关： 温湿度模块信息的读取

gps -相关： 定位信息的读取

cJSON -相关： JSON格式数据生成

main ：与服务器建立连接并上传数据

uart ：串口配置文件

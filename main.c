#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h> //包含套接字的函数库
#include <netinet/in.h> //包含AF_INET的相关结构
#include <arpa/inet.h>  //包含AF_INET的操作函数
#include "gps.h"
#include "dht.h"
#include "cJSON.h"//JSON数据格式

#define PORT 3333 

pthread_mutex_t mut;

int init();
void send_truck_info(int sockfd);
void send_device_info(int sockfd);
void* write_dht(void* arg);
void* write_gps(void* arg);

int main(int argc,char **argv)
{

	int ret,tmp;//线程创建验证
	/**（待完成）借socket与服务器建立连接*/
	//客户端套接字标识符，只需要一个套接字文件描述符，用于和客户端通信
    int sockfd = init(); 

    send_truck_info(sockfd);//车辆信息
    sleep(1);
    send_device_info(sockfd);//设备信息
    
    pthread_mutex_init(&mut,NULL);

	pthread_t tid1;
    pthread_t tid2;
    pthread_create(&tid1,NULL,write_dht,&sockfd);   //线程1创建
    pthread_detach(tid1);

    sleep(1);

    pthread_create(&tid2,NULL,write_gps,&sockfd);   //线程2创建
    pthread_detach(tid2);
    //pthread_mutex_destroy(&Device_mutex);
    while(1){}
	//关闭连接
    close(sockfd);

	return 0;
}

/**
 * socket通信建立
 * @return [socket套接字sockfd]
 */
int init()
{
	int sockfd;
	//描述客户端的socket
    struct sockaddr_in addr;
    //消息长度
    int dataBytes = 0;
    //创建套接字
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
        perror("创建套接字失败");
        return 0;
    }
    printf("创建套接字成功\n");

    //初始化客户端套接字   htons和htonl将端口和地址转成网络字节序  定义客户端中地址中的域，AF_INET指IPv4
    addr.sin_family = AF_INET;
    //定义服务端的套接字端口
    addr.sin_port = htons(PORT);
    //指定服务器端的ip，本地测试：127.0.0.1  
    addr.sin_addr.s_addr = inet_addr("120.26.175.170");//inet_addr()函数，将点分十进制IP转换成网络字节序IP 

    /**********************连接服务器**********************/
    if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)  
    {  
        perror("连接服务器失败");  
        return 0;  
    }
    printf("连接到服务器...\n");
    return sockfd;
}
/**
 * 发送车辆信息
 * @param sockfd [socket套接字]
 */
void send_truck_info(int sockfd)
{
    cJSON *truck = cJSON_CreateObject();
    cJSON_AddItemToObject(truck,"type",cJSON_CreateString("C"));
    cJSON_AddItemToObject(truck,"ttype",cJSON_CreateString("M"));
    cJSON_AddItemToObject(truck,"remarks",cJSON_CreateString("suC6666"));
	char *sendbuff = cJSON_Print(truck);
	send(sockfd,sendbuff,strlen(sendbuff)+1,0);
	cJSON_Delete(truck);
	free(sendbuff);
}
/**
 * 发送设备信息
 * @param sockfd [socket套接字]
 */
void send_device_info(int sockfd)
{
	cJSON *device = cJSON_CreateObject();
    cJSON_AddItemToObject(device,"type",cJSON_CreateString("D"));
    cJSON_AddItemToObject(device,"dname",cJSON_CreateString("collector1"));
    cJSON_AddItemToObject(device,"remarks",cJSON_CreateString("suC6666"));
    char *sendbuff = cJSON_Print(device);
    send(sockfd,sendbuff,strlen(sendbuff)+1,0);
    cJSON_Delete(device);
	free(sendbuff);

}
/**
 * 读取dht，构建JSON数据，发送给服务器
 * @param arg [socket套接字]
 */
void* write_dht(void* arg)
{
	int dhtTemp = 0,dhtHdty = 0;
	DHT_INFO dinfo;
	dht_init();//配置dht
	memset(&dinfo , 0 , sizeof(dinfo));
	int fd;
	fd = *(int*)arg;
	
	while(1)
	{

		/**获取温湿度信息*/
		if ((dht11Read(DHT_HUMIDITY, &dhtHdty)) != -1) {
			dinfo.hdty = dhtHdty/1000;
        	printf("The humidity is %d RH\n", dhtHdty);
	    } else {
	        printf("Faided to get humidity\n");
	    }
	    if ((dht11Read(DHT_TEMP, &dhtTemp)) != -1) {
	    	dinfo.temp = dhtTemp/1000;
	        printf("The temperature is %d ℃\n", dhtTemp);
	    } else {
	        printf("Faided to get temperature\n");
	    }

	    cJSON *dht = cJSON_CreateObject();
	    cJSON_AddItemToObject(dht,"type",cJSON_CreateString("T"));
	    cJSON_AddItemToObject(dht,"dname",cJSON_CreateString("collector1"));
	    cJSON_AddItemToObject(dht,"temperature",cJSON_CreateNumber(dinfo.temp));
		cJSON_AddItemToObject(dht,"humidity",cJSON_CreateNumber(dinfo.hdty));

		char *sendbuff = cJSON_Print(dht);
		pthread_mutex_lock(&mut);//线程互斥
		sleep(1);
		send(fd,sendbuff,strlen(sendbuff)+1,0);
		pthread_mutex_unlock(&mut);
		free(sendbuff);
		cJSON_Delete(dht);
		sleep(10);
	}
	pthread_detach(pthread_self());
    return (void*)0;//线程终止
}



/**
 * 读取GPS数据，构建JSON数据，发送给服务器
 * @param arg [socket套接字]
 */
void* write_gps(void* arg)
{
	int n;
	int gps_fd = 0;//gps文件描述符
	char buff[BUFSIZ];
	GPS_INFO ginfo;//gps数据结构
	memset(&ginfo , 0 , sizeof(ginfo));//初始化
	gps_fd = gps_init();//配置gps
	
	int fd;//socket
	fd = *(int*)arg;

	pthread_detach(pthread_self());//线程分离
	while(1)
	{
		if((n = read(gps_fd,buff,sizeof(buff))) < 0)//判断GPS读取是否成功
		{
			perror("read error");
			return (void*)0;
		}
		/**获取GPS信息*/
		gps_acquisition(buff , &ginfo);
		if(ginfo.pos_state == 0){//获得的GPS数据无效
			sleep(2);
			continue;
		}
		/**创建JSON数据*/
	    cJSON *gps = cJSON_CreateObject();
	    cJSON_AddItemToObject(gps,"type",cJSON_CreateString("T"));
	    cJSON_AddItemToObject(gps,"dname",cJSON_CreateString("collector1"));
		cJSON_AddItemToObject(gps,"longtitude",cJSON_CreateNumber(ginfo.longtitude));
		cJSON_AddItemToObject(gps,"latitude",cJSON_CreateNumber(ginfo.latitude));
		/**发送JSON数据*/
		char *sendbuff = cJSON_Print(gps);
		pthread_mutex_lock(&mut);//线程互斥
		sleep(1);
		send(fd,sendbuff,strlen(sendbuff)+1,0);
		pthread_mutex_unlock(&mut);
		free(sendbuff);
		cJSON_Delete(gps);
		sleep(2);
	}
	close(gps_fd);
    return (void*)0;//线程终止
}


#ifndef _DHT_H_
#define _DHT_H_

#define DRIVER_MODULE "dht11"
#define	DHT_TEMP 		        (1)
#define	DHT_HUMIDITY	        (2)
#define DHT11_SYS_PATH          "/sys/devices/platform/dht11/"   
#define D_NAME					"iio:device0"
#define BUF_SIZE  (64)

typedef unsigned int UINT;

typedef struct dinfo
{
	int temp;
	int hdty; 
}DHT_INFO;

extern int dht_init();
extern int dht11Read(int type, int *data);
extern int readIntValueFromFile(char* fileName); 

#endif
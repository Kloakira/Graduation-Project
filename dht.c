#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>//opendir();closedir();
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "dht.h"

int dht_init()
{
	char modStr[BUF_SIZE]; 
	int pintoGPIO = 203;
	sprintf(modStr, "modprobe %s gpio=%d", DRIVER_MODULE,pintoGPIO);//pin:7→gpio：203
	system(modStr);
}


int dht11Read(int type, int *data) 
{
    int ret = -1;
    DIR *d;

    if (!(d = opendir(DHT11_SYS_PATH))) {
        printf("Fail to opendir %s", DHT11_SYS_PATH);
        return 0;
    }
    char dht11File[128];

    if (type == DHT_HUMIDITY)
    {
    	sprintf(dht11File, "%s%s/in_humidityrelative_input",DHT11_SYS_PATH,D_NAME);
    	ret = readIntValueFromFile(dht11File);
    }
    else if(type == DHT_TEMP)
    {
		sprintf(dht11File, "%s%s/in_temp_input",DHT11_SYS_PATH, D_NAME);
    	ret = readIntValueFromFile(dht11File);
    }else{
    	printf("Unsupport dht11 data type %d", type);
    }

    closedir(d);

    if (ret != -1) {
        *data = ret;
        ret = 0;
    } else {
        printf("Invalid dht11 data");
    }

    return ret;
}

int readIntValueFromFile(char* fileName) 
{
    char buff[255];
    int ret = -1;
    memset(buff, 0, sizeof(buff));

    FILE *fp = fopen(fileName,"r");
    if (fp == NULL) {
        printf("Unable to open file %s",fileName);
        return -1;
    } else {
        if (fread(buff, sizeof(char), sizeof(buff)-1, fp)>0) {
            ret = 0;
        }
    }
    fclose(fp);

    if (ret == 0) {
        return atoi(buff);//有结果的话返回的是buff里的内容
    }
    return ret;
}

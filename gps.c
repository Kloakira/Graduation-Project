#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include "gps.h"

int gps_acquisition(char *buff,GPS_INFO *data)
{
	float speed;//速度
	float direction;//方位角，这两个数据不需要
	char time[10], latitude[10], longtitude[10], a1[10], b1[10], date[10], i[10];
	char state;
	char *str = NULL;
	if(data == NULL)	return -1;
	if(strlen(buff) < 10)  return -1;
	if((str=strstr(buff , "$GNRMC")) == NULL) return -1;//在字符串中找到GPRMC并把其地址赋给str

	sscanf(str, "$GNRMC,%[^','],%c,%[^','],N,%[^','],E,%[^','],%[^','],%[^','],%s" , time , &state , latitude , longtitude , a1 , b1, date,i );
	data->date = atoi(date);
	data->time = atoi(time);
	data->latitude = atof(latitude);
	data->longtitude = atof(longtitude);

	if(state == 'A')
	{
		data->pos_state = 1;
	}else{
		data->pos_state = 0;
	}

}

int gps_init()
{
	int fd = 0;
	char *dev_name = "/dev/ttyS1";
	if((fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
	{
		perror("fail to open the ttyUSB0 Serial Port");
		return -1;
	}
	set_option(fd , 9600 , 8 , 'N' , 1);
	return fd;
}
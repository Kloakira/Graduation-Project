#ifndef _GPS_H_
#define _GPS_H_

typedef unsigned int UINT;

typedef struct ginfo
{
	UINT time;//时间：hhmmss.ss
	UINT date;//日期：ddmmyy
	UINT pos_state;//定位状态 ：1=数据有效；0=数据无效
	double latitude;//纬度：ddmm.mmmm
	double longtitude;//经度：dddmm.mmmm

}GPS_INFO;

extern int set_option(int fd,int nSpeed,int nBits,char nEvent,int nStop);
extern int gps_acquisition(char *buff,GPS_INFO *data);
extern int gps_init();
#endif
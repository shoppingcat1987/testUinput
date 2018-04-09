#define LOG_NDEBUG 0
#define LOG_TAG "testUinput"

#include<stdio.h>
#include<linux/uinput.h>
#include<linux/input.h>
#include<linux/fcntl.h>
#include <log/log.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

#define POINT_MUM 40
#define MAX_X 1920
#define MAX_Y 1080

int uinput_fd=0;
struct uinput_user_dev uinput_dev;
struct input_event event;

typedef struct _touch_point_info_t
{
	int id;
	int x;
	int y;
	int pressure;
}touch_point_info_t;



int setup_uinput_device()
{
		ALOGV("setup_uinput_device");
        uinput_fd=open("/dev/uinput",O_WRONLY|O_NDELAY);
        if(uinput_fd<0)
        {
                return -1;
        }
		ioctl(uinput_fd, UI_SET_EVBIT, EV_SYN);   
		ioctl(uinput_fd, UI_SET_EVBIT, EV_ABS);   
		ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
		ioctl(uinput_fd, UI_SET_ABSBIT, ABS_X);   
		ioctl(uinput_fd, UI_SET_ABSBIT, ABS_Y);  
		
		ioctl(uinput_fd, UI_SET_ABSBIT, ABS_MT_SLOT);  
		ioctl(uinput_fd, UI_SET_ABSBIT, ABS_MT_POSITION_X);  
		ioctl(uinput_fd, UI_SET_ABSBIT, ABS_MT_POSITION_Y);  
		ioctl(uinput_fd, UI_SET_ABSBIT, ABS_MT_TRACKING_ID);  
		ioctl(uinput_fd, UI_SET_ABSBIT, ABS_MT_WIDTH_MAJOR);  
		ioctl(uinput_fd, UI_SET_ABSBIT, ABS_MT_TOUCH_MAJOR);  
		
		ioctl(uinput_fd, UI_SET_KEYBIT, BTN_TOUCH);   
		
		memset(&uinput_dev,0,sizeof(uinput_dev)); 
		strncpy(uinput_dev.name, "test mt device", UINPUT_MAX_NAME_SIZE);   
		uinput_dev.id.version = 4;   
		uinput_dev.id.bustype = BUS_USB;  
		uinput_dev.absmax[ABS_X]=MAX_X;
		uinput_dev.absmax[ABS_Y]=MAX_Y;
		uinput_dev.absmax[ABS_MT_POSITION_X]=MAX_X;
		uinput_dev.absmax[ABS_MT_POSITION_Y]=MAX_Y;
		uinput_dev.absmax[ABS_MT_SLOT]=POINT_MUM;
		uinput_dev.absmax[ABS_MT_TRACKING_ID]=POINT_MUM;
		uinput_dev.absmax[ABS_MT_TOUCH_MAJOR]=255;
		uinput_dev.absmax[ABS_MT_WIDTH_MAJOR]=255;
		write(uinput_fd, &uinput_dev, sizeof(uinput_dev));   
		
		if (ioctl(uinput_fd, UI_DEV_CREATE))   
		{   
			ALOGV("Unable to create UINPUT device.");   
			return -1;   
		}   
				
       
        return 0;
}


int release_uinput_device()
{
	ioctl(uinput_fd, UI_DEV_DESTROY);   
	close(uinput_fd);   
	return 0;
}



void send_event(int type,int code,int value)
{
	memset(&event, 0, sizeof(event));   
	gettimeofday(&event.time, NULL);
	event.type = type;   
	event.code = code;   
	event.value = value;   
	write(uinput_fd, &event, sizeof(event));   
	usleep(100);
}


void mt_report_event(touch_point_info_t *touch_point_info,int point_count,int touch)
{
	int i;
	if(touch!=0)
	{
		
		for(i=0;i<point_count;i++)
		{
			//send_event(EV_KEY,BTN_TOUCH,0x01);
			
			send_event(EV_ABS,ABS_MT_SLOT,touch_point_info[i].id);
			send_event(EV_ABS,ABS_MT_TRACKING_ID,touch_point_info[i].id);
			send_event(EV_ABS,ABS_MT_POSITION_X,touch_point_info[i].x);
			send_event(EV_ABS,ABS_MT_POSITION_Y,touch_point_info[i].y);
			send_event(EV_ABS,ABS_X,touch_point_info[i].x);
			send_event(EV_ABS,ABS_Y,touch_point_info[i].y);
			send_event(EV_ABS,ABS_MT_TOUCH_MAJOR,1);
			send_event(EV_ABS,ABS_MT_WIDTH_MAJOR,1);
			
		}
	}
	else
	{
		for(i=0;i<point_count;i++)
		{
			send_event(EV_ABS,ABS_MT_SLOT,touch_point_info[i].id);
			send_event(EV_ABS,ABS_MT_TRACKING_ID,-1);
		}
		//send_event(EV_KEY,BTN_TOUCH,0x00);
	}
	send_event(EV_SYN,SYN_REPORT,0);
}

//frameworks/native/include/input/Input.h


int main()
{
   int i,j,k;
  
   touch_point_info_t touch_point_info[POINT_MUM];
   setup_uinput_device();
 	
   for(j=0;j<POINT_MUM;j++)
   	{
   		for(k=0;k<2;k++)
		{	
			for(i=0;i<j;i++)
			{
				touch_point_info[i].id=i;
				touch_point_info[i].x=(int)((float)rand()*(float)MAX_X/(float)RAND_MAX);
				touch_point_info[i].y=(int)((float)rand()*(float)MAX_Y/(float)RAND_MAX);
				touch_point_info[i].pressure=100;		
			}
			send_event(EV_KEY,BTN_TOUCH,0x01);
			send_event(EV_SYN,SYN_REPORT,0);
			mt_report_event(touch_point_info,i-1,1);
			sleep(2);
			
		}
	    mt_report_event(touch_point_info,i-1,0);
	 	send_event(EV_KEY,BTN_TOUCH,0x00);
	 	send_event(EV_SYN,SYN_REPORT,0);
	 	sleep(1);
	
   	}
   
	
   release_uinput_device();
   return 0;
}


#include <reg52.h>
#include "delay.h"
#include "ds1302.h"
#include "smg.h"
#include "onewire.h"
#include "iic.h"
#include "select573.h"
bit readTimeFlag=0;
bit auto_flag = 0;
bit S7_switch = 0;
bit is_select = 0;
bit is_sys = 0;
bit is_alarm = 1;

sbit S7 = P1^0;
sbit S6 = P1^1;
sbit S5 = P1^2;
sbit S4 = P1^3;

unsigned char max_limt[3] = {2,5};
unsigned char low_limt[3] = {1,0};

unsigned int tempL;
unsigned int tempH;


extern unsigned char cur_time_buf[8];

sbit LED_AUTO = P0;
sbit SPEAKER = P3^3;
sendBytesTo24C02(unsigned char suba,unsigned char *p,unsigned char no);
void Alarm(unsigned int count)
{
	unsigned char i;
	while(count>0)
	{
		for(i=0;i<200;i++)
		{
			DelayUs2x(200); 
			SPEAKER=!SPEAKER;
		}
		SPEAKER=0;
		DelayMs(100);  
		count--;
	}	
}

void init_timer0()
{
	TMOD= 0X01;
	TH0 = (65536 - 10000) / 256;
	TL0 = (65536 - 10000) % 256;
	ET0 = 1;
	TR0 = 1;
	EA  = 1;
}

void timer0_Sevr() interrupt 1
{
	static unsigned char cnt1,cnt2;
	TH0 = (65536 - 10000) / 256;
	TL0 = (65536 - 10000) % 256;
	cnt1++;
	if(cnt1==1)
	{
		cnt1=0;
		SelectHC573(6);
		P0 = 0x00;
		SelectHC573(7);
		showLed();
		P0 = 0x00;
		SelectHC573(0);
	}
	cnt2++;
	if(cnt2==200)
	{
		cnt2=0;
		readTimeFlag=1;
		TR0=0;
	}

}
unsigned int Read_Temperature()
{
    unsigned char LSB,MSB;
    unsigned int T_dat;
    TR1=0;
    Init_DS18B20();
    Write_DS18B20(0xCC);
    Write_DS18B20(0x44);
    TR1=1;
    TR1=0;
    Init_DS18B20();
    Write_DS18B20(0xCC);
    Write_DS18B20(0xBE);
    LSB=Read_DS18B20();
    MSB=Read_DS18B20();
    Init_DS18B20();
    TR1=1;
    
    T_dat=0x0000;
    T_dat=MSB;
    T_dat<<=8;
    T_dat=T_dat|LSB;
    
    return T_dat;
 
}
void doShowTemperature()
{
    unsigned int T_dat;
    float temp_f;
    char temp_z;
    char temp_x;
    T_dat=Read_Temperature();
    if((T_dat&0xf800)==0x0000)
	{
		temp_f=T_dat*0.0625;
		temp_z=temp_f;
		if(temp_z/10>=1)
		{
			updateLed(3,temp_z/10);
		}
		updateLed(4,temp_z%10);
		temp_x=(temp_f-temp_z)*10;
    }
    else
	{
		temp_f=(~T_dat+0x01)*0.0625;
		temp_z=temp_f;
		if(temp_z/10>=1)
		{
			updateLed(4,11);
			updateLed(5,temp_z/10);
		}
		else
		{
			updateLed(5,11);
		}
		updateLed(6,temp_z%10);
		temp_x=(temp_f-temp_z)*10;
		updateLed(7,temp_x);
    }
}


void doShowTemperature2()
{
    unsigned int T_dat;
    float temp_f;
    char temp_z;
    char temp_x;
    T_dat=Read_Temperature();
    if((T_dat&0xf800)==0x0000)
	{
		temp_f=T_dat*0.0625;
		temp_z=temp_f;
		if(temp_z/10>=1)
		{
			updateLed(6,temp_z/10);
		}
		updateLed(7,temp_z%10);
		temp_x=(temp_f-temp_z)*10;
    }
    else
	{
		temp_f=(~T_dat+0x01)*0.0625;
		temp_z=temp_f;
		if(temp_z/10>=1)
		{
			updateLed(4,11);
			updateLed(5,temp_z/10);
		}
		else
		{
			updateLed(5,11);
		}
		updateLed(6,temp_z%10);
		temp_x=(temp_f-temp_z)*10;
		updateLed(7,temp_x);
    }
}




void display_VOL()
{
	unsigned char convertVal;
	float vol;
	unsigned char val;
    ET0=0;
    convertVal=Read_PCF8591(0);
    ET0=1;
    vol=5.0/255*convertVal*0.198;
    val=vol*100;
    updateLed(6, val/10);
    updateLed(7, val%10);
}
void CheckSMG()
{	
	SelectHC573(4);
	P0=0x00;
	P0 = 0xfe;
	SelectHC573(0);
}

void auto_start()
{
	SelectHC573(6);
	Ds1302_Read_Time();
	TR0=1;
	updateLed(0,cur_time_buf[4]/10);
	updateLed(1,cur_time_buf[4]%10);
	updateLed(2,11);
	doShowTemperature();
	updateLed(5,11);
	display_VOL();
	
}

void auto_switch()
{
	SelectHC573(6);

	updateLed(0,cur_time_buf[4]/10);
	updateLed(1,cur_time_buf[4]%10);
	updateLed(2,11);
	doShowTemperature();
	updateLed(5,11);
	Ds1302_Read_Time();
	TR0=1;
	updateLed(0,cur_time_buf[4]/10);
	updateLed(1,cur_time_buf[4]%10);
	
}

void auto_mode_select()
{
	SelectHC573(6);
	//Ds1302_Read_Time();
	TR0=1;
	updateLed(0,tempH/10%10);
	updateLed(1,tempH%10);
	updateLed(2,11);
	updateLed(3,tempL/10%10);
	updateLed(4,tempL%10);
	updateLed(5,11);
	display_VOL();
	//doShowTemperature2();
	//DelayMs(500);  
}

void ReaL(unsigned char R_flag)
{
	if(R_flag)
	{
		SelectHC573(5);
		P0=0xff;
		/*SelectHC573(4);
		P0=0xff;*/
		SelectHC573(0);	
	}
	else
	{
		SelectHC573(5);
		P0=0x00;
		/*SelectHC573(4);
		P0=0xff;*/
		SelectHC573(0);	
	}
}

void initSystem()
{


	SelectHC573(5);
	P0=0xff;
	SelectHC573(4);
	P0=0xff;
	SelectHC573(0);		

	/*else
	{
		SelectHC573(5);
		P0=0x00;
		SelectHC573(4);
		P0=0x00;
		SelectHC573(0);	
	}*/

}
void scanKeys()
{
	if(S7==0)
	{
		DelayMs(5);
		if(S7==0 && S7_switch == 0)
		{
			S7_switch = 1;
			while(S7==0)
				DelayMs(5);
			SelectHC573(4);
			P0 = 0xfd;		
			SelectHC573(0);
			is_select = 0;
		}
		else
		{
			S7_switch = 0;
			while(S7==0)
				DelayMs(5);
			SelectHC573(4);
			P0 = 0xfe;		
			SelectHC573(0);
		}
	}
	if(S6==0)
	{
		DelayMs(5);
		if(S6==0&&S7_switch==1)
		{
			while(S6==0)
			{
				DelayMs(5);
			}
			is_select = 0;
		}
		else if(S6==0&&S7_switch==0)
		{
			while(S6==0)
			{
				DelayMs(5);
			}
			if(!is_select)
			{
				is_select = 1;
				is_alarm = 0;
			}
				
			else
			{
				is_select = 0;
				is_alarm = 1;
			}
				
		}
	}
	if(S5==0)
	{
		DelayMs(5);
		if(S5==0&&S7_switch==1)
		{
			while(S5==0)
			{
				DelayMs(5);
			}
			is_sys = 1;
			ReaL(1);
		}
		else if(S5==0&&S7_switch==0)
		{
			while(S5==0)
			{
				DelayMs(5);
			}
			tempH++;
		}
	}
	if(S4==0)
	{
		DelayMs(5);
		if(S4==0&&S7_switch==1)
		{
			while(S4==0)
			{
				DelayMs(5);
			}
			is_sys = 1;
			ReaL(0);
		}
		else if(S4==0&&S7_switch==0)
		{
			while(S4==0)
			{
				DelayMs(5);
			}
			tempL--;
		}
	}
}
void temperture_check()
{
    unsigned int T_dat;
    float temp_f;
    T_dat=Read_Temperature();
    if((T_dat&0xf800)==0x0000)
		temp_f=T_dat*0.0625;
    else
		temp_f=(~T_dat+0x01)*0.0625;
			
	if(temp_f > tempH)
	{
		if(is_select)
		{
			if(is_alarm)
				Alarm(3);
		}
		else
		{
			//ReaL(1);
		}
		
	}
	else if(temp_f < tempL)
	{
		if(is_select)
		{
			if(is_alarm)
				Alarm(3);
		}
		else
		{
			//ReaL(1);
		}
	}
}
void main()
{
	Ds1302_Init();
	init_timer0();
	initSystem();
	Ds1302_Write_Time();

	tempL = 10;
	tempH = 25;
	sendBytesTo24C02(0x05, &tempL, 1);
	sendBytesTo24C02(0x07, &tempL, 1);
	DelayMs(10);
	while(1)
	{
		if(readTimeFlag==1)//&&is_select==0
		{
			if(!auto_flag)
			{
				CheckSMG();
				auto_flag= 1;
			}	
			readTimeFlag==0;
			if(!is_select)
				auto_start();
			else
				auto_mode_select();
		}
		scanKeys();
		temperture_check();
		DelayMs(10);
	}
}
	
	
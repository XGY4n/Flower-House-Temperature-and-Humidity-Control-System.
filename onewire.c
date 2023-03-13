/*
  程序说明: 单总线驱动程序
  软件环境: Keil uVision 4.10 
  硬件环境: CT107单片机综合实训平台
  日    期: 2011-8-9
*/

#include "onewire.h"

//单总线延时函数
void Delay_OneWire(unsigned int t)
{
  while(t--);
}

//DS18B20芯片初始化
bit Init_DS18B20(void)
{
	bit initflag = 0;
	DQ = 1;
	Delay_OneWire(12);
	DQ = 0;
	Delay_OneWire(80); 		//拉低总线480us以上
	DQ = 1;									//然后释放总线
	Delay_OneWire(10); 			//等待15～60us
	initflag = DQ;    			//读取DS18B20的复位应答信号
	Delay_OneWire(5);			//等待60～240us后释放总线
  
	return initflag;				//应答信号为低电平，表示复位成功
}

//通过单总线向DS18B20写一个字节
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;								//先将总线拉低10~15us
		DQ = dat&0x01;				//向总线写入数据
		Delay_OneWire(5);		//维持20～45us
		DQ = 1;								//释放总线
		dat >>= 1;						//发送下一个数据位
	}
	Delay_OneWire(5);
}

//从DS18B20读取一个字节
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;								//先将总线拉低10～15us
		dat >>= 1;						//数据位右移
		DQ = 1;								//释放总线
		if(DQ)								//读取总线上的数据
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5);		//延迟45us左右，再读下一个数据位
	}
	return dat;
}
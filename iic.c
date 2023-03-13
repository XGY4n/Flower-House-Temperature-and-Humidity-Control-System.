
/*-----------------------------------------------
  ���ƣ�IICЭ�� 
  ��̳��www.doflye.net
  ��д��shifang
  �޸ģ���
  ���ݣ������ǲ���������ʱ�ķ�������SCL����,�̶Ը߾���Ƶ��Ҫ�� һ�����޸�....(������1us����
		����,������Ƶ��ҪС��12MHZ)
------------------------------------------------*/  
  
#include "iic.h"
#include "delay.h"
#include <reg52.h> 


#define ADDR_PCF8591  0x90  //PCF8591������ַ  1001 0000 д����+1
#define ADDR_24C02    0xA0  //24C02������ַ  1010 0000    д����+1

sbit SDA=P2^0;
sbit SCL=P2^1;

void Delay_IIC(unsigned char time){
	while(time--);
}

void IIC_Start(void){
	SDA=1;
	SCL=1;
	Delay_IIC(5);
	SDA=0;
	Delay_IIC(5);
	SCL=0;
}

void IIC_Stop(void){
	SDA=0;
	SCL=1;
	Delay_IIC(5);
	SDA=1;
	Delay_IIC(5);
}

void IIC_Ack(unsigned char ackbit){
	if(ackbit) 	SDA=0;
	else		SDA=1;

	Delay_IIC(5);
	SCL=1;
	Delay_IIC(5);
	SCL=0;
	SDA=1;
	Delay_IIC(5);;
}

bit IIC_WaitAck(void){
	SDA=1;
	Delay_IIC(5);
	SCL=1;
	Delay_IIC(5);

	if(SDA){
		SCL=0;
		IIC_Stop();
		return 0;
	}
	else{
		SCL=0;
		return 1;
	}
}

void IIC_SendByte(unsigned char byt){
	unsigned char i;
	for(i=0;i<8;i++){
		if(byt & 0x80)	SDA=1;
		else			SDA=0;
	

		Delay_IIC(5);
		SCL=1;
		byt<<=1;
		Delay_IIC(5);
		SCL=0;
	}
}

unsigned char IIC_RecByte(void){
	unsigned char da, i;
	for(i=0;i<8;i++){
		SCL=1;
		Delay_IIC(5);
		da<<=1;
		if(SDA)	da|=0x01;
		SCL=0;
		Delay_IIC(5);
	}
	return da;
}

unsigned char Read_PCF8591(unsigned char com ){
 unsigned char dat;
 
  IIC_Start();
  IIC_SendByte(ADDR_PCF8591);  //???????0x90
  IIC_WaitAck();   
  IIC_SendByte(com); //?????? com
  IIC_WaitAck();  
  IIC_Stop(); 
 
  IIC_Start();
  IIC_SendByte(ADDR_PCF8591+1); //???????0x91
  IIC_WaitAck(); 
  dat = IIC_RecByte(); //??????? 
  IIC_Ack(0); 
  IIC_Stop(); 
 
  return dat; //????Information
}




bit sendBytesTo24C02(unsigned char suba,unsigned char *p,unsigned char no)
{
   unsigned char i;
 	for(i=0;i<no;i++)
	{ 
   	IIC_Start();               	//????
   	IIC_SendByte(ADDR_24C02); 	//??????
    IIC_WaitAck();
		IIC_SendByte(suba);         //???????
    IIC_WaitAck();
		IIC_SendByte(*p);            //????
 		IIC_WaitAck();
    IIC_Stop();                  //????
	 	
		DelayMs(1);               //??????????????????
	 	p++;
	 	suba++;
   } 
	return (1);
}

bit recBytesFrom24C02(unsigned char suba,unsigned char *p,unsigned char no)
{
	unsigned char i;
  IIC_Start();               //????			 
  IIC_SendByte(ADDR_24C02);             //??????
	IIC_WaitAck();   	
	IIC_SendByte(suba);            //???????
	IIC_WaitAck();
  IIC_Start();
  IIC_SendByte(ADDR_24C02+1);				//??? 1
	IIC_WaitAck();
  	
	for(i=0;i<no-1;i++) 
	{ 
    *p=IIC_RecByte();              //????
     IIC_Ack(1);                //????? 
     p++;
  } 
   	*p=IIC_RecByte();
    IIC_Ack(0);                 //?????
    IIC_Stop();                    //????
  	return(1);
}

/*unsigned char Read_PCF8591(unsigned char com)
{
	unsigned char dat;
	
	IIC_Start();
	IIC_SendByte(ADDR_PCF8591);
	IIC_WaitAck();
	IIC_SendByte(com);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(ADDR_PCF8591 + 1);
	IIC_WaitAck();
	dat = IIC_RecByte();
	IIC_Ack(0);
	IIC_Stop();
	
	return dat;
}
*/




























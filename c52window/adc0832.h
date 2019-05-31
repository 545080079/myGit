#include <reg52.h>
#include <intrins.h>

 
#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long




sbit ADCS = P3^2;	//adc0832 时钟引脚定义
sbit ADCLK =P3^6;
sbit ADDI = P3^4;	//adc0832  数据引脚定义
sbit ADDO = P3^4;	//adc0832  片选引脚定义
  











		  
uchar ADC0832(bit mode,bit channel)    //AD转换，返回结果
{
	uchar i,dat,ndat;
	
	ADCS = 0;//拉低CS端
	_nop_();
	_nop_();
	
	ADDI = 1;	//第1个下降沿为高电平
	ADCLK = 1;//拉高CLK端
	_nop_();
	_nop_();
	ADCLK = 0;//拉低CLK端,形成下降沿1
	_nop_();
	_nop_();
	
	ADDI = mode;	//低电平为差分模式，高电平为单通道模式。	
	ADCLK = 1;//拉高CLK端
	_nop_();
	_nop_();
	ADCLK = 0;//拉低CLK端,形成下降沿2
	_nop_();
	_nop_();
	
	ADDI = channel;	//低电平为CH0，高电平为CH1	
	ADCLK = 1;//拉高CLK端
	_nop_();
	_nop_();
	ADCLK = 0;//拉低CLK端,形成下降沿3
	
	ADDI = 1;//控制命令结束(经试验必需)
	dat = 0;
	//下面开始读取转换后的数据，从最高位开始依次输出（D7~D0）
	for(i = 0;i < 8;i++)
	{
		dat <<= 1;
		ADCLK=1;//拉高时钟端
		_nop_();
		_nop_();
		ADCLK=0;//拉低时钟端形成一次时钟脉冲
		_nop_();
		_nop_();
		dat |= ADDO;
	}
	ndat = 0; 	   //记录D0
	if(ADDO == 1)
	ndat |= 0x80;
	//下面开始继续读取反序的数据（从D1到D7） 
	for(i = 0;i < 7;i++)
	{
		ndat >>= 1;
		ADCLK = 1;//拉高时钟端
		_nop_();
		_nop_();
		ADCLK=0;//拉低时钟端形成一次时钟脉冲
		_nop_();
		_nop_();
		if(ADDO==1)
		ndat |= 0x80;
	}	  
	ADCS=1;//拉高CS端,结束转换
	ADCLK=0;//拉低CLK端
	ADDI=1;//拉高数据端,回到初始状态
	if(dat==ndat)
	return(dat);
	else
	return 0;   
}

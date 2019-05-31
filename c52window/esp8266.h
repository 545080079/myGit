
#include<string.h>

#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long

sbit kaiguan = P3^4;

sbit rs=P2^0;//LCD 1602 引脚定义
sbit rd=P2^1;//LCD 1602 引脚定义
sbit lcden=P2^2;//LCD 1602 引脚定义
uchar num;
unsigned char Usart_Receive[20]={0};
unsigned char Usart_Cnt=0;
bit Usart_AT_flage;
unsigned char code RST[]="AT+RST\r\n";
unsigned char code CWMODE2[]="AT+CWMODE=2\r\n";
unsigned char code CIPMUX[]="AT+CIPMUX=1\r\n";
unsigned char code CIFSR[]="AT+CIFSR\r\n";
unsigned char code CIPSERVER[]="AT+CIPSERVER=1,8876\r\n";
uchar code logo1[]= "WinSysByLyd    x "; 
uchar code logo2[]="window   system  ";
uchar code logo3[]="S=253 T=85 U=85%";
uchar code logo4[]="Br=     12:00:00 ";
uchar code logo5[]="window open      ";
uchar code logo6[]="window close     ";
uchar code logo7[]="GWbrig=    LUX  ";
uchar code logo8[]="UP-Hum=    %    ";
uchar code logo9[]="Please input...  ";
uchar code logo10[]="Please wait...  ";
uchar code logo11[]="UP-TEM=    oC   ";
uchar code logo12[]="SEtime  12:00:00";
uchar code logo13[]="ONtime  12:00:00";
uchar code logo14[]="OFtime  12:00:00";
uchar code logo15[]="UP-SMO=         ";
uchar code logo16[]="KWbrig=    LUX  ";	

uchar code logolink1[]="Link Success   1 ";
uchar code logolink2[]="Link Success   2 ";   	   
uchar code waitMess[]="Wait Message   0 ";	   
uchar flag_set='f';

		
/***************延时500ms***************************/
void delays500ms(void)
{
	unsigned char a,b,c;
	for(c=123;c>0;c--)
	{
		for(b=212;b>0;b--)
		{
			for(a=25;a>0;a--)
			{
			}
		}
	}
}

void delay(uint z)//延时函数
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=10;y>0;y--);
}


void delaylog(uint z)//延时函数
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=500;y>0;y--);
}




void write_com(uchar com) //LCD 1602 写指令
{
	rs=0;//	RS置低电平是Command
	lcden=0;
	P0=com;
	delay(5);
	lcden=1;
	delay(5);
	lcden=0;	
}

void write_date(uchar date)//LCD 1602 写数据
{
	rs=1;// RS置高电平是Data
	lcden=0;
	P0=date;
	delay(5);
	lcden=1;
	delay(5);
	lcden=0;	
}

void init1602()//LCD 1602 初始化
{
	uchar num;
	rd=0;
	lcden=0;
	write_com(0x38);
	write_com(0x0c);
	write_com(0x06);
	write_com(0x01);	  

	write_com(0x80);
	for(num=0;num<16;num++)
		{
			write_date(logo1[num]);//开机字幕
			delay(2);
		}

	write_com(0x80+0x40);
	for(num=0;num<16;num++)
		{
			write_date(logo2[num]);	//开机字幕
			delay(2);
		}

	delaylog(200);	
 //***************************************************

		  write_com(0x80);

		  	for(num=0;num<16;num++)	//开机字幕
		{
			write_date(logo6[num]);
			delay(2);
		}

	
   

	write_com(0x80+0x40);
	for(num=0;num<16;num++)
		{
			write_date(logo10[num]);//开机字幕
			delay(2);
		}
		   							  

}





/**********************************************************/
void delay_ms(unsigned long t)
{
	unsigned int x,y;
	for(x=t;x>0;x--)
	{
		for(y=110;y>0;y--)
		{
		}
	}
}
/**********************************************************/
void delays(void)
{
	unsigned char a,b,c;
	for(c=95;c>0;c--)
	{
		for (b=26;b>0;b--)
		{
			for (a=185;a>0;a--)
			{
			}
		}
	}
}
/***************中断设置***********************************/
void InitUART(void)
{
	TMOD=0x20;
	SCON=0x50;//0101 0000		SM0=0 SM1=1-->方式1		REN=1-->允许接收数据
	PCON=0x80;//PCON
/***************波特率设置*********************************/ 
	TH1=0xF3;	 //4800 BAUD
	TL1=TH1;
	EA=1;
	ES=1;
	TR1=1;	  
}
/*************发送数据***************************************/
void Send_Uart(unsigned char value)
{
	ES=0;
	TI=0;
	SBUF=value;
	while(TI==0);
	TI=0;
	ES=1;
}
/*************接受数据***************************************/
void UARTInterrupt(void)interrupt 4
{		   		 
  			  


	if(RI)
	{
		RI=0;
		Usart_Receive[Usart_Cnt]=SBUF;
		Usart_Cnt++;
		
	//	if(Usart_Receive[Usart_Cnt-2]=='e'&&Usart_Receive[Usart_Cnt-1]=='d'&&Usart_Cnt>=2)
	   	if(flag_set=='t')
		{				   
			if(Usart_Receive[Usart_Cnt-2]=='\r'&&Usart_Receive[Usart_Cnt-1]=='\n'&&Usart_Cnt>=2)
			{
				Usart_Cnt=0;							  
				Usart_AT_flage=1;		  		
			}
			else if(Usart_Cnt>20)
			{
				Usart_Cnt=0;
			}
		}
		else
		{
			if(Usart_Cnt>=2) 
			{
				Usart_Cnt=0;							  
				Usart_AT_flage=1;		  		
			}	
		}
	}
}
/*****************发送设置**********************************/
void ESP8266_Set(unsigned char *puf)
{
	while(*puf!='\0')
	{
		Send_Uart(*puf);
		puf++;
	}
}
/**************设置多连*************************************/
void ManyConnect_AP()
{									
	ESP8266_Set(RST);//返回一大溜，不用判断返回	
	delays();
	delays();  	 
	while(1)					 
	{
		ESP8266_Set(CWMODE2);//返回ok		   //发不了\r\n 所以导致一直收到AT+CWMODE=2（原命令）
		delays();
		if(Usart_AT_flage ==1)
		{		
			if(strstr(Usart_Receive, "OK") )
			{
				Usart_AT_flage = 0;
				break;
			}
		}
	}
	while (1)				   
	{
		ESP8266_Set(CIPMUX);//返回ok
		delays();
		if(Usart_AT_flage ==1)
		{
			if(strstr(Usart_Receive, "OK") )
			{
				Usart_AT_flage = 0;
				break;
			}
		}
	}
					   


	while (1)
	{				 
		ESP8266_Set(CIPSERVER);//返回ok，多了也返回ok	  
		delays(); 
		if(Usart_AT_flage ==1)
		{
			if(strstr(Usart_Receive, "OK") )
			{					   	
				Usart_AT_flage = 0;
				break;
			}

		}
	}
	flag_set='t'; 	 
}

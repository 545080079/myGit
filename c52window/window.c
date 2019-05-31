#include <reg52.h>
#include <dht11.h> //温湿度库函数
#include <adc0832.h> //AD转换库函数
#include <esp8266.h>

sbit menu=P2^5;	//主功能键	
sbit add=P2^4;//加键		
sbit dec=P2^3;// 减键
sbit open=P2^6;// 手动 开
sbit close=P2^7;// 手动 关	
sbit IR=P3^5; //红外感 应
sbit BEEP=P3^1;// 报警
#define MOTORSTEP P1 			//定义P1口为步进电机驱动端口	 

uchar data dsflj,kval,lofl,menusw,con,befl,zhenfl,fanfl,brigfl;// 各种变量标记 正反转标记
uint data sudu,dwbrigfl,humfl;  //电机速度 //光度下限/湿度标记
uchar data count0,second,zhenzhuan,fanzhuan,timeflg,opensw,closesw,keysw;//时钟变量 正反转 变量	
uint data adc0,adc1;
uint data upbrig,dwbrig,kwbrig,uphum,uptemp,upsmo; // 光控下限 温度上限
uint i;//esp8266

uint data hour,minit,secon,DSdat,VAldat ,time;//时钟变量

uint data ONhour,ONminit,ONsecon; // 开窗时间变量
uint data OFhour,OFminit,OFsecon; // 关窗时间变量
uchar U8T_data_H,U8RH_data_H;


#define Imax 14000    //晶振为11.0592时的取值
#define Imin 8000
#define Inum1 1450
#define Inum2 700 
#define Inum3 3000 

uchar idata f=0;
uchar idata Im[4]={0x00,0x00,0x00,0x00}; // 遥控码存储空间
uchar idata show[2]={0,0};  // 变量申明
ulong idata m,Tc;	  // 变量申明
uchar idata IrOK;		// 变量申明


void delay(uint z);// 开窗时间变量
void write_rvalue(); // 显示函数
void open8266();
void init2();


void delaymoto() 					//步进电机每一步间延迟函数
{  	
	uint y=sudu;	 //sudu越大->转动越慢
	while(y--);
}



uchar code FFW[]={0x01,0x03,0x02,0x06,0x04,0x0c,0x08,0x09}; //步进电机驱动值数组

uchar code REV[]={0x09,0x08,0x0c,0x04,0x06,0x02,0x03,0x01}; //步进电机驱动值数组

num1 [4] =    {0x00,0x00,0x00,0x00,};				
num2 [4] =    {0x00,0x00,0x00,0x00,};			 
num3 [4] =    {0x00,0x00,0x00,0x00,};		   
num4 [4] =    {0x00,0x00,0x00,0x00,};
num5 [4] =    {0x00,0x00,0x00,0x00,};	// AD转换变量组组









  void SETP_MOTOR_FFW()	 //反转
{
 		   uint i=0;

		for(i=0;i<8;i++) //8步一个循环
			{
				MOTORSTEP=FFW[i]; //取值赋给P1驱动口
				delaymoto(); 
			}

 }

void SETP_MOTOR_REV()  //正转
{
 

    uint i=0;

		for(i=0;i<8;i++) //8步一个循环
			{
				MOTORSTEP=REV[i]; //取值赋给P1驱动口
				delaymoto(); 
			}

}


void write_sfm() // 拆分 个位 十位显示

{	uchar shi ,ge;

	shi=DSdat/10;
	ge=DSdat%10;
	
	write_date(0x30+shi);
	write_date(0x30+ge);	


}







void keyscn()//按键扫描函数
{
  uchar num; //局部变量
	

	//在非设置模式时，加键按下可进入WIFI控制模式
//	if(keysw==1)//如果当前开关窗键可用
//	{
		if(add==0)	 //如果加键按下
		{
			//保存现场--begin
			TR1=0;
			ET0=0;
			ET1=0;

			TR2=0;
			ET2=0;
			IT1=0;

			TR0=0;
			EX1=0;
			//保存现场--end

			InitUART();//init timer1
			open8266();//一次命令测试	
				
			//恢复现场: begin
		   	TR1=0;//关闭TR1
			PCON=0x30;//倍率还原(地址0x87)	   ResetValue=00X1 0000		    debug 2019年4月9日 09:13:43
			SCON=0x00;//串口接收禁止(地址0x98)	  
			ES=0;//串口中断禁止
			EA=0;//中断关
		   	init2();//再次开启所有时钟，恢复待机模式	   
			//恢复现场: end
		}
//	}
	
					
	
	
					
					
if(keysw==1)
{
 if(opensw==1)
 {
if(open==0)	 // 如果open键被按下，手动开窗
	{
	  	  write_com(0x80);

		  	for(num=0;num<16;num++)//显示提示语
		{
			write_date(logo5[num]);
			delay(2);
		}


		  write_com(0x80+0x40);

		  	for(num=0;num<16;num++)//显示提示语
		{
			write_date(logo10[num]);
			delay(2);
		}



		 
		 
		 closesw=1;	// 已打开标记
	     delay(10);
		if(open==0)				  //防抖，确认open键被按下
		{
		while(!open);  			//待open键被放开，解除阻塞
		zhenzhuan=1;// 正转 开启
		fanzhuan=0;	 // 返转关闭
		second=0;// 秒清零 步进电机 会停
		keysw=0;
		menusw=0;
		 }
      	 
		} 
	 
 }


  if(closesw==1)//如果窗户是打开的
  {
 if(close==0)//按下手动关
	{

	   write_com(0x80);

		  	for(num=0;num<16;num++)//显示提示语
		{
			write_date(logo6[num]);
			delay(2);
		}

		 	  write_com(0x80+0x40);

		  	for(num=0;num<16;num++)
		{
			write_date(logo10[num]);//显示提示语
			delay(2);
		}


		 
	     delay(10);
		if(close==0)			 //如果close键被按下
		{
		while(!close);								//待close键被放开
		zhenzhuan=0;  //正转关
		fanzhuan=1;	// 反转 开启
		second=0;  	// 定时清零
		keysw=0; // 标记请零
		 menusw=0;
		 }
      	 
		} 

 }

}


 if(menusw==1)//如果窗户运行完毕
{
  if(menu==0)// 如果主功能键menu被按下
	{

		lofl=1;	//字幕载入

		

	     delay(10);	//延时消抖动
		if(menu==0)
		{
		while(!menu);//松手检测
		 dsflj=0;// 关主页显示
		 kval++; // 按键变量++
		 keysw=0;//不允许手动 开窗  和关窗
		 }
		 } 

		 

//*************************************************************************8

  if(kval==1)  // 如果变量==1  进入时间设定

 {
  
  if(lofl==1)
  {
 write_com(0x80);

		  	for(num=0;num<16;num++)
		{
			write_date(logo12[num]);//显示提示语
			delay(2);

		
		}



	   write_com(0x80+0x40);

		  	for(num=0;num<16;num++)	//显示提示语
		{
			write_date(logo9[num]);
			delay(2);

			lofl=0;
		}




	   	DSdat=hour;
        write_com(0x80+8);	// 刷新 设定时间
		write_sfm();


       
		DSdat=minit;
	    write_com(0x80+11);// 刷新 设定时间
	    write_sfm();

		 DSdat=secon;
		write_com(0x80+14);	// 刷新 设定时间
		write_sfm();



}


 }



//uint hour,minit,secon;

   if(kval==2)//如果变量=2 进入小时设定

 {DSdat=hour;	  // 刷新显示小时
  
  
  write_com(0x40+0x40+8);// 显示坐标
  write_com(0x0e);
					   // 开关标

  
if(add==0)// 如果加键按下

{

while(!add); // 松手检测
 

hour++;	//小时++

DSdat=hour; //送显示
write_sfm(); // 拆分
if(hour==24)  //
{hour=0;}



}





 if( dec==0) // 如果减键按下

{

while(! dec);// 松手检测
 

hour--;	 //小时--
DSdat=hour;
write_sfm(); //拆分限位
if(hour==0)//限位
{hour=23;}




}

 }







 //**************************以下时 分钟设 定 秒钟设定 和上面小时一样
 //只是就量不一样 不作注释*******************************************
 //********************************************************************
 
   if(kval==3)

 {


DSdat=minit;

  write_com(0x40+0x40+11);
  write_com(0x0e);


  
if(add==0)

{

while(!add);
 

minit++;

DSdat=minit;
write_sfm();
if(minit==60)
{minit=0;}



}





 if( dec==0)

{

while(! dec);
 

minit--;

DSdat=minit;
write_sfm();
if(minit==0)
{minit=59;}



}

 }













 
   if(kval==4)

 { 
 
 
 DSdat=secon;
  
  write_com(0x40+0x40+14);
  write_com(0x0e);


  
if(add==0)

{

while(!add);
 

secon++;

DSdat=secon;
write_sfm();
if(secon==60)
{secon=0;}



}





 if( dec==0)

{

while(! dec);
 

secon--;

DSdat=secon;
write_sfm();
if(secon==0)
{secon=59;}



}

 }


//uint ONhour,ONminit,ONsecon;
//uint OFhour,OFminit,OFsecon;

 

 //**************************以下 开窗时  小时 分钟设 定 秒钟设定 和上面小时一样
 //只是就量不一样 不作注释*******************************************
 //**************

//*************************************设定开时****************************************
 
   if(kval==5)

 {
  
   if(lofl==1)
  {
 write_com(0x80);

		  	for(num=0;num<16;num++)
		{
			write_date(logo13[num]);
			delay(2);

		
		}



	   write_com(0x80+0x40);

		  	for(num=0;num<16;num++)
		{
			write_date(logo9[num]);
			delay(2);

		
		}



		DSdat=ONhour;
        write_com(0x80+8);
		write_sfm();


	  	DSdat=ONminit;
	    write_com(0x80+11);
	    write_sfm();


       DSdat=ONsecon;
		write_com(0x80+14);
		write_sfm();

	
		


	  	lofl=0;





}





}

 


  if(kval==6)

 {
  
  write_com(0x80+0x08);
  write_com(0x0e);


  
if(add==0)

{

while(!add);
 

ONhour++;

DSdat=ONhour;
write_sfm();
if(ONhour==24)
{ONhour=0;}



}





 if( dec==0)

{

while(! dec);
 

ONhour--;

DSdat=ONhour;
write_sfm();
if(ONhour==0)
{ONhour=23;}



}

 }






   if(kval==7)

 {
  
  write_com(0x40+0x40+11);
  write_com(0x0e);


  
if(add==0)

{

while(!add);
 

ONminit++;

DSdat=ONminit;
write_sfm();
if(ONminit==60)
{ONminit=0;}



}





 if( dec==0)

{

while(! dec);
 

ONminit--;

DSdat=ONminit;
write_sfm();
if(ONminit==0)
{ONminit=59;}



}

 }






  if(kval==8)

 {
  
  write_com(0x40+0x40+14);
  write_com(0x0e);


  
if(add==0)

{

while(!add);
 

ONsecon++;

DSdat=ONsecon;
write_sfm();
if(ONsecon==60)
{ONsecon=0;}



}





 if( dec==0)

{

while(! dec);
 

ONsecon--;

DSdat=ONsecon;
write_sfm();
if(ONsecon==0)
{ONsecon=59;}



}

 }
 

 //**************************以下 关窗时  小时 分钟设 定 秒钟设定 和上面小时一样
 //只是就量不一样 不作注释*******************************************
 //**************


//**************************************设定关时间**************************************
	
	
 
   if(kval==9)

 {
  
   if(lofl==1)
  {
 write_com(0x80);

		  	for(num=0;num<16;num++)
		{
			write_date(logo14[num]);
			delay(2);

		
		}



	   write_com(0x80+0x40);

		  	for(num=0;num<16;num++)
		{
			write_date(logo9[num]);
			delay(2);

			lofl=0;
		}


		DSdat=OFhour;
        write_com(0x80+8);
		write_sfm();


		DSdat=OFminit;
	    write_com(0x80+11);
	    write_sfm();

		

        DSdat=OFsecon;
		write_com(0x80+14);
		write_sfm();

		

}





}	
	
	
	
	
	
	
	
	
		
 if(kval==10)

 {
  
  write_com(0x80+0x08);
  write_com(0x0e);


  
if(add==0)

{

while(!add);
 

OFhour++;

DSdat=OFhour;
write_sfm();
if(OFhour==24)
{OFhour=0;}



}





 if( dec==0)

{

while(! dec);
 

OFhour--;

DSdat=OFhour;
write_sfm();
if(OFhour==0)
{OFhour=23;}



}

 }






   if(kval==11)

 {
  
  write_com(0x40+0x40+11);
  write_com(0x0e);


  
if(add==0)

{

while(!add);
 

OFminit++;

DSdat=OFminit;
write_sfm();
if(OFminit==60)
{OFminit=0;}



}





 if( dec==0)

{

while(! dec);
 

OFminit--;

DSdat=OFminit;
write_sfm();
if(OFminit==0)
{OFminit=59;}



}

 }












  if(kval==12)

 {
  
  write_com(0x40+0x40+14);
  write_com(0x0e);


  
if(add==0)

{

while(!add);
 

OFsecon++;

DSdat=OFsecon;
write_sfm();
if(OFsecon==60)
{OFsecon=0;}



}





 if( dec==0)

{

while(! dec);
 

OFsecon--;

DSdat=OFsecon;
write_sfm();
if(OFsecon==0)
{OFsecon=59;}



}

 }










 

 //**************************以下 亮度会上设定  和上面小时一样
 //只是就量不一样 不作注释*******************************************
 //**************


  //********************************设定环境参数************************************
 
  if(kval==13)

 {

   write_com(0x0c);

   num4[4]=dwbrig;
   write_rvalue();
  if(lofl==1)
  {
 write_com(0x80);

		  	for(num=0;num<16;num++)
		{
			write_date(logo7[num]);
			delay(2);

		
		}



	   write_com(0x80+0x40);

		  	for(num=0;num<16;num++)
		{
			write_date(logo9[num]);
			delay(2);

			lofl=0;
		}


}


    if(add==0)	
	{
	   delay(10);

	    
		if(add==0)
		{
		while(!add);						                
		 dwbrig++;
		 num4[4]=dwbrig;
		 write_rvalue();


         if(dwbrig==250)
          {dwbrig=0;}


		 }
		 } 



		if(dec==0)	
	{
	   delay(10);

	    
		if(dec==0)
		{
		while(!dec);						                
		 dwbrig--;
		 num4[4]=dwbrig;
		 write_rvalue();


         if(dwbrig==0)
          {dwbrig=250;}


		 }
		 } 


 }











//*********************************************************************


   //********************************设定环境参数************************************
 
  if(kval==14)

 {

   write_com(0x0c);

   num4[4]=kwbrig;
   write_rvalue();
  if(lofl==1)
  {
 write_com(0x80);

		  	for(num=0;num<16;num++)
		{
			write_date(logo16[num]);
			delay(2);

		
		}



	   write_com(0x80+0x40);

		  	for(num=0;num<16;num++)
		{
			write_date(logo9[num]);
			delay(2);

			lofl=0;
		}


}


    if(add==0)	
	{
	   delay(10);

	    
		if(add==0)
		{
		while(!add);						                
		 kwbrig++;
		 num4[4]=kwbrig;
		 write_rvalue();


         if(kwbrig==250)
          {kwbrig=0;}


		 }
		 } 



		if(dec==0)	
	{
	   delay(10);

	    
		if(dec==0)
		{
		while(!dec);						                
		 kwbrig--;
		 num4[4]=kwbrig;
		 write_rvalue();


         if(kwbrig==0)
          {kwbrig=250;}


		 }
		 } 


 }













 
 //**************************以下 湿度会上设定  和上面小时一样
 //只是就量不一样 不作注释*******************************************
 //**************		
 if(kval==15)

 {
   num4[4]=uphum;
   write_rvalue();
  if(lofl==1)
  {
 write_com(0x80);

		  	for(num=0;num<16;num++)
		{
			write_date(logo8[num]);
			delay(2);

		
		}



	   write_com(0x80+0x40);

		  	for(num=0;num<16;num++)
		{
			write_date(logo9[num]);
			delay(2);

			lofl=0;
		}


}


	   

	  




    if(add==0)	
	{
	   delay(10);

	    
		if(add==0)
		{
		while(!add);						                
		 uphum++;
		 num4[4]=uphum;
		 write_rvalue();


         if(uphum==100)
          {uphum=0;}


		 }
		 } 



		if(dec==0)	
	{
	   delay(10);

	    
		if(dec==0)
		{
		while(!dec);						                
		 uphum--;
		 num4[4]=uphum;
		 write_rvalue();


         if(uphum==0)
          {uphum=100;}


		 }
		 } 


 }





 
 //**************************以下 温度度会上设定  和上面小时一样
 //只是就量不一样 不作注释*******************************************
 //**************				
 if(kval==16)

 {
   num4[4]=uptemp;
   write_rvalue();
  if(lofl==1)
  {
 write_com(0x80);

		  	for(num=0;num<16;num++)
		{
			write_date(logo11[num]);
			delay(2);

		
		}



	   write_com(0x80+0x40);

		  	for(num=0;num<16;num++)
		{
			write_date(logo9[num]);
			delay(2);

			lofl=0;
		}


}


	   

	  




    if(add==0)	
	{
	   delay(10);

	    
		if(add==0)
		{
		while(!add);						                
		 uptemp++;
		 num4[4]=uptemp;
		 write_rvalue();


         if(uptemp==100)
          {uptemp=0;}


		 }
		 } 



		if(dec==0)	
	{
	   delay(10);

	    
		if(dec==0)
		{
		while(!dec);						                
		 uptemp--;
		 num4[4]=uptemp;
		 write_rvalue();


         if(uptemp==0)
          {uptemp=100;}


		 }
		 } 


 }





 

 //**************************以下烟雾度会上设定  和上面小时一样
 //只是就量不一样 不作注释*******************************************
 //**************				
 if(kval==17)

 {
   num4[4]=upsmo;
   write_rvalue();
  if(lofl==1)
  {
 write_com(0x80);

		  	for(num=0;num<16;num++)
		{
			write_date(logo15[num]);
			delay(2);

		
		}



	   write_com(0x80+0x40);

		  	for(num=0;num<16;num++)
		{
			write_date(logo9[num]);
			delay(2);

			lofl=0;
		}


}


	   

	  




    if(add==0)	
	{
	   delay(10);

	    
		if(add==0)
		{
		while(!add);						                
		 upsmo++;
		 num4[4]=upsmo;
		 write_rvalue();


         if(upsmo==100)
          {upsmo=0;}


		 }
		 } 



		if(dec==0)	
	{
	   delay(10);

	    
		if(dec==0)
		{
		while(!dec);						                
		 upsmo--;
		 num4[4]=upsmo;
		 write_rvalue();


         if(uptemp==0)
          {upsmo=100;}


		 }
		 } 


 }












 


   
 
 //*****退出设定*********		


  if(kval==18)

 {



  
   write_com(0x0c);
 

kval=0;
  
dsflj=1;
lofl=1;
keysw=1;
  if(lofl==1)
  {
 write_com(0x80);

		  	for(num=0;num<16;num++)
		{
			write_date(logo4[num]);//字幕显示
			delay(2);
		}


	   write_com(0x80+0x40);

		  	for(num=0;num<16;num++)//字幕显示
		{
			write_date(logo3[num]);
			delay(2);
		}




	   lofl=0;

	  }






  }





 }










 }


	
//步进电机函数
 void setmoto()
 {
  char num;
 //zhenfl fanfl

   if(zhenzhuan==1)	 //正转
	   {
		 dsflj=0;
		  SETP_MOTOR_REV();
		 
		if(second==4)//4s则停转，打开主页面显示
		{
		zhenzhuan=0;
		P1=0X00;
		dsflj=1;
		opensw=0;
		closesw=1;
		keysw=1;
	   menusw=1;


	  	zhenfl=1;
		fanfl=0;
		brigfl=0;
		 humfl=0;


		write_com(0x80);
		  	for(num=0;num<16;num++)
		{						   //字幕显示
			write_date(logo4[num]);
			delay(2);
		}


			write_com(0x80+0x40);	//字幕显示
		  	for(num=0;num<16;num++)
		{
			write_date(logo3[num]);
			delay(2);
		}



		}


	

	   }
	
	  	

		
     if(fanzhuan==1)  //反转
	   { dsflj=0;
		
		  SETP_MOTOR_FFW();
		 
		if(second==4)
		{ second=0;
		  	P1=0X00;

	    	zhenfl=0;
	     	fanfl=1;	
   
		 fanzhuan=0;
		 dsflj=1;
		 
		 dwbrigfl=0;
		 brigfl=0;


		opensw=1;
		closesw=0;
		keysw=1;
		menusw=1;
	    humfl=1;
	  
		    

		write_com(0x80);

		  	for(num=0;num<16;num++)	 //字幕显示
		{
			write_date(logo4[num]);
			delay(2);
		}

				write_com(0x80+0x40);
		  	for(num=0;num<16;num++)
		{							  //字幕显示
			write_date(logo3[num]);
			delay(2);
		}


		 
		}



		



	   }
		
 
 }
 
void TEMPHRDSPLAY()		    //显示空气温湿度函数   写入1602
{  
    num1[4]=U8T_data_H;		//温度高8位，即是整数部分  
    num1[1]=num1[4]/10;	//整数十位
    num1[0]=num1[4]%10;	//整数个位
	
    write_com(0x80+0x40+7);				 //显示温度
	write_date(0x30+num1[1]);
	write_date(0x30+num1[0]);





	num2[4]=U8RH_data_H; 	//湿度高8位，即是整数部分  
    num2[3]=num2[4]/10;	//整数十位
    num2[2]=num2[4]%10;	//整数个位

    write_com(0x80+0x40+13);			 //显示湿度
	write_date(0x30+num2[3]);
	write_date(0x30+num2[2]);

}	


  
 


 
void SMOGDSPLAY()		    //显示烟雾浓度函数   写入1602
{  
    num3[4]=adc0;		//温度高8位，即是整数部分 
	 

    
	 
    num3[0]=num3[4]%10;         //显示ge位  
	num3[1]=num3[4]%100/10;   //显示shi位
	num3[2]=num3[4]%1000/100;//显示百位
	
    write_com(0x80+0X40+2);				 
	write_date(0x30+num3[2]);
	write_date(0x30+num3[1]);
	write_date(0x30+num3[0]);

 }

  








void BRDSPLAY()	
{  
    
	 
	num5[4]=adc1;		//温度高8位，即是整数部分 
    
	 
    num5[0]=num5[4]%10;         //显示ge位  
	num5[1]=num5[4]%100/10;   //显示shi位
	num5[2]=num5[4]%1000/100;//显示百位

    write_com(0x80+3);			
	write_date(0x30+num5[2]);
	write_date(0x30+num5[1]);
	write_date(0x30+num5[0]);

  }

  

void write_rvalue()

{    

    
	 
    num4[0]=num4[4]%10;         //显示ge位  
	num4[1]=num4[4]%100/10;   //显示shi位
	num4[2]=num4[4]%1000/100;//显示百位



    write_com(0x80+7);				 //显示温度
	write_date(0x30+num4[2]);
	write_date(0x30+num4[1]);
	write_date(0x30+num4[0]);
 }







void init()
{

	TH0=0X3C;					//定时器0置初值 0.05S
	TL0=0XBA;

	ET0=1;						//定时器0中断开启
	TR0=1;						//启动定时0

	RCAP2H =(0xFFFF-50000)/256;                //赋T2的预置值0x1000，溢出30次就是1秒钟
    RCAP2L =(0xFFFF-50000)%256;   
    TR2=1;                       //启动定时器
    ET2=1;   


    IT1=1;
	TH0=0;	//定时器0初值
	TL0=0;	//定时器0初值
    TR0=1;	//定时器0启动	   
	EX1=1;	//外部中断 软件没用到


	
}



void init2()
{	
 	TMOD=0X11;	   				//定时器设置
	TH0=0X3C;					//定时器0置初值 0.05S
	TL0=0XBA;
	EA=1;						//开总中断
    ES=0;//禁止串口中断		debug	2019年4月9日 15:07:13
   
	ET0=1;						//定时器0中断开启
	TR0=1;						//启动定时0


  	TL1=0X3C;		//定时初值
	TH1=0XBA;		//定时初值
			 //总中断打开
	TR1=1;		 //启动定时器
	ET1=1;		 //软件没用到
	


	RCAP2H =(0xFFFF-50000)/256;                //赋T2的预置值0x1000，溢出30次就是1秒钟
    RCAP2L =(0xFFFF-50000)%256;   
    TR2=1;                       //启动定时器
    ET2=1;   


    IT1=1;

	TH0=0;	//定时器0初值  !
	TL0=0;	//定时器0初值	!

    TR0=1;	//定时器0启动	   
	EX1=1;	//外部中断 软件没用到
		  
    ES=0;//禁止串口中断		debug	2019年4月9日 15:07:13
   

}
  

void open8266()	//esp8266.h封装	一次命令
{		 
		ManyConnect_AP(); 


						write_com(0x80);
						for(num=0;num<16;num++)
						{
							write_date(waitMess[num]);//显示提示
							delay(2);	   
						}
						delays();
									
		   		  
					//test open win	   
					while(1)
					{
							
					   if((Usart_Receive[0]=='+')&&(Usart_Receive[1]=='I')&&(Usart_Receive[2]=='P'))
					   {		 
		 			   	      
						  	  write_com(0x80);
					
							  	for(num=0;num<16;num++)//显示提示语
							{
								write_date(logo5[num]);
								delay(2);
							}
					
					
							  write_com(0x80+0x40);
					
							  	for(num=0;num<16;num++)//显示提示语
							{
								write_date(logo10[num]);
								delay(2);
							}

							closesw=1;	// 已打开标记	  
						    delay(10);
							zhenzhuan=1;// 正转 开启
							fanzhuan=0;	 // 返转关闭
							second=0;// 秒清零 步进电机 会停
							keysw=0;
							menusw=0;
							
							 break;
					   }
					}		   
					//end test

						for(i = 0 ; i<20; i++)
						{
							Usart_Receive[i]=' ';
						}

}


void main()//主函数
{
	//kaiguan=0;
	init2(); //初始化所有时钟 
	init1602();	   



  sudu=200;	//电机速度
  zhenzhuan=0;//上电关正转
  fanzhuan=1;//上电反转打开
  dsflj=1;
			// 显示打开

  uptemp=38; //温度上限
  uphum=80;	 //湿度上限


  upbrig=120;
  dwbrig=10;		//光控下限
  kwbrig=180;	 	// 光控上限
  upsmo=85;//烟雾浓度上限

  opensw=1;
  closesw=1;
  keysw=1;
  menusw=1;
  second=0;
 
        	zhenfl=0;
	     	fanfl=0;


			brigfl=1;
			dwbrigfl=1;
			humfl=0;

 adc0 = ADC0832(1,0);  //差分模式，CH0-CH1
 adc1 = ADC0832(1,1);  //差分模式，CH0-CH1




 hour=12;
 minit=5;
 secon=10; // 系统时间


ONhour=12;	// 自动开窗时间  为了方便演示 设定 一分钟后开
ONminit=6;
ONsecon=10;


OFhour=12;	// 自动关窗时间 为了方便演示 设定 一分钟后关
OFminit=7;
OFsecon=10;



	while(1)//大循环

	{
  
keyscn();
setmoto();
											  


if (dsflj==1 )
{

 //get_temp(); //SHT10调用
//cal_wet();//SHT10调用

RH(); //DHT11 调用
   
 //  U8T_data_H= SENSOR[k].temp;//SHT10调用
 //  U8RH_data_H=SENSOR[k].wet;//SHT10调用	 
	 
	 
TEMPHRDSPLAY(); 

								 
										
 

 




 
adc0 = ADC0832(1,0);  //差分模式，CH0-CH1 
SMOGDSPLAY();

adc1 = ADC0832(1,1);  //差分模式，CH0-CH1
BRDSPLAY(); 
 







 	if(fanfl==1)
	{


     if(adc1> kwbrig )//如果际亮度大于设定值kwbrig(默认180) 关窗
 {
								  
  zhenzhuan=1;


} 
   
   
   
   
   
   	 
   if(Im[2]==0x40)
	 
{
 Im[2]=0x00;
zhenzhuan=1;

}




   second=0;

// if(U8T_data_H>uptemp )//如果际温度大与设定值 开窗  		  -------debug 2019年4月9日 15:23:32 温度检测异常
// {

// zhenzhuan=1;
 
// }
 


 if(adc0>upsmo )//如果烟雾实际值大于设定值 开窗
 {

 zhenzhuan=1;
 
 }


  













}

	if(zhenfl==1)
	{

	   if(Im[2]==0x44)
	 
{
Im[2]=0x00;
fanzhuan=1;

}


	second=0;
// if(U8RH_data_H>uphum )//如果际湿度大与设定值 关窗		 -------debug 2019年4月9日 15:23:32 湿度检测异常
// {
//
// fanzhuan=1;
//  brigfl=0;
//  dwbrigfl=0;
//  
//  }


    if(adc1<dwbrig )//如果际亮度小与设定值 关窗
 {
										   

fanzhuan=1;
}
  





  


 
if(IR==0)	
	{
	   delay(10);

	    
		if(IR==0)
		{
		//while(!IR);	
		  befl=1;
		   ET2=1; 
		  fanzhuan=1;
		  con=0;
		 					                
		 } 

		  


	   }




 



  


}




 


 
	 		if(dec==0)	
	{
	   delay(10);

	    
		if(dec==0)
		{
		while(!dec);						                
		 befl=0;
		 BEEP=1; 
		  

		 } 


	   }
   


		    	
		







        DSdat=secon;
		write_com(0x80+14);
		write_sfm();

		DSdat=minit;
	    write_com(0x80+11);
	    write_sfm();

		DSdat=hour;
        write_com(0x80+0x08);
		write_sfm();














}


}


}
 







 
//定时器2中断
timer2() interrupt 5 
{
    TF2=0; //!!!注意!!! 定时器2必须由软件对溢出标志位清零，硬件不能清零，这里与定时器0和定时器1不同!!!
 

	count0++;
	con++;

  	if(count0==15)
   	{
		count0=0;
    	second++;
	}	
		
			
		
     if(befl==1)
   {
		if(con==20)
   	{
		con=0;
    	//BEEP=~BEEP;  消音
		
		
			
	}	
    
	}
 
   	



}
   



  void time0(void) interrupt 1 using 1  	//定时中断子程序
{
	TH0=0X3C;							//重赋初值
	TL0=0XBA;

  




}

                                                                            



  //*********************************中断服务函数**************************************
void  time0_int(void) interrupt 3 
{

  
 	TH0=0X3C;							//重赋初值
	TL0=0XBA;		//定时初值
 	TR1=1;
 	time++;
 
   	   
  	if(time==20)
   	{
		time=0;
    	secon++;	
	   	
	    
	   


	}
	
		
    	if(secon==60)
   	{
		secon=0;
    	minit++;
	
			
			
	}	
    
   	
 	if(minit==60)
   	{
		minit=0;
    	hour++;	
			if(hour==24)
   	{
		hour=0;
    }	

 	  
		

	    
	}	


	






 
   	if(fanfl==1)
	{

 



  if((ONhour==hour)&&(ONminit==minit)&&(ONsecon==secon))//定时开窗
  {
			


 zhenzhuan=1;

 }
		 
}






	if(zhenfl==1)
	{



  if((hour==OFhour)&&(minit==OFminit)&&(secon==OFsecon)) //定时关窗
  {
			


 fanzhuan=1;




}
  

}












 






}




//*********************************中断服务函数**************************************
// 遥控 解码 函数 接收头 接外部 中断口 P3.3
/************************************************************************/	
//外部中断解码程序
void intersvr1(void) interrupt 2 using 1
{
    Tc=TH0*256+TL0; //定时器清空只记TC 值                                              //提取中断时间间隔时长
    TH0=0; 
    TL0=0;              //定时中断重新置零

	if((Tc>Imin)&&(Tc<Imax))//启动码判定 两个时间对比0.9MS	
	{  
        m=0;
        f=1;
        return;
	}       //找到启始码
	
	if(f==1)
	{
        if(Tc>Inum1&&Tc<Inum3) //判定是0该还是1 是0 1.125MS 是1  2.25MS
		{
			Im[m/8]=Im[m/8]>>1|0x80; m++;  //存0
		}
		if(Tc>Inum2&&Tc<Inum1) 
        {
			Im[m/8]=Im[m/8]>>1; m++; //	存1
		}
		if(m==32) 	//32位存储完毕
		{
			m=0;  
			f=0;
			if(Im[2]==~Im[3])  //反码检查
			{	 
				IrOK=1; 	//OK 完成
			}
			else IrOK=0;   //取码完成后判断读码是否正确
		}
		//准备读下一码
	}
	
}
/************************************************************************/
//************zheng 0X45; stop  0X46; fan  0X47   aut  0X43;

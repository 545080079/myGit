#include <reg52.h>
#include <dht11.h> //��ʪ�ȿ⺯��
#include <adc0832.h> //ADת���⺯��
#include <esp8266.h>

sbit menu=P2^5;	//�����ܼ�	
sbit add=P2^4;//�Ӽ�		
sbit dec=P2^3;// ����
sbit open=P2^6;// �ֶ� ��
sbit close=P2^7;// �ֶ� ��	
sbit IR=P3^5; //����� Ӧ
sbit BEEP=P3^1;// ����
#define MOTORSTEP P1 			//����P1��Ϊ������������˿�	 

uchar data dsflj,kval,lofl,menusw,con,befl,zhenfl,fanfl,brigfl;// ���ֱ������ ����ת���
uint data sudu,dwbrigfl,humfl;  //����ٶ� //�������/ʪ�ȱ��
uchar data count0,second,zhenzhuan,fanzhuan,timeflg,opensw,closesw,keysw;//ʱ�ӱ��� ����ת ����	
uint data adc0,adc1;
uint data upbrig,dwbrig,kwbrig,uphum,uptemp,upsmo; // ������� �¶�����
uint i;//esp8266

uint data hour,minit,secon,DSdat,VAldat ,time;//ʱ�ӱ���

uint data ONhour,ONminit,ONsecon; // ����ʱ�����
uint data OFhour,OFminit,OFsecon; // �ش�ʱ�����
uchar U8T_data_H,U8RH_data_H;


#define Imax 14000    //����Ϊ11.0592ʱ��ȡֵ
#define Imin 8000
#define Inum1 1450
#define Inum2 700 
#define Inum3 3000 

uchar idata f=0;
uchar idata Im[4]={0x00,0x00,0x00,0x00}; // ң����洢�ռ�
uchar idata show[2]={0,0};  // ��������
ulong idata m,Tc;	  // ��������
uchar idata IrOK;		// ��������


void delay(uint z);// ����ʱ�����
void write_rvalue(); // ��ʾ����
void open8266();
void init2();


void delaymoto() 					//�������ÿһ�����ӳٺ���
{  	
	uint y=sudu;	 //suduԽ��->ת��Խ��
	while(y--);
}



uchar code FFW[]={0x01,0x03,0x02,0x06,0x04,0x0c,0x08,0x09}; //�����������ֵ����

uchar code REV[]={0x09,0x08,0x0c,0x04,0x06,0x02,0x03,0x01}; //�����������ֵ����

num1 [4] =    {0x00,0x00,0x00,0x00,};				
num2 [4] =    {0x00,0x00,0x00,0x00,};			 
num3 [4] =    {0x00,0x00,0x00,0x00,};		   
num4 [4] =    {0x00,0x00,0x00,0x00,};
num5 [4] =    {0x00,0x00,0x00,0x00,};	// ADת����������









  void SETP_MOTOR_FFW()	 //��ת
{
 		   uint i=0;

		for(i=0;i<8;i++) //8��һ��ѭ��
			{
				MOTORSTEP=FFW[i]; //ȡֵ����P1������
				delaymoto(); 
			}

 }

void SETP_MOTOR_REV()  //��ת
{
 

    uint i=0;

		for(i=0;i<8;i++) //8��һ��ѭ��
			{
				MOTORSTEP=REV[i]; //ȡֵ����P1������
				delaymoto(); 
			}

}


void write_sfm() // ��� ��λ ʮλ��ʾ

{	uchar shi ,ge;

	shi=DSdat/10;
	ge=DSdat%10;
	
	write_date(0x30+shi);
	write_date(0x30+ge);	


}







void keyscn()//����ɨ�躯��
{
  uchar num; //�ֲ�����
	

	//�ڷ�����ģʽʱ���Ӽ����¿ɽ���WIFI����ģʽ
//	if(keysw==1)//�����ǰ���ش�������
//	{
		if(add==0)	 //����Ӽ�����
		{
			//�����ֳ�--begin
			TR1=0;
			ET0=0;
			ET1=0;

			TR2=0;
			ET2=0;
			IT1=0;

			TR0=0;
			EX1=0;
			//�����ֳ�--end

			InitUART();//init timer1
			open8266();//һ���������	
				
			//�ָ��ֳ�: begin
		   	TR1=0;//�ر�TR1
			PCON=0x30;//���ʻ�ԭ(��ַ0x87)	   ResetValue=00X1 0000		    debug 2019��4��9�� 09:13:43
			SCON=0x00;//���ڽ��ս�ֹ(��ַ0x98)	  
			ES=0;//�����жϽ�ֹ
			EA=0;//�жϹ�
		   	init2();//�ٴο�������ʱ�ӣ��ָ�����ģʽ	   
			//�ָ��ֳ�: end
		}
//	}
	
					
	
	
					
					
if(keysw==1)
{
 if(opensw==1)
 {
if(open==0)	 // ���open�������£��ֶ�����
	{
	  	  write_com(0x80);

		  	for(num=0;num<16;num++)//��ʾ��ʾ��
		{
			write_date(logo5[num]);
			delay(2);
		}


		  write_com(0x80+0x40);

		  	for(num=0;num<16;num++)//��ʾ��ʾ��
		{
			write_date(logo10[num]);
			delay(2);
		}



		 
		 
		 closesw=1;	// �Ѵ򿪱��
	     delay(10);
		if(open==0)				  //������ȷ��open��������
		{
		while(!open);  			//��open�����ſ����������
		zhenzhuan=1;// ��ת ����
		fanzhuan=0;	 // ��ת�ر�
		second=0;// ������ ������� ��ͣ
		keysw=0;
		menusw=0;
		 }
      	 
		} 
	 
 }


  if(closesw==1)//��������Ǵ򿪵�
  {
 if(close==0)//�����ֶ���
	{

	   write_com(0x80);

		  	for(num=0;num<16;num++)//��ʾ��ʾ��
		{
			write_date(logo6[num]);
			delay(2);
		}

		 	  write_com(0x80+0x40);

		  	for(num=0;num<16;num++)
		{
			write_date(logo10[num]);//��ʾ��ʾ��
			delay(2);
		}


		 
	     delay(10);
		if(close==0)			 //���close��������
		{
		while(!close);								//��close�����ſ�
		zhenzhuan=0;  //��ת��
		fanzhuan=1;	// ��ת ����
		second=0;  	// ��ʱ����
		keysw=0; // �������
		 menusw=0;
		 }
      	 
		} 

 }

}


 if(menusw==1)//��������������
{
  if(menu==0)// ��������ܼ�menu������
	{

		lofl=1;	//��Ļ����

		

	     delay(10);	//��ʱ������
		if(menu==0)
		{
		while(!menu);//���ּ��
		 dsflj=0;// ����ҳ��ʾ
		 kval++; // ��������++
		 keysw=0;//�������ֶ� ����  �͹ش�
		 }
		 } 

		 

//*************************************************************************8

  if(kval==1)  // �������==1  ����ʱ���趨

 {
  
  if(lofl==1)
  {
 write_com(0x80);

		  	for(num=0;num<16;num++)
		{
			write_date(logo12[num]);//��ʾ��ʾ��
			delay(2);

		
		}



	   write_com(0x80+0x40);

		  	for(num=0;num<16;num++)	//��ʾ��ʾ��
		{
			write_date(logo9[num]);
			delay(2);

			lofl=0;
		}




	   	DSdat=hour;
        write_com(0x80+8);	// ˢ�� �趨ʱ��
		write_sfm();


       
		DSdat=minit;
	    write_com(0x80+11);// ˢ�� �趨ʱ��
	    write_sfm();

		 DSdat=secon;
		write_com(0x80+14);	// ˢ�� �趨ʱ��
		write_sfm();



}


 }



//uint hour,minit,secon;

   if(kval==2)//�������=2 ����Сʱ�趨

 {DSdat=hour;	  // ˢ����ʾСʱ
  
  
  write_com(0x40+0x40+8);// ��ʾ����
  write_com(0x0e);
					   // ���ر�

  
if(add==0)// ����Ӽ�����

{

while(!add); // ���ּ��
 

hour++;	//Сʱ++

DSdat=hour; //����ʾ
write_sfm(); // ���
if(hour==24)  //
{hour=0;}



}





 if( dec==0) // �����������

{

while(! dec);// ���ּ��
 

hour--;	 //Сʱ--
DSdat=hour;
write_sfm(); //�����λ
if(hour==0)//��λ
{hour=23;}




}

 }







 //**************************����ʱ ������ �� �����趨 ������Сʱһ��
 //ֻ�Ǿ�����һ�� ����ע��*******************************************
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

 

 //**************************���� ����ʱ  Сʱ ������ �� �����趨 ������Сʱһ��
 //ֻ�Ǿ�����һ�� ����ע��*******************************************
 //**************

//*************************************�趨��ʱ****************************************
 
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
 

 //**************************���� �ش�ʱ  Сʱ ������ �� �����趨 ������Сʱһ��
 //ֻ�Ǿ�����һ�� ����ע��*******************************************
 //**************


//**************************************�趨��ʱ��**************************************
	
	
 
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










 

 //**************************���� ���Ȼ����趨  ������Сʱһ��
 //ֻ�Ǿ�����һ�� ����ע��*******************************************
 //**************


  //********************************�趨��������************************************
 
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


   //********************************�趨��������************************************
 
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













 
 //**************************���� ʪ�Ȼ����趨  ������Сʱһ��
 //ֻ�Ǿ�����һ�� ����ע��*******************************************
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





 
 //**************************���� �¶ȶȻ����趨  ������Сʱһ��
 //ֻ�Ǿ�����һ�� ����ע��*******************************************
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





 

 //**************************��������Ȼ����趨  ������Сʱһ��
 //ֻ�Ǿ�����һ�� ����ע��*******************************************
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












 


   
 
 //*****�˳��趨*********		


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
			write_date(logo4[num]);//��Ļ��ʾ
			delay(2);
		}


	   write_com(0x80+0x40);

		  	for(num=0;num<16;num++)//��Ļ��ʾ
		{
			write_date(logo3[num]);
			delay(2);
		}




	   lofl=0;

	  }






  }





 }










 }


	
//�����������
 void setmoto()
 {
  char num;
 //zhenfl fanfl

   if(zhenzhuan==1)	 //��ת
	   {
		 dsflj=0;
		  SETP_MOTOR_REV();
		 
		if(second==4)//4s��ͣת������ҳ����ʾ
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
		{						   //��Ļ��ʾ
			write_date(logo4[num]);
			delay(2);
		}


			write_com(0x80+0x40);	//��Ļ��ʾ
		  	for(num=0;num<16;num++)
		{
			write_date(logo3[num]);
			delay(2);
		}



		}


	

	   }
	
	  	

		
     if(fanzhuan==1)  //��ת
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

		  	for(num=0;num<16;num++)	 //��Ļ��ʾ
		{
			write_date(logo4[num]);
			delay(2);
		}

				write_com(0x80+0x40);
		  	for(num=0;num<16;num++)
		{							  //��Ļ��ʾ
			write_date(logo3[num]);
			delay(2);
		}


		 
		}



		



	   }
		
 
 }
 
void TEMPHRDSPLAY()		    //��ʾ������ʪ�Ⱥ���   д��1602
{  
    num1[4]=U8T_data_H;		//�¶ȸ�8λ��������������  
    num1[1]=num1[4]/10;	//����ʮλ
    num1[0]=num1[4]%10;	//������λ
	
    write_com(0x80+0x40+7);				 //��ʾ�¶�
	write_date(0x30+num1[1]);
	write_date(0x30+num1[0]);





	num2[4]=U8RH_data_H; 	//ʪ�ȸ�8λ��������������  
    num2[3]=num2[4]/10;	//����ʮλ
    num2[2]=num2[4]%10;	//������λ

    write_com(0x80+0x40+13);			 //��ʾʪ��
	write_date(0x30+num2[3]);
	write_date(0x30+num2[2]);

}	


  
 


 
void SMOGDSPLAY()		    //��ʾ����Ũ�Ⱥ���   д��1602
{  
    num3[4]=adc0;		//�¶ȸ�8λ�������������� 
	 

    
	 
    num3[0]=num3[4]%10;         //��ʾgeλ  
	num3[1]=num3[4]%100/10;   //��ʾshiλ
	num3[2]=num3[4]%1000/100;//��ʾ��λ
	
    write_com(0x80+0X40+2);				 
	write_date(0x30+num3[2]);
	write_date(0x30+num3[1]);
	write_date(0x30+num3[0]);

 }

  








void BRDSPLAY()	
{  
    
	 
	num5[4]=adc1;		//�¶ȸ�8λ�������������� 
    
	 
    num5[0]=num5[4]%10;         //��ʾgeλ  
	num5[1]=num5[4]%100/10;   //��ʾshiλ
	num5[2]=num5[4]%1000/100;//��ʾ��λ

    write_com(0x80+3);			
	write_date(0x30+num5[2]);
	write_date(0x30+num5[1]);
	write_date(0x30+num5[0]);

  }

  

void write_rvalue()

{    

    
	 
    num4[0]=num4[4]%10;         //��ʾgeλ  
	num4[1]=num4[4]%100/10;   //��ʾshiλ
	num4[2]=num4[4]%1000/100;//��ʾ��λ



    write_com(0x80+7);				 //��ʾ�¶�
	write_date(0x30+num4[2]);
	write_date(0x30+num4[1]);
	write_date(0x30+num4[0]);
 }







void init()
{

	TH0=0X3C;					//��ʱ��0�ó�ֵ 0.05S
	TL0=0XBA;

	ET0=1;						//��ʱ��0�жϿ���
	TR0=1;						//������ʱ0

	RCAP2H =(0xFFFF-50000)/256;                //��T2��Ԥ��ֵ0x1000�����30�ξ���1����
    RCAP2L =(0xFFFF-50000)%256;   
    TR2=1;                       //������ʱ��
    ET2=1;   


    IT1=1;
	TH0=0;	//��ʱ��0��ֵ
	TL0=0;	//��ʱ��0��ֵ
    TR0=1;	//��ʱ��0����	   
	EX1=1;	//�ⲿ�ж� ���û�õ�


	
}



void init2()
{	
 	TMOD=0X11;	   				//��ʱ������
	TH0=0X3C;					//��ʱ��0�ó�ֵ 0.05S
	TL0=0XBA;
	EA=1;						//�����ж�
    ES=0;//��ֹ�����ж�		debug	2019��4��9�� 15:07:13
   
	ET0=1;						//��ʱ��0�жϿ���
	TR0=1;						//������ʱ0


  	TL1=0X3C;		//��ʱ��ֵ
	TH1=0XBA;		//��ʱ��ֵ
			 //���жϴ�
	TR1=1;		 //������ʱ��
	ET1=1;		 //���û�õ�
	


	RCAP2H =(0xFFFF-50000)/256;                //��T2��Ԥ��ֵ0x1000�����30�ξ���1����
    RCAP2L =(0xFFFF-50000)%256;   
    TR2=1;                       //������ʱ��
    ET2=1;   


    IT1=1;

	TH0=0;	//��ʱ��0��ֵ  !
	TL0=0;	//��ʱ��0��ֵ	!

    TR0=1;	//��ʱ��0����	   
	EX1=1;	//�ⲿ�ж� ���û�õ�
		  
    ES=0;//��ֹ�����ж�		debug	2019��4��9�� 15:07:13
   

}
  

void open8266()	//esp8266.h��װ	һ������
{		 
		ManyConnect_AP(); 


						write_com(0x80);
						for(num=0;num<16;num++)
						{
							write_date(waitMess[num]);//��ʾ��ʾ
							delay(2);	   
						}
						delays();
									
		   		  
					//test open win	   
					while(1)
					{
							
					   if((Usart_Receive[0]=='+')&&(Usart_Receive[1]=='I')&&(Usart_Receive[2]=='P'))
					   {		 
		 			   	      
						  	  write_com(0x80);
					
							  	for(num=0;num<16;num++)//��ʾ��ʾ��
							{
								write_date(logo5[num]);
								delay(2);
							}
					
					
							  write_com(0x80+0x40);
					
							  	for(num=0;num<16;num++)//��ʾ��ʾ��
							{
								write_date(logo10[num]);
								delay(2);
							}

							closesw=1;	// �Ѵ򿪱��	  
						    delay(10);
							zhenzhuan=1;// ��ת ����
							fanzhuan=0;	 // ��ת�ر�
							second=0;// ������ ������� ��ͣ
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


void main()//������
{
	//kaiguan=0;
	init2(); //��ʼ������ʱ�� 
	init1602();	   



  sudu=200;	//����ٶ�
  zhenzhuan=0;//�ϵ����ת
  fanzhuan=1;//�ϵ練ת��
  dsflj=1;
			// ��ʾ��

  uptemp=38; //�¶�����
  uphum=80;	 //ʪ������


  upbrig=120;
  dwbrig=10;		//�������
  kwbrig=180;	 	// �������
  upsmo=85;//����Ũ������

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

 adc0 = ADC0832(1,0);  //���ģʽ��CH0-CH1
 adc1 = ADC0832(1,1);  //���ģʽ��CH0-CH1




 hour=12;
 minit=5;
 secon=10; // ϵͳʱ��


ONhour=12;	// �Զ�����ʱ��  Ϊ�˷�����ʾ �趨 һ���Ӻ�
ONminit=6;
ONsecon=10;


OFhour=12;	// �Զ��ش�ʱ�� Ϊ�˷�����ʾ �趨 һ���Ӻ��
OFminit=7;
OFsecon=10;



	while(1)//��ѭ��

	{
  
keyscn();
setmoto();
											  


if (dsflj==1 )
{

 //get_temp(); //SHT10����
//cal_wet();//SHT10����

RH(); //DHT11 ����
   
 //  U8T_data_H= SENSOR[k].temp;//SHT10����
 //  U8RH_data_H=SENSOR[k].wet;//SHT10����	 
	 
	 
TEMPHRDSPLAY(); 

								 
										
 

 




 
adc0 = ADC0832(1,0);  //���ģʽ��CH0-CH1 
SMOGDSPLAY();

adc1 = ADC0832(1,1);  //���ģʽ��CH0-CH1
BRDSPLAY(); 
 







 	if(fanfl==1)
	{


     if(adc1> kwbrig )//��������ȴ����趨ֵkwbrig(Ĭ��180) �ش�
 {
								  
  zhenzhuan=1;


} 
   
   
   
   
   
   	 
   if(Im[2]==0x40)
	 
{
 Im[2]=0x00;
zhenzhuan=1;

}




   second=0;

// if(U8T_data_H>uptemp )//������¶ȴ����趨ֵ ����  		  -------debug 2019��4��9�� 15:23:32 �¶ȼ���쳣
// {

// zhenzhuan=1;
 
// }
 


 if(adc0>upsmo )//�������ʵ��ֵ�����趨ֵ ����
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
// if(U8RH_data_H>uphum )//�����ʪ�ȴ����趨ֵ �ش�		 -------debug 2019��4��9�� 15:23:32 ʪ�ȼ���쳣
// {
//
// fanzhuan=1;
//  brigfl=0;
//  dwbrigfl=0;
//  
//  }


    if(adc1<dwbrig )//���������С���趨ֵ �ش�
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
 







 
//��ʱ��2�ж�
timer2() interrupt 5 
{
    TF2=0; //!!!ע��!!! ��ʱ��2����������������־λ���㣬Ӳ���������㣬�����붨ʱ��0�Ͷ�ʱ��1��ͬ!!!
 

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
    	//BEEP=~BEEP;  ����
		
		
			
	}	
    
	}
 
   	



}
   



  void time0(void) interrupt 1 using 1  	//��ʱ�ж��ӳ���
{
	TH0=0X3C;							//�ظ���ֵ
	TL0=0XBA;

  




}

                                                                            



  //*********************************�жϷ�����**************************************
void  time0_int(void) interrupt 3 
{

  
 	TH0=0X3C;							//�ظ���ֵ
	TL0=0XBA;		//��ʱ��ֵ
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

 



  if((ONhour==hour)&&(ONminit==minit)&&(ONsecon==secon))//��ʱ����
  {
			


 zhenzhuan=1;

 }
		 
}






	if(zhenfl==1)
	{



  if((hour==OFhour)&&(minit==OFminit)&&(secon==OFsecon)) //��ʱ�ش�
  {
			


 fanzhuan=1;




}
  

}












 






}




//*********************************�жϷ�����**************************************
// ң�� ���� ���� ����ͷ ���ⲿ �жϿ� P3.3
/************************************************************************/	
//�ⲿ�жϽ������
void intersvr1(void) interrupt 2 using 1
{
    Tc=TH0*256+TL0; //��ʱ�����ֻ��TC ֵ                                              //��ȡ�ж�ʱ����ʱ��
    TH0=0; 
    TL0=0;              //��ʱ�ж���������

	if((Tc>Imin)&&(Tc<Imax))//�������ж� ����ʱ��Ա�0.9MS	
	{  
        m=0;
        f=1;
        return;
	}       //�ҵ���ʼ��
	
	if(f==1)
	{
        if(Tc>Inum1&&Tc<Inum3) //�ж���0�û���1 ��0 1.125MS ��1  2.25MS
		{
			Im[m/8]=Im[m/8]>>1|0x80; m++;  //��0
		}
		if(Tc>Inum2&&Tc<Inum1) 
        {
			Im[m/8]=Im[m/8]>>1; m++; //	��1
		}
		if(m==32) 	//32λ�洢���
		{
			m=0;  
			f=0;
			if(Im[2]==~Im[3])  //������
			{	 
				IrOK=1; 	//OK ���
			}
			else IrOK=0;   //ȡ����ɺ��ж϶����Ƿ���ȷ
		}
		//׼������һ��
	}
	
}
/************************************************************************/
//************zheng 0X45; stop  0X46; fan  0X47   aut  0X43;

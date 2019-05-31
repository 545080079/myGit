#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int




sbit  date = P3^6;






typedef unsigned char  U8;       /* defined for unsigned 8-bits integer variable    无符号8位整型变量  */
typedef signed   char  S8;       /* defined for signed 8-bits integer variable    有符号8位整型变量  */
typedef unsigned int   U16;      /* defined for unsigned 16-bits integer variable    无符号16位整型变量 */


uchar display[5] =   {0x00,0x00,0x00,0x00,0x00};
uchar display0[5] =   {0x00,0x00,0x00,0x00,0x00};
uchar display1[5] =   {0x00,0x00,0x00,0x00,0x00};
uchar display2[5] =   {0x00,0x00,0x00,0x00,0x00};



//----------------定义区--------------------//
U8  U8FLAG;
U8  U8count,U8temp;
U8  U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
U8  U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;
U8  U8comdata; 









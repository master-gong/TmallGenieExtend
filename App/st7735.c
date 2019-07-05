/*
 * st7735.c
 *
 *  Created on: 2019年4月13日
 *      Author: gongjunhui
 */

#include "st7735.h"
//#include "GB2424.h"	//24*24汉字字模
#include "GB1616.h"	//16*16汉字字模
#include "GB1212.h"	//12*12汉字字模
#include "image.h"	//图片数据

/******************************************************************************
      函数说明：LCD串行数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
static void LCD_Writ_Bus(uint8_t dat)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;
	}
}
/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
static void LCD_WR_REG(uint8_t dat)
{
	OLED_DC_Clr();//写命令
	LCD_Writ_Bus(dat);
}
/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
static void LCD_WR_DATA8(uint8_t dat)
{
	OLED_DC_Set();//写数据
	LCD_Writ_Bus(dat);
}
/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
static void LCD_WR_DATA(uint16_t dat)
{
	OLED_DC_Set();//写数据
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}
/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
static void LCD_WR_DATA_FAST(uint8_t dat)
{
	uint8_t i;
	OLED_DC_Set();//写数据
	OLED_SDIN_Clr(); //高8位全写0
	for(i=0;i<8;i++){
		OLED_SCLK_Clr();
		OLED_SCLK_Set();
	}

	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();
		if(dat&0x80)
			OLED_SDIN_Set();
		else
			OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;
	}
}
/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：Xstart,Xend 设置列的起始和结束地址
         Ystart,Yend 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
static void LCD_Address_Set(uint8_t Xstart,uint8_t Ystart,uint8_t Xend,uint8_t Yend)
{
#if  USE_HORIZONTAL==0
	LCD_WR_REG(0x2a);//列地址设置
	LCD_WR_DATA(Xstart+26);
	LCD_WR_DATA(Xend+26);
	LCD_WR_REG(0x2b);//行地址设置
	LCD_WR_DATA(Ystart+1);
	LCD_WR_DATA(Yend+1);
	LCD_WR_REG(0x2c);//储存器写

#elif USE_HORIZONTAL==1
	LCD_WR_REG(0x2a);//列地址设置
	LCD_WR_DATA(Xstart+26);
	LCD_WR_DATA(Xend+26);
	LCD_WR_REG(0x2b);//行地址设置
	LCD_WR_DATA(Ystart+1);
	LCD_WR_DATA(Yend+1);
	LCD_WR_REG(0x2c);//储存器写

#elif USE_HORIZONTAL==2
	LCD_WR_REG(0x2a);//列地址设置
	LCD_WR_DATA(Xstart+1);
	LCD_WR_DATA(Xend+1);
	LCD_WR_REG(0x2b);//行地址设置
	LCD_WR_DATA(Ystart+26);
	LCD_WR_DATA(Yend+26);
	LCD_WR_REG(0x2c);//储存器写

#else
	LCD_WR_REG(0x2a);//列地址设置
	LCD_WR_DATA(Xstart+1);
	LCD_WR_DATA(Xend+1);
	LCD_WR_REG(0x2b);//行地址设置
	LCD_WR_DATA(y1+26);
	LCD_WR_DATA(Yend+26);
	LCD_WR_REG(0x2c);//储存器写

#endif
}

/**********************************************
函数名：Lcd块选函数
功能：选定Lcd上指定的矩形区域
注意：xStart和 yStart随着屏幕的旋转而改变，位置是矩形框的四个角
入口参数：        xStart x方向的起始点
          ySrart y方向的终止点
          xLong 要选定矩形的x方向长度
          yLong  要选定矩形的y方向长度
返回值：无
***********************************************/
void lcd_color_box(uint8_t xStart,uint8_t yStart,uint8_t xLong,uint8_t yLong,uint16_t color)
{
	uint8_t i,j;
	OLED_CS_Clr(); //片选有效
	LCD_Address_Set(xStart,yStart,xStart+xLong,yStart+yLong);
	OLED_DC_Set(); //写数据
	for(i=0;i<xLong+1;i++)
	{
		for(j=0;j<yLong+1;j++)
		{
			LCD_WR_DATA(color);
		}
	}
	OLED_CS_Set();    //片选无效
}



/**********************************************
Lcd打点函数
***********************************************/
static void DrawPixel(uint8_t x, uint8_t y, uint16_t color)
{
	//OLED_CS_Clr(); //片选有效
	LCD_WR_REG(0x2a);//列地址设置
	LCD_WR_DATA_FAST(x+1);
	LCD_WR_REG(0x2b);//行地址设置
	LCD_WR_DATA_FAST(y+26);
	LCD_WR_REG(0x2c);//储存器写
	LCD_WR_DATA(color);
	//OLED_CS_Set();    //片选无效
}

/******************************************************
* 函数名：Display_GB1212
* 描述  ：显示12*12的汉字
* 输入  : 位置、内容、字体颜色
* 输出  ：无
* 说明  : 需要在GB1212.h里加入要显示的字体 或引用外部flash字库
* 履历  ：V0.00   2018-05-27
*********************************************************/
void display_gb1212(uint8_t x0, uint8_t y0,const char *s, uint16_t fColor,uint16_t bColor)
{
    unsigned int font_data=0;
    unsigned short Num,h=0,l=0,i=0;
    while(*s != '\0')//如果字符串未检验完 继续循环
    {
    	OLED_CS_Clr(); //片选有效
        if( *s >=0xa1){  //GB2312编码汉字的 显示
          //--------对比是否在字库中--------------------------
           for(Num = 0; Num < len_code12x12; Num++)
           {
             //判断是否是否为输入字节
             if((GB2312Code12x12[Num].Head[0] == *s) && (GB2312Code12x12[Num].Head[1] == *(s+1))){

                 for(h=0;h<12;h++){ //行数
                     //-----------记录数据---------------------------
                     font_data = (GB2312Code12x12[Num].Infor[2*h]<<8)|(GB2312Code12x12[Num].Infor[2*h+1]);
                     //打点
                     for(i=0;i<12;i++){
                         if(font_data&0x8000){
                             DrawPixel(x0+i+l,y0+h,fColor);
                         }else{
                        	 DrawPixel(x0+i+l,y0+h,bColor);
                         }
                         font_data<<=1;
                     }
                 }
                 break;
             }
           }
           l+=12;//转移到下一个显示位置
           s+=2;//转移到下一个字符
        }else{             //ASCII的表示
        //--------对比是否在字库中--------------------------
           for(Num = 0; Num < len_code6x12; Num++)
           {
             //判断是否是否为输入字节
             if(GB2312Code6x12[Num].Head[0] == *s){

                 for(h=0;h<12;h++){ //行数
                     //-----------记录数据---------------------------
                     font_data = GB2312Code6x12[Num].Infor[h];
                     //打点
                     for(i=0;i<8;i++){
                         if(font_data&0x80){
                             DrawPixel(x0+i+l,y0+h,fColor);
                         }//else{
                        //	 DrawPixel(x0+i+l,y0+h,bColor);
                         //}
                         font_data<<=1;
                     }
                 }
                 break;
             }
           }
           l+=7;//转移到下一个显示位置
           s+=1;//转移到下一个字符
        }
    }
    OLED_CS_Set();    //片选无效
}

/******************************************************
* 函数名：Display_GB1616
* 描述  ：显示16*16的汉字 或 8*16的ASCII
* 输入  : 位置、内容、字体颜色
* 输出  ：无
* 说明  : 需要在GB1616.h里加入要显示的字体 或引用外部flash字库
* 履历  ：V0.02   2017-05-24
*********************************************************/
void display_gb1616(uint8_t x0, uint8_t y0,const char *s, uint16_t fColor,uint16_t bColor)
{
    unsigned int font_data=0;
    unsigned short Num,h=0,l=0,i=0;
    while(*s != '\0')//如果字符串未检验完 继续循环
    {
    	OLED_CS_Clr(); //片选有效
        if( *s >=0xa1){  //GB2312编码汉字的 显示
          //--------对比是否在字库中--------------------------
           for(Num = 0; Num < len_code16x16; Num++)
           {
             //判断是否是否为输入字节
             if((GB2312Code16x16[Num].Head[0] == *s) && (GB2312Code16x16[Num].Head[1] == *(s+1))){

                 for(h=0;h<16;h++){ //行数
                     //-----------记录数据---------------------------
                     font_data = (GB2312Code16x16[Num].Infor[2*h]<<8)|(GB2312Code16x16[Num].Infor[2*h+1]);
                     //打点
                     for(i=0;i<16;i++){
                         if(font_data&0x8000){
                                 DrawPixel(x0+i+l,y0+h,fColor);
                         }else{
                                 DrawPixel(x0+i+l,y0+h,bColor);
                         }
                         font_data<<=1;
                     }
                 }
                 break;
             }
           }
           l+=17;//转移到下一个显示位置
           s+=2;//转移到下一个字符
        }else{             //ASCII的表示
          //--------对比是否在字库中--------------------------
           for(Num = 0; Num < len_code8x16; Num++)
           {
             //判断是否是否为输入字节
             if(GB2312Code8x16[Num].Head[0] == *s){

                 for(h=0;h<16;h++){ //行数
                     //-----------记录数据---------------------------
                     font_data = GB2312Code8x16[Num].Infor[h];
                     //打点
                     for(i=0;i<8;i++){
                         if(font_data&0x80){
                                 DrawPixel(x0+i+l,y0+h,fColor);
                         }else{
                                 DrawPixel(x0+i+l,y0+h,bColor);
                         }
                         font_data<<=1;
                     }
                 }
                 break;
             }
           }
           l+=9;//转移到下一个显示位置
           s+=1;//转移到下一个字符
        }
    }
    OLED_CS_Set();    //片选无效
}
///******************************************************
//* 函数名：Display_GB2424
//* 描述  ：显示24*24的汉字 或 12*24的ASCII
//* 输入  : 位置、内容、字体颜色
//* 输出  ：无
//* 说明  : 需要在GB2424.h里加入要显示的字体 或引用外部flash字库
//* 履历  ：V0.02   2019-04-30
//*********************************************************/
//void display_gb2424(uint8_t x0, uint8_t y0,const char *s, uint16_t fColor)
//{
//    unsigned int font_data=0;
//    unsigned short Num,h=0,l=0,i=0;
//    while(*s != '\0')//如果字符串未检验完 继续循环
//    {
//    	OLED_CS_Clr(); //片选有效
//        if( *s >=0xa1){  //GB2312编码汉字的 显示
//          //--------对比是否在字库中--------------------------
//           for(Num = 0; Num < len_code24x24; Num++)
//           {
//             //判断是否是否为输入字节
//             if((GB2312Code24x24[Num].Head[0] == *s) && (GB2312Code24x24[Num].Head[1] == *(s+1))){
//
//                 for(h=0;h<24;h++){ //行数
//                     //-----------记录数据---------------------------
//                     font_data = (GB2312Code24x24[Num].Infor[3*h]<<16)|(GB2312Code24x24[Num].Infor[3*h+1]<<8)|(GB2312Code24x24[Num].Infor[3*h+2]);
//                     //打点
//                     for(i=0;i<24;i++){
//                         if(font_data&0x800000){
//                                 DrawPixel(x0+i+l,y0+h,fColor);
//                         }//else{
//                          //       DrawPixel(x0+i+l,y0+h,bColor);
//                          //}
//                         font_data<<=1;
//                     }
//                 }
//                 break;
//             }
//           }
//           l+=25;//转移到下一个显示位置
//           s+=2;//转移到下一个字符
//        }else{             //ASCII的表示
//          //--------对比是否在字库中--------------------------
//           for(Num = 0; Num < len_code12x24; Num++)
//           {
//             //判断是否是否为输入字节
//             if(GB2312Code12x24[Num].Head[0] == *s){
//
//                 for(h=0;h<24;h++){ //行数
//                     //-----------记录数据---------------------------
//                     font_data = (GB2312Code12x24[Num].Infor[h*2]<<8)|(GB2312Code12x24[Num].Infor[h*2+1]);
//                     //打点
//                     for(i=0;i<16;i++){
//                         if(font_data&0x8000){
//                                 DrawPixel(x0+i+l,y0+h,fColor);
//                         }//else{
//                              //   DrawPixel(x0+i+l,y0+h,bColor);
//                         //}
//                         font_data<<=1;
//                     }
//                 }
//                 break;
//             }
//           }
//           if((*s=='s')||(*s=='r')||(*s=='.')){
//        	   l+=8;//转移到下一个显示位置
//           }else{
//        	   l+=12;//转移到下一个显示位置
//           }
//
//           s+=1;//转移到下一个字符
//        }
//    }
//    OLED_CS_Set();    //片选无效
//}
/******************************************************************************
      函数说明：LCD清屏函数
      入口数据：无
      返回值：  无
******************************************************************************/
void disp_clear_screen(uint16_t color)
{
	uint8_t i,j;
	OLED_CS_Clr(); //片选有效
	LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);
	OLED_DC_Set(); //写数据
	for(i=0;i<LCD_W;i++){
		for(j=0;j<LCD_H;j++){
			LCD_Writ_Bus(color>>8);
			LCD_Writ_Bus(color);;
		}
	}
	OLED_CS_Set();    //片选无效
}

/******************************************************************************
      函数说明：显示二维码
      入口数据：无
      返回值：  无
******************************************************************************/
//void disp_QRcode(uint16_t color,uint16_t Bcolor)
//{
//	uint8_t i,j,h;
//	uint8_t font_data;
//	OLED_CS_Clr(); //片选有效
//	LCD_Address_Set(40,0,120-1,80-1);
//	OLED_DC_Set(); //写数据
//	for(h=0;h<80;h++){ //行数
//		for(j=0;j<10;j++){
//			  font_data = gImage_drcode[h*10+j];
//			  for(i=0;i<8;i++){
//			    if(font_data&0x80){
//			    	LCD_Writ_Bus(color>>8);
//			    	LCD_Writ_Bus(color);
//			    }else{
//			    	LCD_Writ_Bus(Bcolor>>8);
//			    	LCD_Writ_Bus(Bcolor);
//			    }
//			    font_data<<=1;
//			 }
//	   }
//	}
//	OLED_CS_Set();    //片选无效
//}

/******************************************************************************
      函数说明：显示图标
      入口数据：无
      返回值：  无
******************************************************************************/
void disp_image_3232(uint8_t x,uint8_t y,const uint8_t* buff, uint16_t color,uint16_t Bcolor)
{
	uint8_t i,j,h;
	uint8_t font_data;
	OLED_CS_Clr(); //片选有效
	LCD_Address_Set(x,y,x+32-1,y+32-1);
	OLED_DC_Set(); //写数据
	for(h=0;h<32;h++){ //行数
		for(j=0;j<4;j++){
			  font_data = buff[h*4+j];
			  for(i=0;i<8;i++){
			    if(font_data&0x80){
			    	LCD_Writ_Bus(color>>8);
			    	LCD_Writ_Bus(color);
			    }else{
			    	LCD_Writ_Bus(Bcolor>>8);
			    	LCD_Writ_Bus(Bcolor);
			    }
			    font_data<<=1;
			 }
	   }
	}
	OLED_CS_Set();    //片选无效
}

/******************************************************************************
      函数说明：显示图标
      入口数据：无
      返回值：  无
******************************************************************************/
void disp_image_8040(uint8_t x,uint8_t y,const uint8_t* buff, uint16_t color,uint16_t Bcolor)
{
	uint8_t i,j,h;
	uint8_t font_data;
	OLED_CS_Clr(); //片选有效
	LCD_Address_Set(x,y,x+80-1,y+40-1);
	OLED_DC_Set(); //写数据
	for(h=0;h<40;h++){ //行数
		for(j=0;j<10;j++){
			font_data = buff[h*10+j];
			for(i=0;i<8;i++){
			   if(font_data&0x80){
				   LCD_Writ_Bus(color>>8);
				   LCD_Writ_Bus(color);
		       }else{
				   LCD_Writ_Bus(Bcolor>>8);
				   LCD_Writ_Bus(Bcolor);
			   }
			   font_data<<=1;
	       }
	    }
	}
	OLED_CS_Set();    //片选无效
}
/******************************************************************************
      函数说明：显示图标
      入口数据：无
      返回值：  无
******************************************************************************/
void disp_image_10080(uint8_t x,uint8_t y,const uint8_t* buff, uint16_t color,uint16_t Bcolor)
{
	uint8_t i,j,h;
	uint8_t font_data;
	OLED_CS_Clr(); //片选有效
	LCD_Address_Set(x,y,x+96-1,y+80-1);
	OLED_DC_Set(); //写数据
	for(h=0;h<80;h++){ //行数
		for(j=0;j<12;j++){
			font_data = buff[h*12+j];
			for(i=0;i<8;i++){
			   if(font_data&0x80){
				   LCD_Writ_Bus(color>>8);
				   LCD_Writ_Bus(color);
		       }else{
				   LCD_Writ_Bus(Bcolor>>8);
				   LCD_Writ_Bus(Bcolor);
			   }
			   font_data<<=1;
	       }
	    }
	}
	OLED_CS_Set();    //片选无效
}
/******************************************************************************
      函数说明：显示矩形
      入口数据：无
      返回值：  无
******************************************************************************/
void disp_rectangle(uint8_t x,uint8_t y,uint8_t x_len,uint8_t y_len, uint16_t color)
{
	uint8_t j,h;
	OLED_CS_Clr(); //片选有效
	LCD_Address_Set(x,y,x+x_len-1,y+y_len-1);
	OLED_DC_Set(); //写数据
	for(h=0;h<y_len;h++){ //行数
		for(j=0;j<x_len;j++){
			LCD_Writ_Bus(color>>8);
			LCD_Writ_Bus(color);
	    }
	}
	OLED_CS_Set();    //片选无效
}
/******************************************************************************
      函数说明：LCD初始化函数
      入口数据：无
      返回值：  无
******************************************************************************/
void lcd_init(void)
{
    //************* Start Initial Sequence **********//
	OLED_CS_Clr();
    LCD_WR_REG(0x36);
    LCD_WR_DATA8(HORIZONTAL_DATA);
    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x05);
    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);
    LCD_WR_REG(0xB7);
    LCD_WR_DATA8(0x35);
    LCD_WR_REG(0xBB);
    LCD_WR_DATA8(0x19);
    LCD_WR_REG(0xC0);
    LCD_WR_DATA8(0x2C);
    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x01);
    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x12);
    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x20);
    LCD_WR_REG(0xC6);
    LCD_WR_DATA8(0x0F);
    LCD_WR_REG(0xD0);
    LCD_WR_DATA8(0xA4);
    LCD_WR_DATA8(0xA1);
    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x04);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x11);
    LCD_WR_DATA8(0x13);
    LCD_WR_DATA8(0x2B);
    LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x54);
    LCD_WR_DATA8(0x4C);
    LCD_WR_DATA8(0x18);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x0B);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x23);
    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x04);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x11);
    LCD_WR_DATA8(0x13);
    LCD_WR_DATA8(0x2C);
    LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x44);
    LCD_WR_DATA8(0x51);
    LCD_WR_DATA8(0x2F);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x20);
    LCD_WR_DATA8(0x23);
    OLED_CS_Set();
    disp_clear_screen(BLACK);
    OLED_CS_Clr();
    LCD_WR_REG(0x21);
    LCD_WR_REG(0x11);
    LCD_WR_REG(0x29);
    OLED_CS_Set();
}

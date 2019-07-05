/*
 * st7735.h
 *
 *  Created on: 2019年4月13日
 *      Author: gongjunhui
 */

#ifndef ST7735_H_
#define ST7735_H_

#include "main.h"

#define USE_HORIZONTAL 2  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

#if  USE_HORIZONTAL==0
#define LCD_W 80
#define LCD_H 160
#define HORIZONTAL_DATA  0x08

#elif USE_HORIZONTAL==1
#define LCD_W 80
#define LCD_H 160
#define HORIZONTAL_DATA  0xC8

#elif USE_HORIZONTAL==2
#define LCD_W 160
#define LCD_H 80
#define HORIZONTAL_DATA  0x78

#else
#define LCD_W 160
#define LCD_H 80
#define HORIZONTAL_DATA  0xA8

#endif



//-----------------OLED端口定义----------------
#define OLED_SCLK_Clr() LCD_SCL_CLR//CLK
#define OLED_SCLK_Set() LCD_SCL_SET
#define OLED_SDIN_Clr() LCD_SDA_CLR//DIN
#define OLED_SDIN_Set() LCD_SDA_SET
#define OLED_RST_Clr()  LCD_RESET_CLR//RES
#define OLED_RST_Set()  LCD_RESET_SET
#define OLED_DC_Clr()   LCD_RS_CLR//DC
#define OLED_DC_Set()   LCD_RS_SET
#define OLED_CS_Clr()   LCD_CS_CLR//CS
#define OLED_CS_Set()   LCD_CS_SET


//画笔颜色
#define   BLACK                0x0000                // 黑色：    0,   0,   0 //
#define   BLUE                 0x001F                // 蓝色：    0,   0, 255 //
#define   GREEN                0x07E0                // 绿色：    0, 255,   0 //
#define   CYAN                 0x07FF                // 青色：    0, 255, 255 //
#define   RED                  0xF800                // 红色：  255,   0,   0 //
#define   MAGENTA              0xF81F                // 品红：  255,   0, 255 //
#define   YELLOW               0xFFE0                // 黄色：  255, 255, 0   //
#define   WHITE                0xFFFF                // 白色：  255, 255, 255 //
#define   NAVY                 0x000F                // 深蓝色：  0,   0, 128 //
#define   DGREEN               0x03E0                // 深绿色：  0, 128,   0 //
#define   DCYAN                0x03EF                // 深青色：  0, 128, 128 //
#define   MAROON               0x7800                // 深红色：128,   0,   0 //
#define   PURPLE               0x780F                // 紫色：  128,   0, 128 //
#define   OLIVE                0x7BE0                // 橄榄绿：128, 128,   0 //
#define   LGRAY                0xC618                // 灰白色：192, 192, 192 //
#define   DGRAY                0x7BEF                // 深灰色：128, 128, 128 //
#define   BROWN 			   0XBC40                // 棕色
#define   BRRED 			   0XFC07                // 棕红色
#define   GRAY  			   0X8430                //灰色
//GUI颜色
#define   DARKBLUE      	  0X01CF	    //深蓝色
#define   LIGHTBLUE      	  0X7D7C	    //浅蓝色
#define   GRAYBLUE       	  0X5458        //灰蓝色
//以上三色为PANEL的颜色
#define   LIGHTGREEN     	  0X841F        //浅绿色
#define   LGRAYBLUE           0XA651        //浅灰蓝色(中间层颜色)
#define   LBBLUE              0X2B12        //浅棕蓝色(选择条目的反色)













extern void lcd_init(void); //LCD初始化函数
extern void disp_clear_screen(uint16_t color);//LCD清屏函数
//--------24*24字库显示-------------
//extern void display_gb2424(uint8_t x0, uint8_t y0,const char *s, uint16_t fColor);
//--------16*16字库显示-------------
extern void display_gb1616(uint8_t x0, uint8_t y0,const char *s, uint16_t fColor,uint16_t bColor);
//--------12*12字库显示-------------
extern void display_gb1212(uint8_t x0, uint8_t y0,const char *s, uint16_t fColor,uint16_t bColor);

//extern void disp_QRcode(uint16_t color,uint16_t Bcolor);//二维码显示函数
extern void disp_image_3232(uint8_t x,uint8_t y,const uint8_t* buff, uint16_t color,uint16_t Bcolor);
extern void disp_image_8040(uint8_t x,uint8_t y,const uint8_t* buff, uint16_t color,uint16_t Bcolor);
extern void disp_image_10080(uint8_t x,uint8_t y,const uint8_t* buff, uint16_t color,uint16_t Bcolor);
extern void disp_rectangle(uint8_t x,uint8_t y,uint8_t x_len,uint8_t y_len, uint16_t color);

#endif /* ST7735_H_ */



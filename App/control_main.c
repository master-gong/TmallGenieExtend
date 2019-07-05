/*
 * control_main.c
 *
 *  Created on: 2019年4月13日
 *      Author: gongjunhui
 */


#include "control_main.h"
#include "control_bl.h"
#include "key_handle.h"
#include <string.h>
#include "gpio.h"
#include "st7735.h"
#include "qn8027.h"
#include "image.h"	//图片数据
#include "iwdg.h"


_audio_mode_em  em_audio_mode; //音频处理模式
_work_mode_em   em_work_mode;  //工作显示模式

_device_switch_em  em_FM_switch;//FM广播开关
_device_switch_em  em_BL_switch;//蓝牙开关
//static _device_switch_em  em_ADC_switch;//解码开关
static _device_switch_em  em_play_switch;//方糖播放开关

static  uint8_t  first_power_on_flag;//初次上电标志位
static  uint8_t  uc_lcd_init_state;//用于液晶初始化判断

static  uint16_t  uc_lcd_init_count;//用于液晶初始化计数
//static  uint8_t  lcd_display_refresh_flag; //显示标志

static  uint8_t  uc_fm_channel_old;  //fm发射频率 显示用 76Mhz - 108 Mhz 只取整数
static  _bl_status_em  em_bl_state_old;//蓝牙模块状态

static  uint8_t   falsh_write_flag;  //flash数据写标志
static  uint16_t  falsh_write_count; //flash数据写计数
static  uint16_t  menu_exit_count;       //菜单退出计数
static  uint16_t  dormancy_exit_count;   //休眠计数

/*******************************************
 * 函数名：flash_write_handle
 * 描述  ：  flash写操作
 * 输入  ：  无
 * 输出  ：  无
 * 履历  ：  V0.00  2019-5-2
********************************************/
static void flash_write_handle(void)
{
   FLASH_EraseInitTypeDef f;
   uint32_t PageError = 0;//设置PageError
   uint32_t ui_data = 0;//需要保存的数据
   uint32_t ui_dataq = 0;

   //byte1 byte2 byte3 byte4
   //byte4:bit7-bit4: 0001音频解码模式  0000音频输入模式
   //byte4:bit3-bit0: 0001方糖播放开      0000方糖播放关
   //byte3:bit7-bit4: 0001蓝牙开             0000蓝牙关
   //byte3:bit3-bit0: 0001FM开               0000FM关
   //byte2:FM频道

   HAL_FLASH_Unlock();//1、解锁FLASH
   //2、擦除FLASH
   //初始化FLASH_EraseInitTypeDef
   f.TypeErase = FLASH_TYPEERASE_PAGES;/*!<Pages erase only*/
   f.PageAddress = 0x08007C00;//起始地址
   f.NbPages = 1;//擦除1个页
   //调用擦除函数
   HAL_FLASHEx_Erase(&f, &PageError);
   ui_data = 0;
   if(em_audio_mode == AUDIO_DECODING){//音频处理模式 音频解码
	   ui_data |= 0x10;
   }
   //if(em_play_switch == SWITCH_OPEN){//方糖播放开关
	//   ui_data |= 0x01;
   //}
   if(em_BL_switch == SWITCH_OPEN){   //蓝牙开关
   	   ui_data |= 0x1000;
   }
   if(em_FM_switch == SWITCH_OPEN){   //FM开关
      ui_data |= 0x0100;
   }
   ui_dataq = uc_fm_channel;
   ui_data |= ((ui_dataq<<16)&0x00ff0000);//广播频道
   //3、对FLASH烧写
   HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x08007C00, ui_data);
   //4、锁住FLASH
   HAL_FLASH_Lock();
}
/*******************************************
 * 函数名：flash_read_handle
 * 描述  ：  flash读操作
 * 输入  ：  无
 * 输出  ：  无
 * 履历  ：  V0.00  2019-5-2
********************************************/
static void flash_read_handle(void)
{
   uint32_t ui_data = *(__IO uint32_t*)(0x08007C00);
   //byte4:bit3-bit0: 0001方糖播放开      0000方糖播放关
//   if((ui_data&0x0000000F)== 0x00000001){
//	   em_play_switch = SWITCH_OPEN;
//   }else{
//	   em_play_switch = SWITCH_CLOSE;
//   }
   //byte4:bit7-bit4: 0001音频解码模式  0000音频输入模式
   if((ui_data&0x000000F0)== 0x00000010){
	   em_audio_mode = AUDIO_DECODING;
   }else{
	   em_audio_mode = AUDIO_INPUT;
   }
   //byte3:bit7-bit4: 0001蓝牙开             0000蓝牙关
   if((ui_data&0x0000F000)== 0x00001000){
	   em_BL_switch = SWITCH_OPEN;
   }else{
	   em_BL_switch = SWITCH_CLOSE;
   }
   //byte3:bit3-bit0: 0001FM开               0000FM关
   if((ui_data&0x00000F00)== 0x00000100){
   	   em_FM_switch = SWITCH_OPEN;
   }else{
   	   em_FM_switch = SWITCH_CLOSE;
   }
   //byte2:FM频道
   uc_fm_channel = (ui_data>>16)&0xff;
   if(uc_fm_channel>108){
	   uc_fm_channel=108;
   }else if(uc_fm_channel<76){
	   uc_fm_channel=76;
   }

}
/*******************************************
 * 函数名：work_mode_screen_init
 * 描述  ：  屏幕初始化
 * 输入  ：  无
 * 输出  ：  无
 * 履历  ：  V0.00  2019-4-13
********************************************/
static void work_mode_screen_init(void)
{
	uint8_t disp_buff[5];
	if(uc_lcd_init_state == 0){
		OLED_RST_Clr();
		uc_lcd_init_count++;
		if(uc_lcd_init_count>20){
			uc_lcd_init_count = 0;
			uc_lcd_init_state = 1;
		}
		return;
	}else if(uc_lcd_init_state == 1){
		OLED_RST_Set();
		uc_lcd_init_count++;
		if(uc_lcd_init_count>30){
			uc_lcd_init_count = 0;
			uc_lcd_init_state = 2;
		}
		return;
	}
	lcd_init();
	MX_IWDG_Refresh();
	if(first_power_on_flag == 1){
		first_power_on_flag = 0;
		em_work_mode = WORK_MODE_POWER_ON; //开机logo
	}else{
		em_work_mode = WORK_MODE_NORMAL;   //正常模式
		uckey_value = KEY_NONE;   //没有键值
		//disp_clear_screen(BLACK);
		disp_rectangle(39,0,1,80,LGRAY);//
		disp_rectangle(120,0,1,80,LGRAY);
		disp_rectangle(40,0,80,80,DCYAN);

		if(em_FM_switch == SWITCH_OPEN){//FM广播开关
			disp_image_3232(128,45,gImage_FM,GRAYBLUE,BLACK);
			if(uc_fm_channel>99){//三位数
				disp_buff[0] = (uc_fm_channel/100)+0x30;
				disp_buff[1] = (uc_fm_channel/10%10)+0x30;
				disp_buff[2] = (uc_fm_channel%10)+0x30;
				disp_buff[3] = 0;
				display_gb1212(134,58,(const char*)disp_buff,BLUE,BLACK);//数字显示
			}else{ //二位数
				disp_buff[0] = (uc_fm_channel/10%10)+0x30;
				disp_buff[1] = (uc_fm_channel%10)+0x30;
				disp_buff[2] = 0;
				display_gb1212(141,58,(const char*)disp_buff,BLUE,BLACK);//数字显示
			}
		}else{
			disp_image_3232(128,45,gImage_FM,GRAY,BLACK);
		}
		if(em_BL_switch == SWITCH_OPEN){//蓝牙开关
			if(em_bl_state == BL_STATUS_LINK_OK){ //连接成功
				disp_image_3232(2,45,gImage_BL_ON,GRAYBLUE,BLACK);
			}else{
				disp_image_3232(2,45,gImage_BL_OFF,GRAYBLUE,BLACK);
			}
		}else{
			disp_image_3232(2,45,gImage_BL_OFF,GRAY,BLACK);
		}
		if(em_play_switch == SWITCH_OPEN){  //方糖播放开关
			disp_image_3232(2,5,gImage_audio,GRAYBLUE,BLACK);
		}else{
			disp_image_3232(2,5,gImage_audio,GRAY,BLACK);
		}
		if(em_audio_mode == AUDIO_DECODING){  //音频处理模式 音频解码
			disp_image_3232(128,5,gImage_headset,GRAYBLUE,BLACK);
			display_gb1616(64,2,"模式",BLUE,DCYAN);//LGRAY
			disp_image_8040(40,20,gImage_music,MAGENTA,DCYAN);//LIGHTGREEN
			display_gb1212(56,67,"方糖解码",GREEN,DCYAN);//RED
		}else{
			disp_image_3232(128,5,gImage_headset,GRAY,BLACK);
			display_gb1616(64,2,"模式",BLUE,DCYAN);//LGRAY
			disp_image_8040(40,20,gImage_35mm,YELLOW,DCYAN);//LIGHTGREEN
			display_gb1212(56,67,"音频输入",GREEN,DCYAN);//RED
		}
	}
}
/*******************************************
 * 函数名：work_mode_power_on
 * 描述  ：  屏幕logo显示
 * 输入  ：  无
 * 输出  ：  无
 * 履历  ：  V0.00  2019-4-13
********************************************/
static void work_mode_power_on(void)
{
	uint8_t disp_buff[5];
	uc_lcd_init_count++;
	if(uc_lcd_init_count==1){
		//disp_clear_screen(BLACK);
		disp_image_10080(31,0,gImage_logo,BLUE,BLACK);
		//display_gb2424(8,47,"mastergong.cn",BLUE);
	}else if(uc_lcd_init_count==(1000/10)){
		display_gb1616(20,30,"方糖配我更精彩",RED,BLACK);
	}else if(uc_lcd_init_count==(3000/10)){
		display_gb1616(132,60,"1.0",GREEN,BLACK);
	}else if(uc_lcd_init_count==(5000/10)){
		disp_clear_screen(BLACK);
	}else if(uc_lcd_init_count>(5000/10)){
		uc_lcd_init_count =0 ;
//		disp_QRcode(BLUE,WHITE);
//		display_gb1616(12,20,"改",RED);
//		display_gb1616(12,50,"装",RED);
//		display_gb1616(134,20,"教",RED);
//		display_gb1616(134,50,"程",RED);
		em_work_mode = WORK_MODE_NORMAL;   //正常模式
		uckey_value = KEY_NONE;   //没有键值

		//disp_clear_screen(BLACK);
		disp_rectangle(39,0,1,80,LGRAY);//
		disp_rectangle(120,0,1,80,LGRAY);
		disp_rectangle(40,0,80,80,DCYAN);

		if(em_FM_switch == SWITCH_OPEN){//FM广播开关
			disp_image_3232(128,45,gImage_FM,GRAYBLUE,BLACK);
			if(uc_fm_channel>99){//三位数
				disp_buff[0] = (uc_fm_channel/100)+0x30;
				disp_buff[1] = (uc_fm_channel/10%10)+0x30;
				disp_buff[2] = (uc_fm_channel%10)+0x30;
				disp_buff[3] = 0;
				display_gb1212(134,58,(const char*)disp_buff,BLUE,BLACK);//数字显示
			}else{ //二位数
				disp_buff[0] = (uc_fm_channel/10%10)+0x30;
				disp_buff[1] = (uc_fm_channel%10)+0x30;
				disp_buff[2] = 0;
				display_gb1212(141,58,(const char*)disp_buff,BLUE,BLACK);//数字显示
			}
		}else{
			disp_image_3232(128,45,gImage_FM,GRAY,BLACK);
		}
		if(em_BL_switch == SWITCH_OPEN){//蓝牙开关
			if(em_bl_state == BL_STATUS_LINK_OK){ //连接成功
				 disp_image_3232(2,45,gImage_BL_ON,GRAYBLUE,BLACK);
			}else{
				 disp_image_3232(2,45,gImage_BL_OFF,GRAYBLUE,BLACK);
			}
		}else{
			disp_image_3232(2,45,gImage_BL_OFF,GRAY,BLACK);
		}
		if(em_play_switch == SWITCH_OPEN){  //方糖播放开关
			disp_image_3232(2,5,gImage_audio,GRAYBLUE,BLACK);
		}else{
			disp_image_3232(2,5,gImage_audio,GRAY,BLACK);
		}
		if(em_audio_mode == AUDIO_DECODING){  //音频处理模式 音频解码
			disp_image_3232(128,5,gImage_headset,GRAYBLUE,BLACK);
			display_gb1616(64,2,"模式",BLUE,DCYAN);//LGRAY
			disp_image_8040(40,20,gImage_music,MAGENTA,DCYAN);//LIGHTGREEN
			display_gb1212(56,67,"方糖解码",GREEN,DCYAN);//RED
		}else{
			disp_image_3232(128,5,gImage_headset,GRAY,BLACK);
			display_gb1616(64,2,"模式",BLUE,DCYAN);//LGRAY
			disp_image_8040(40,20,gImage_35mm,YELLOW,DCYAN);//LIGHTGREEN
			display_gb1212(56,67,"音频输入",GREEN,DCYAN);//RED
		}
	}



}
/*******************************************
 * 函数名：work_mode_screen_close
 * 描述  ：  屏幕关闭
 * 输入  ：  无
 * 输出  ：  无
 * 履历  ：  V0.00  2019-4-13
********************************************/
static void work_mode_screen_close(void)
{
  LCD_POWER_OFF;
  LCD_SCL_CLR;
  LCD_SDA_CLR;
  LCD_RESET_CLR;
  LCD_RS_CLR;
  LCD_CS_CLR;
  //----------按键处理---------------
  if(uckey_value != KEY_NONE){  //有按键
	  uckey_value = KEY_NONE;
	  em_work_mode = WORK_MODE_SCREEN_INIT;  //液晶初始化
	  LCD_POWER_ON;
	  LCD_SCL_SET;
	  LCD_SDA_SET;
	  LCD_RESET_SET;
	  LCD_RS_SET;
	  LCD_CS_SET;
  }
}
/*******************************************
 * 函数名：work_mode_normal
 * 描述  ：  正常模式
 * 输入  ：  无
 * 输出  ：  无
 * 履历  ：  V0.00  2019-4-13
********************************************/
static void work_mode_normal(void)
{
  uint8_t disp_buff[5];

  task_qn8027_function();

  //--------休眠判断-------
  dormancy_exit_count++;   //休眠计数
  if(dormancy_exit_count >= (5*60*1000/10)){//5min
	  dormancy_exit_count=0; //休眠计数
	  em_work_mode = WORK_MODE_SCREEN_CLOSE; //息屏
  }
  //------- 静音判断 ------------
  if(em_play_switch == SWITCH_OPEN){//方糖播放开关
	  DAC_MTUE_OFF;
  }else{
	  DAC_MTUE_ON;
  }

  //-------Flash 存储部分操作------------
  if(falsh_write_flag == 0x5A){ //flash数据写标志
	  falsh_write_count++;
	  if(falsh_write_count >= (5*60*1000/10)){//5min
		  falsh_write_flag = 0;
		  flash_write_handle();//flash写操作
	  }
  }else{
	  falsh_write_count=0;
  }
  //----------解码部分电源控制----------------
  if(em_audio_mode == AUDIO_DECODING){ //音频解码
	  DAC_POWER_ON;
  }else{             //音频输入
	  DAC_POWER_OFF;
  }
  //------------蓝牙标志实时刷新----------------
  if(em_bl_state_old != em_bl_state){
	  if(em_bl_state == BL_STATUS_LINK_OK){ //连接成功
		  disp_image_3232(2,45,gImage_BL_ON,GRAYBLUE,BLACK);
	  }else{
	  	  disp_image_3232(2,45,gImage_BL_OFF,GRAYBLUE,BLACK);
	  }
      em_bl_state_old = em_bl_state;//记录蓝牙状态
  }
  //----------按键处理---------------
  if(uckey_value == KEY_FM_SHORT_PRESS){  //FM键 短按
	  uckey_value = KEY_NONE;
	  dormancy_exit_count=0; //休眠计数
	  falsh_write_flag = 0x5A; //flash数据写标志
	  if(em_FM_switch == SWITCH_OPEN){//FM广播开关
		  em_FM_switch = SWITCH_CLOSE;
		  disp_image_3232(128,45,gImage_FM,GRAY,BLACK);
	  }else{
		  em_FM_switch = SWITCH_OPEN;
		  disp_image_3232(128,45,gImage_FM,GRAYBLUE,BLACK);
		  if(uc_fm_channel>99){//三位数
			 disp_buff[0] = (uc_fm_channel/100)+0x30;
			 disp_buff[1] = (uc_fm_channel/10%10)+0x30;
			 disp_buff[2] = (uc_fm_channel%10)+0x30;
			 disp_buff[3] = 0;
			 display_gb1212(134,58,(const char*)disp_buff,BLUE,BLACK);//数字显示
		  }else{ //二位数
			 disp_buff[0] = (uc_fm_channel/10%10)+0x30;
			 disp_buff[1] = (uc_fm_channel%10)+0x30;
			 disp_buff[2] = 0;
			 display_gb1212(141,58,(const char*)disp_buff,BLUE,BLACK);//数字显示
		  }

	  }
  }else if(uckey_value == KEY_FM_LONG_PRESS){  //FM键 长按
	  uckey_value = KEY_NONE;
	  dormancy_exit_count=0; //休眠计数
	  menu_exit_count = 0;   //菜单退出计数
	  em_work_mode = WORK_MODE_FM;  //FM设置
	  em_FM_switch = SWITCH_OPEN;
	  disp_clear_screen(BLACK);
	  uc_fm_channel_old = uc_fm_channel;//记录FM调频值
	  display_gb1616(55,2,"FM调频",RED,BLACK);//LGRAY
	  display_gb1212(4,65,"减",GREEN,BLACK);
	  display_gb1212(144,65,"加",GREEN,BLACK);
	  display_gb1212(66,65,"确定",GREEN,BLACK);
	  if(uc_fm_channel_old < 76){//保护
		  uc_fm_channel_old=76;
	  }
	  if(uc_fm_channel_old>99){//三位数
		  disp_buff[0] = (uc_fm_channel_old/100)+0x30;
		  disp_buff[1] = (uc_fm_channel_old/10%10)+0x30;
		  disp_buff[2] = (uc_fm_channel_old%10)+0x30;
		  disp_buff[3] = 0;
	  }else{ //二位数
		  disp_buff[0] = ' ';
		  disp_buff[1] = (uc_fm_channel_old/10%10)+0x30;
		  disp_buff[2] = (uc_fm_channel_old%10)+0x30;
		  disp_buff[3] = 0;
	  }
	  display_gb1616(42,30,(const char*)disp_buff,YELLOW,BLACK);//数字显示
	  display_gb1616(88,30,"MHz",BLUE,BLACK);//数字显示

  }else if(uckey_value == KEY_BL_SHORT_PRESS){  //蓝牙键 短按
	  uckey_value = KEY_NONE;
	  dormancy_exit_count=0; //休眠计数
	  falsh_write_flag = 0x5A; //flash数据写标志
	  if(em_BL_switch == SWITCH_OPEN){//蓝牙开关
		  em_BL_switch = SWITCH_CLOSE;
		  disp_image_3232(2,45,gImage_BL_OFF,GRAY,BLACK);
	  }else{
		  em_BL_switch = SWITCH_OPEN;
		  disp_image_3232(2,45,gImage_BL_OFF,GRAYBLUE,BLACK);
	  }
  }else if(uckey_value == KEY_BL_LONG_PRESS){  //蓝牙键 长按
	  uckey_value = KEY_NONE;
	  dormancy_exit_count=0; //休眠计数
	  menu_exit_count = 0;   //菜单退出计数
	  em_work_mode = WORK_MODE_BL;   //蓝牙设置
	  em_BL_switch = SWITCH_OPEN;
	  disp_clear_screen(BLACK);
	  display_gb1616(50,2,"蓝牙设置",RED,BLACK);//LGRAY
	  display_gb1212(4,65,"复位",GREEN,BLACK);
	  display_gb1212(135,65,"重启",GREEN,BLACK);
	  display_gb1212(66,65,"返回",GREEN,BLACK);

	  if(em_bl_state == BL_STATUS_RESET){ //出厂设置
	  	  display_gb1212(45,35,"数据清除中",YELLOW,BLACK);

	  }else if(em_bl_state == BL_STATUS_RESTART){ //重启中
		  display_gb1212(45,35,"詹重启中詹",YELLOW,BLACK);

	  }else if(em_bl_state == BL_STATUS_LINKING){ //连接中
		  display_gb1212(45,35,"詹连接中詹",YELLOW,BLACK);//

	  }else{//连接成功
		  display_gb1212(45,35,"詹连接成功",BLUE,BLACK);//
	  }
	  em_bl_state_old = em_bl_state;//记录蓝牙状态

  }else if(uckey_value == KEY_MAIN_SHORT_PRESS){  //主键 短按
	  uckey_value = KEY_NONE;
	  dormancy_exit_count=0; //休眠计数
	  //falsh_write_flag = 0x5A; //flash数据写标志
	  if(em_play_switch == SWITCH_OPEN){  //方糖播放开关
		  em_play_switch = SWITCH_CLOSE;
		  disp_image_3232(2,5,gImage_audio,GRAY,BLACK);
	  }else{
		  em_play_switch = SWITCH_OPEN;
		  disp_image_3232(2,5,gImage_audio,GRAYBLUE,BLACK);
	  }
  }else if(uckey_value == KEY_MAIN_LONG_PRESS){   //主键 长按
	  uckey_value = KEY_NONE;
	  dormancy_exit_count=0; //休眠计数
	  falsh_write_flag = 0x5A; //flash数据写标志
	  if(em_audio_mode == AUDIO_DECODING){  //音频处理模式 音频解码
		  em_audio_mode = AUDIO_INPUT;//音频输入
		  disp_image_3232(128,5,gImage_headset,GRAY,BLACK);
		  display_gb1616(64,2,"模式",BLUE,DCYAN);
		  disp_image_8040(40,20,gImage_35mm,YELLOW,DCYAN);//LIGHTGREEN
		  display_gb1212(56,67,"音频输入",GREEN,DCYAN);//RED
	  }else{
		  em_audio_mode = AUDIO_DECODING;////音频解码
		  disp_image_3232(128,5,gImage_headset,GRAYBLUE,BLACK);
		  display_gb1616(64,2,"模式",BLUE,DCYAN);//LGRAY
		  disp_image_8040(40,20,gImage_music,MAGENTA,DCYAN);//LIGHTGREEN
		  display_gb1212(56,67,"方糖解码",GREEN,DCYAN);//RED
	  }
  }
}
/*******************************************
 * 函数名：work_mode_bl_set
 * 描述  ：  蓝牙设置
 * 输入  ：  无
 * 输出  ：  无
 * 履历  ：  V0.00  2019-5-1
********************************************/
static void work_mode_bl_set(void)
{
	uint8_t disp_buff[5];
    if(em_bl_state_old != em_bl_state){
    	if(em_bl_state == BL_STATUS_RESET){ //出厂设置
    		display_gb1212(45,35,"数据清除中",YELLOW,BLACK);

    	}else if(em_bl_state == BL_STATUS_RESTART){ //重启中
    		display_gb1212(45,35,"詹重启中詹",YELLOW,BLACK);

    	}else if(em_bl_state == BL_STATUS_LINKING){ //连接中
    		display_gb1212(45,35,"詹连接中詹",YELLOW,BLACK);//

    	}else{//连接成功
    		display_gb1212(45,35,"詹连接成功",BLUE,BLACK);//
    	}
    	em_bl_state_old = em_bl_state;//记录蓝牙状态
    }

	//----------按键处理---------------
	if(uckey_value == KEY_FM_SHORT_PRESS){  //FM键 短按 +
	    uckey_value = KEY_NONE;
	    menu_exit_count = 0;   //菜单退出计数
	    bl_binding();

    }else if(uckey_value == KEY_BL_SHORT_PRESS){  //蓝牙键 短按 -
	    uckey_value = KEY_NONE;
	    menu_exit_count = 0;   //菜单退出计数
	    bl_bind_init();

    }else if(uckey_value == KEY_MAIN_SHORT_PRESS){  //主键 短按
	    uckey_value = KEY_NONE;
	    menu_exit_count = 0;   //菜单退出计数
	    em_work_mode = WORK_MODE_NORMAL; //正常模式
	 	//写入FLASH数据
        //disp_clear_screen(BLACK);
	 	disp_rectangle(39,0,1,80,LGRAY);//
	 	disp_rectangle(120,0,1,80,LGRAY);
	 	disp_rectangle(40,0,80,80,DCYAN);
	 	if(em_FM_switch == SWITCH_OPEN){//FM广播开关
	 	     disp_image_3232(128,45,gImage_FM,GRAYBLUE,BLACK);
	 	     if(uc_fm_channel>99){//三位数
	 	   	    disp_buff[0] = (uc_fm_channel/100)+0x30;
	 	   	    disp_buff[1] = (uc_fm_channel/10%10)+0x30;
	 	   	    disp_buff[2] = (uc_fm_channel%10)+0x30;
	 	   	    disp_buff[3] = 0;
	 	   	    display_gb1212(134,58,(const char*)disp_buff,BLUE,BLACK);//数字显示
	 	     }else{ //二位数
	 	   	    disp_buff[0] = (uc_fm_channel/10%10)+0x30;
	 	   	    disp_buff[1] = (uc_fm_channel%10)+0x30;
	 	   	    disp_buff[2] = 0;
	 	   	    display_gb1212(141,58,(const char*)disp_buff,BLUE,BLACK);//数字显示
	 	     }
	 	}else{
	 	   	 disp_image_3232(128,45,gImage_FM,GRAY,BLACK);
	 	}
	 	if(em_BL_switch == SWITCH_OPEN){//蓝牙开关
	 		if(em_bl_state == BL_STATUS_LINK_OK){ //连接成功
	 			disp_image_3232(2,45,gImage_BL_ON,GRAYBLUE,BLACK);
	 		}else{
	 			disp_image_3232(2,45,gImage_BL_OFF,GRAYBLUE,BLACK);
	 		}

	 	}else{
	 	    disp_image_3232(2,45,gImage_BL_OFF,GRAY,BLACK);
	 	}
	 	if(em_play_switch == SWITCH_OPEN){  //方糖播放开关
	 	    disp_image_3232(2,5,gImage_audio,GRAYBLUE,BLACK);
	 	}else{
	 	   	disp_image_3232(2,5,gImage_audio,GRAY,BLACK);
	 	}
	 	if(em_audio_mode == AUDIO_DECODING){  //音频处理模式 音频解码
	 	   	disp_image_3232(128,5,gImage_headset,GRAYBLUE,BLACK);
	 	   	display_gb1616(64,2,"模式",BLUE,DCYAN);//LGRAY
	 	   	disp_image_8040(40,20,gImage_music,MAGENTA,DCYAN);//LIGHTGREEN
	 	   	display_gb1212(56,67,"方糖解码",GREEN,DCYAN);//RED
	 	}else{
	 	   	disp_image_3232(128,5,gImage_headset,GRAY,BLACK);
	 	   	display_gb1616(64,2,"模式",BLUE,DCYAN);//LGRAY
	 	   	disp_image_8040(40,20,gImage_35mm,YELLOW,DCYAN);//LIGHTGREEN
	 	   	display_gb1212(56,67,"音频输入",GREEN,DCYAN);//RED
	 	}

	}else if(uckey_value == KEY_BL_LONG_PRESS){   //蓝牙键 长按
		uckey_value = KEY_NONE;
		menu_exit_count = 0;   //菜单退出计数
    }else if(uckey_value == KEY_FM_LONG_PRESS){   //FM键 长按
		uckey_value = KEY_NONE;
		menu_exit_count = 0;   //菜单退出计数
	}else if(uckey_value == KEY_MAIN_LONG_PRESS){ //主键 长按
		uckey_value = KEY_NONE;
		menu_exit_count = 0;   //菜单退出计数
	}
	menu_exit_count++;   //菜单退出计数
	if(menu_exit_count > (30*1000/10)){//30S无操作自动退出
		em_work_mode = WORK_MODE_NORMAL; //正常模式

		//disp_clear_screen(BLACK);
	    disp_rectangle(39,0,1,80,LGRAY);//
	    disp_rectangle(120,0,1,80,LGRAY);
		disp_rectangle(40,0,80,80,DCYAN);
		if(em_FM_switch == SWITCH_OPEN){//FM广播开关
			 disp_image_3232(128,45,gImage_FM,GRAYBLUE,BLACK);
			 if(uc_fm_channel>99){//三位数
			 	disp_buff[0] = (uc_fm_channel/100)+0x30;
			 	disp_buff[1] = (uc_fm_channel/10%10)+0x30;
			 	disp_buff[2] = (uc_fm_channel%10)+0x30;
			 	disp_buff[3] = 0;
			 	display_gb1212(134,58,(const char*)disp_buff,BLUE,BLACK);//数字显示
			 }else{ //二位数
			 	disp_buff[0] = (uc_fm_channel/10%10)+0x30;
			 	disp_buff[1] = (uc_fm_channel%10)+0x30;
			 	disp_buff[2] = 0;
			 	display_gb1212(141,58,(const char*)disp_buff,BLUE,BLACK);//数字显示
			 }
		}else{
			 	disp_image_3232(128,45,gImage_FM,GRAY,BLACK);
	    }
	    if(em_BL_switch == SWITCH_OPEN){//蓝牙开关
			 if(em_bl_state == BL_STATUS_LINK_OK){ //连接成功
			 	disp_image_3232(2,45,gImage_BL_ON,GRAYBLUE,BLACK);
			 }else{
			 	disp_image_3232(2,45,gImage_BL_OFF,GRAYBLUE,BLACK);
			 }
		}else{
			 disp_image_3232(2,45,gImage_BL_OFF,GRAY,BLACK);
		}
		if(em_play_switch == SWITCH_OPEN){  //方糖播放开关
			 disp_image_3232(2,5,gImage_audio,GRAYBLUE,BLACK);
		}else{
			 disp_image_3232(2,5,gImage_audio,GRAY,BLACK);
		}
	    if(em_audio_mode == AUDIO_DECODING){  //音频处理模式 音频解码
			 disp_image_3232(128,5,gImage_headset,GRAYBLUE,BLACK);
			 display_gb1616(64,2,"模式",BLUE,DCYAN);//LGRAY
			 disp_image_8040(40,20,gImage_music,MAGENTA,DCYAN);//LIGHTGREEN
			 display_gb1212(56,67,"方糖解码",GREEN,DCYAN);//RED
	    }else{
			 disp_image_3232(128,5,gImage_headset,GRAY,BLACK);
			 display_gb1616(64,2,"模式",BLUE,DCYAN);//LGRAY
			 disp_image_8040(40,20,gImage_35mm,YELLOW,DCYAN);//LIGHTGREEN
			 display_gb1212(56,67,"音频输入",GREEN,DCYAN);//RED
		}
	}
}
/*******************************************
 * 函数名：work_mode_fm_set
 * 描述  ：  FM设置
 * 输入  ：  无
 * 输出  ：  无
 * 履历  ：  V0.00  2019-5-1
********************************************/
static void work_mode_fm_set(void)
{
	uint8_t disp_buff[5];
	//----------按键处理---------------
	if(uckey_value == KEY_FM_SHORT_PRESS){  //FM键 短按 +
	   uckey_value = KEY_NONE;
	   menu_exit_count = 0;   //菜单退出计数
	   uc_fm_channel_old++;
	   if(uc_fm_channel_old > 108){
		   uc_fm_channel_old = 76;
	   }
	   if(uc_fm_channel_old>99){//三位数
	  	   disp_buff[0] = (uc_fm_channel_old/100)+0x30;
	  	   disp_buff[1] = (uc_fm_channel_old/10%10)+0x30;
	  	   disp_buff[2] = (uc_fm_channel_old%10)+0x30;
	  	   disp_buff[3] = 0;
	  }else{ //二位数
	  	   disp_buff[0] = ' ';
	  	   disp_buff[1] = (uc_fm_channel_old/10%10)+0x30;
	  	   disp_buff[2] = (uc_fm_channel_old%10)+0x30;
	  	   disp_buff[3] = 0;
	  }
	  display_gb1616(42,30,(const char*)disp_buff,YELLOW,BLACK);//数字显示

	}else if(uckey_value == KEY_BL_SHORT_PRESS){  //蓝牙键 短按 -
	   uckey_value = KEY_NONE;
	   menu_exit_count = 0;   //菜单退出计数
	   uc_fm_channel_old--;
	   if(uc_fm_channel_old < 76){
	   	   uc_fm_channel_old = 108;
	   }
	   if(uc_fm_channel_old>99){//三位数
	  	   disp_buff[0] = (uc_fm_channel_old/100)+0x30;
	  	   disp_buff[1] = (uc_fm_channel_old/10%10)+0x30;
	  	   disp_buff[2] = (uc_fm_channel_old%10)+0x30;
	  	   disp_buff[3] = 0;
	  }else{ //二位数
	  	   disp_buff[0] = ' ';
	  	   disp_buff[1] = (uc_fm_channel_old/10%10)+0x30;
	  	   disp_buff[2] = (uc_fm_channel_old%10)+0x30;
	  	   disp_buff[3] = 0;
	  }
	  display_gb1616(42,30,(const char*)disp_buff,YELLOW,BLACK);//数字显示

	}else if(uckey_value == KEY_MAIN_SHORT_PRESS){  //主键 短按
	   uckey_value = KEY_NONE;
	   menu_exit_count = 0;   //菜单退出计数
	   em_work_mode = WORK_MODE_NORMAL; //正常模式
	   if(uc_fm_channel != uc_fm_channel_old){
	      uc_fm_channel = uc_fm_channel_old;
	      falsh_write_flag = 0x5A; //flash数据写标志
	      qn8027_fm_channel_set_function();//频率设置
	   }
	   //disp_clear_screen(BLACK);
	   disp_rectangle(39,0,1,80,LGRAY);//
	   disp_rectangle(120,0,1,80,LGRAY);
	   disp_rectangle(40,0,80,80,DCYAN);
	   if(em_FM_switch == SWITCH_OPEN){//FM广播开关
	   		disp_image_3232(128,45,gImage_FM,GRAYBLUE,BLACK);
	   		if(uc_fm_channel>99){//三位数
	   			disp_buff[0] = (uc_fm_channel/100)+0x30;
	   			disp_buff[1] = (uc_fm_channel/10%10)+0x30;
	   			disp_buff[2] = (uc_fm_channel%10)+0x30;
	   			disp_buff[3] = 0;
	   			display_gb1212(134,58,(const char*)disp_buff,BLUE,BLACK);//数字显示
	   		}else{ //二位数
	   			disp_buff[0] = (uc_fm_channel/10%10)+0x30;
	   			disp_buff[1] = (uc_fm_channel%10)+0x30;
	   			disp_buff[2] = 0;
	   			display_gb1212(141,58,(const char*)disp_buff,BLUE,BLACK);//数字显示
	   	   }
	   }else{
	   		 disp_image_3232(128,45,gImage_FM,GRAY,BLACK);
	   }
	   if(em_BL_switch == SWITCH_OPEN){//蓝牙开关
		   if(em_bl_state == BL_STATUS_LINK_OK){ //连接成功
		   	   disp_image_3232(2,45,gImage_BL_ON,GRAYBLUE,BLACK);
		   }else{
		   	   disp_image_3232(2,45,gImage_BL_OFF,GRAYBLUE,BLACK);
		   }
	   }else{
	   		 disp_image_3232(2,45,gImage_BL_OFF,GRAY,BLACK);
	   }
	   if(em_play_switch == SWITCH_OPEN){  //方糖播放开关
	   		 disp_image_3232(2,5,gImage_audio,GRAYBLUE,BLACK);
	   }else{
	   		 disp_image_3232(2,5,gImage_audio,GRAY,BLACK);
	   }
	   if(em_audio_mode == AUDIO_DECODING){  //音频处理模式 音频解码
	   		 disp_image_3232(128,5,gImage_headset,GRAYBLUE,BLACK);
	   		 display_gb1616(64,2,"模式",BLUE,DCYAN);//LGRAY
	   		 disp_image_8040(40,20,gImage_music,MAGENTA,DCYAN);//LIGHTGREEN
	   		 display_gb1212(56,67,"方糖解码",GREEN,DCYAN);//RED
	   }else{
	   		 disp_image_3232(128,5,gImage_headset,GRAY,BLACK);
	   		 display_gb1616(64,2,"模式",BLUE,DCYAN);//LGRAY
	   		 disp_image_8040(40,20,gImage_35mm,YELLOW,DCYAN);//LIGHTGREEN
	   		 display_gb1212(56,67,"音频输入",GREEN,DCYAN);//RED
	   }


	}else if(uckey_value == KEY_BL_LONG_PRESS){   //蓝牙键 长按
		  uckey_value = KEY_NONE;
		  menu_exit_count = 0;   //菜单退出计数
	}else if(uckey_value == KEY_FM_LONG_PRESS){   //FM键 长按
		  uckey_value = KEY_NONE;
		  menu_exit_count = 0;   //菜单退出计数
	}else if(uckey_value == KEY_MAIN_LONG_PRESS){ //主键 长按
		  uckey_value = KEY_NONE;
		  menu_exit_count = 0;   //菜单退出计数
	}
	menu_exit_count++;   //菜单退出计数
	if(menu_exit_count > (30*1000/10)){//30S无操作自动退出
		em_work_mode = WORK_MODE_NORMAL; //正常模式
		//disp_clear_screen(BLACK);
	    disp_rectangle(39,0,1,80,LGRAY);//
	    disp_rectangle(120,0,1,80,LGRAY);
		disp_rectangle(40,0,80,80,DCYAN);
		if(em_FM_switch == SWITCH_OPEN){//FM广播开关
			 disp_image_3232(128,45,gImage_FM,GRAYBLUE,BLACK);
			 if(uc_fm_channel>99){//三位数
			 	disp_buff[0] = (uc_fm_channel/100)+0x30;
			 	disp_buff[1] = (uc_fm_channel/10%10)+0x30;
			 	disp_buff[2] = (uc_fm_channel%10)+0x30;
			 	disp_buff[3] = 0;
			 	display_gb1212(134,58,(const char*)disp_buff,BLUE,BLACK);//数字显示
			 }else{ //二位数
			 	disp_buff[0] = (uc_fm_channel/10%10)+0x30;
			 	disp_buff[1] = (uc_fm_channel%10)+0x30;
			 	disp_buff[2] = 0;
			 	display_gb1212(141,58,(const char*)disp_buff,BLUE,BLACK);//数字显示
			 }
		}else{
			 	disp_image_3232(128,45,gImage_FM,GRAY,BLACK);
	    }
	    if(em_BL_switch == SWITCH_OPEN){//蓝牙开关
			 if(em_bl_state == BL_STATUS_LINK_OK){ //连接成功
			 	disp_image_3232(2,45,gImage_BL_ON,GRAYBLUE,BLACK);
			 }else{
			 	disp_image_3232(2,45,gImage_BL_OFF,GRAYBLUE,BLACK);
			 }
		}else{
			 disp_image_3232(2,45,gImage_BL_OFF,GRAY,BLACK);
		}
		if(em_play_switch == SWITCH_OPEN){  //方糖播放开关
			 disp_image_3232(2,5,gImage_audio,GRAYBLUE,BLACK);
		}else{
			 disp_image_3232(2,5,gImage_audio,GRAY,BLACK);
		}
	    if(em_audio_mode == AUDIO_DECODING){  //音频处理模式 音频解码
			 disp_image_3232(128,5,gImage_headset,GRAYBLUE,BLACK);
			 display_gb1616(64,2,"模式",BLUE,DCYAN);//LGRAY
			 disp_image_8040(40,20,gImage_music,MAGENTA,DCYAN);//LIGHTGREEN
			 display_gb1212(56,67,"方糖解码",GREEN,DCYAN);//RED
	    }else{
			 disp_image_3232(128,5,gImage_headset,GRAY,BLACK);
			 display_gb1616(64,2,"模式",BLUE,DCYAN);//LGRAY
			 disp_image_8040(40,20,gImage_35mm,YELLOW,DCYAN);//LIGHTGREEN
			 display_gb1212(56,67,"音频输入",GREEN,DCYAN);//RED
		}
	}

}
/*******************************************
 * 函数名：work_mode_normal
 * 描述  ：  正常模式
 * 输入  ：  无
 * 输出  ：  无
 * 履历  ：  V0.00  2019-4-13
********************************************/
//static void work_mode_select(void)
//{
//
//
//}

/*******************************************
 * 函数名：task_control_power_on_function
 * 描述  ：  主处理  上电初始化
 * 输入  ：无
 * 输出  ：无
 * 履历  ：V0.00  2019-4-13
 ********************************************/
void task_control_power_on_function(void)
{
	flash_read_handle();
	first_power_on_flag = 1;//初次上电标志位
	uc_lcd_init_state = 0;//用于液晶初始化判断
	uc_lcd_init_count = 0;//用于液晶初始化计数
	em_work_mode = WORK_MODE_SCREEN_INIT;//液晶初始化
	LCD_POWER_ON;
	bl_data_handle_init();
	qn8027_init_function();
}

/*******************************************
 * 函数名：task_control_main_function
 * 描述  ：  主处理函数
 * 输入  ：  无
 * 输出  ：  无
 * 履历  ：  V0.00  2019-4-13
********************************************/
void task_control_main_function(void)
{
	bl_control_function();

	switch(em_work_mode){
	    case WORK_MODE_SCREEN_INIT:    //液晶初始化
	    	work_mode_screen_init();
		    break;
	   	case WORK_MODE_POWER_ON:        //开机logo
	   		work_mode_power_on();
	   	    break;
	   	case WORK_MODE_SCREEN_CLOSE:   //息屏
	   		work_mode_screen_close();
	   	   	break;
	   	case WORK_MODE_NORMAL:   	   //正常模式
	   		work_mode_normal();
	   		break;
	   	case WORK_MODE_BL:   	       //蓝牙设置
	   		work_mode_bl_set();
	   		break;
	   	case WORK_MODE_FM:   	       //FM设置
	   		work_mode_fm_set();
	   		break;
	   //	case WORK_MODE_SELECT:         //模式选择
	   //		work_mode_select();
	   //		break;
	   	   default:
	   	       break;
	}

}

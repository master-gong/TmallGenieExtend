/*
 * key_handle.c
 *
 *  Created on: 2019年5月1日
 *      Author: gongjunhui
 */
#include "main.h"
#include "key_handle.h"

volatile  _key_value_em  uckey_value;   /* 按键键值  */

//按键计数
static  uint8_t key_fm_count;
static  uint8_t key_bl_count;
static  uint8_t key_main_count;
//按键计数允许标志
static  uint8_t key_fm_en_flag;
static  uint8_t key_bl_en_flag;
static  uint8_t key_main_en_flag;

/*******************************************
 * 函数名：ISR_key_handle_function
 * 描述  ：  按键处理函数 在中断中调用  10ms一次
 * 输入  ：  无
 * 输出  ：  无
 * 履历  ：  V0.00  2019-5-1
********************************************/
void ISR_key_handle_function(void)
{
   if(uckey_value != KEY_NONE){ //当存在键值不在检测新的键值
//	   key_fm_count = 0;
//	   key_bl_count = 0;
//	   key_main_count = 0;
	   return;
   }
   //----------蓝牙健判断----------------
   if(KEY_BL==GPIO_PIN_RESET){
	   if(key_bl_en_flag==1){
		   return;
	   }
	   key_bl_count++;
	   if(key_bl_count>(2000/10)){//2S
	   	   uckey_value = KEY_BL_LONG_PRESS;   //BL键 长按
	   	   key_bl_en_flag=1;
	   	   key_bl_count=0;
	   }
   }else{
	   if(key_bl_count>(50/10)){
		   uckey_value = KEY_BL_SHORT_PRESS;  //BL键 短按
	   }
	   key_bl_count=0;
	   key_bl_en_flag = 0;
   }
   //----------FM键判断----------------
   if(KEY_FM==GPIO_PIN_RESET){
	   if(key_fm_en_flag==1){
	   		return;
	   }
   	   key_fm_count++;
   	   if(key_fm_count>(2000/10)){//2S
   	   	   uckey_value = KEY_FM_LONG_PRESS;   //BL键 长按
   	   	   key_fm_en_flag=1;
   	   	   key_fm_count=0;
   	   }
   }else{
   	   if(key_fm_count>(50/10)){
   		   uckey_value = KEY_FM_SHORT_PRESS;  //BL键 短按
   	   }
   	   key_fm_count=0;
   	   key_fm_en_flag = 0;
   }
   //----------主键判断----------------
   if(KEY_MAIN==GPIO_PIN_RESET){
	   if(key_main_en_flag==1){
	  	  return;
	   }
   	   key_main_count++;
   	   if(key_main_count>(2000/10)){//2S
   	   	   uckey_value = KEY_MAIN_LONG_PRESS;   //主键 长按
   	   	   key_main_en_flag=1;
   	   	   key_main_count=0;
   	   }
   }else{
   	   if(key_main_count>(50/10)){
   		   uckey_value = KEY_MAIN_SHORT_PRESS;  //主键 短按
   	   }
   	   key_main_count=0;
   	   key_main_en_flag = 0;
   }

}



/*
 * control_main.h
 *
 *  Created on: 2019年4月13日
 *      Author: gongjunhui
 */

#ifndef CONTROL_MAIN_H_
#define CONTROL_MAIN_H_


#include "main.h"

#define   COMM_UART_ID        (huart1)     // 通讯 uart id


typedef enum {           //器件开关
    SWITCH_OPEN         ,  //打开
	SWITCH_CLOSE        ,  //关闭
}_device_switch_em;

typedef enum {           //音频处理模式
    AUDIO_DECODING      ,  //音频解码
	AUDIO_INPUT         ,  //音频输入
}_audio_mode_em;


typedef enum {           //工作模式 显示模式
	WORK_MODE_SCREEN_INIT      ,  //液晶初始化
	WORK_MODE_POWER_ON     	   ,  //开机logo
    WORK_MODE_SCREEN_CLOSE     ,  //息屏
	WORK_MODE_NORMAL           ,  //正常模式
	//WORK_MODE_MENU             ,  //菜单
	//WORK_MODE_SELECT     	   ,  //模式选择
	//WORK_MODE_COURSE     	   ,  //教程
	WORK_MODE_BL     	       ,  //蓝牙设置
	WORK_MODE_FM     	       ,  //FM设置
}_work_mode_em;

//extern void send_read_motor_data(_read_parameter_st  st_read_data);
extern _device_switch_em  em_BL_switch;//蓝牙开关
extern _device_switch_em  em_FM_switch;//FM广播开关

extern void task_control_power_on_function(void);
extern void task_control_main_function(void);

#endif /* CONTROL_MAIN_H_ */

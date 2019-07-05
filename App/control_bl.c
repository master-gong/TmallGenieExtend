/*
 * control_bl.c
 *
 *  Created on: 2019年5月2日
 *      Author: gongjunhui
 */
#include "control_main.h"
#include "control_bl.h"
#include "main.h"
#include "usart.h"
#include <string.h>



//-----------数据接收----------
static  uint8_t  Host_Rec_buff[200];    /* 接收缓存 */
static  uint8_t  Host_Rec_byte[2];      /* 接收byte */
static  uint16_t  Rec_Count = 0;  //接收计数
static  volatile  uint8_t  Rec_OK_Flag = 0;               /* 接收完成标志  */
static  volatile  uint32_t  frame_interval_time=0;        /* 帧间隔计数  */
static  volatile  uint32_t  frame_interval_time_flag=0;   /* 帧间隔计数标志  */

static uint8_t bl_init_flag;    //初始化
static uint8_t bl_restart_flag; //重启标志
static uint8_t bl_delay_count;  //延时计数

_bl_status_em  em_bl_state;//蓝牙模块状态

const char*  str_delvmlink= "AT+DELVMLINK";

/*******************************************
 * 函数名：received_data_Reset
 * 描述  ：  数据接收清零复位
 * 输入  ： 无
 * 输出  ： 无
 * 履历　：V0.00  2019-5-2
********************************************/
static void received_data_Reset(void)
{
	memset(Host_Rec_buff, 0, sizeof(Host_Rec_buff));   //清空缓存区
    Rec_OK_Flag =0;
	Rec_Count=0;
	//Data_Length =0;
	frame_interval_time=0;        /* 帧间隔计数  */
	frame_interval_time_flag=0;   /* 帧间隔计数标志  */

	//HAL_UART_Receive_IT(&MESH_UART_ID,(uint8_t *)Host_Rec_byte,1);
}


/*******************************************
 * 函数名：received_data_processing
 * 描述  ： 收到的数据处理
 * 输入  ： 无
 * 输出  ： 无
 * 履历　：V0.00  2019-5-2
********************************************/
static void  received_data_processing(void)
{

    if(Rec_OK_Flag == 0){
	    return;
    }
    if(strstr((char const*)Host_Rec_buff,"CONNECTED") != NULL){
    	em_bl_state = BL_STATUS_LINK_OK;  //连接成功

    }else if(strstr((char const*)Host_Rec_buff,"POWER ON") != NULL){
        em_bl_state = BL_STATUS_LINKING;  //连接中

    }else if(strstr((char const*)Host_Rec_buff,"DISCONNECT") != NULL){
        em_bl_state = BL_STATUS_LINKING;  //连接中

    }else{

    }

    received_data_Reset();
}


/*******************************************
 * 函数名：bl_data_handle_init
 * 描述  ：
 * 输入  ： 无
 * 输出  ： 无
 * 履历　：V0.00  2019-5-2
********************************************/
void bl_data_handle_init(void)
{
	received_data_Reset();
	while(HAL_OK != HAL_UART_Receive_IT(&huart1,(uint8_t *)Host_Rec_byte,1));

}
/*******************************************
 * 函数名：bl_control_function
 * 描述  ：  蓝牙处理函数
 * 输入  ：无
 * 输出  ：无
 * 履历　：V0.00  2019-5-2
 ********************************************/
void bl_control_function(void)
{
   if(em_BL_switch == SWITCH_OPEN){//打开
	  BL_POWER_ON;
   }else{         //关闭
	  BL_POWER_OFF;
   }
   received_data_processing(); //收到本体数据的处理

   if(bl_init_flag == 0x5A){
	   bl_restart_flag=0;
	   bl_delay_count++;
	   if(bl_delay_count == 1){
		   HAL_UART_Transmit_IT(&huart1,(uint8_t*)str_delvmlink,strlen(str_delvmlink));
	   }else if(bl_delay_count <= (500/10)){
		   em_BL_switch = SWITCH_OPEN;
	   }else if(bl_delay_count <= (1500/10)){
		   em_BL_switch = SWITCH_CLOSE;
	   }else{
		   bl_delay_count = 0;
		   bl_init_flag = 0;
		   em_BL_switch = SWITCH_OPEN;
	   }
   }

   if(bl_restart_flag == 0x5A){
	   bl_delay_count++;
	   if(bl_delay_count <= (1000/10)){
		   em_BL_switch = SWITCH_CLOSE;
	   }else{
		   bl_delay_count = 0;
		   bl_restart_flag = 0;
		   em_BL_switch = SWITCH_OPEN;
	   }
   }


}

/*******************************************
 * 函数名：bl_bind_init
 * 描述  ： 蓝牙模块重新绑定
 * 输入  ： 无
 * 输出  ： 无
 * 履历　：V0.00  2019-5-2
********************************************/
void bl_bind_init(void)
{
	bl_init_flag = 0x5A;    //初始化
	bl_delay_count = 0;     //延时计数
	em_bl_state = BL_STATUS_RESET;    //重启
}
/*******************************************
 * 函数名：bl_binding
 * 描述  ： 蓝牙模块绑定
 * 输入  ： 无
 * 输出  ： 无
 * 履历　：V0.00  2019-5-2
********************************************/
void bl_binding(void)
{
   bl_restart_flag = 0x5A;//重启标志
   bl_delay_count = 0;  //延时计数
   em_bl_state = BL_STATUS_RESTART;  //出厂设置
}
/********************************
 * 函数名: UART_RxTime
 * 说明:
 ********************************/
void UART_RxTime(void)
{
	if(frame_interval_time_flag == 1){
		frame_interval_time ++ ;        /* 帧间隔计数  */
		if(frame_interval_time>=30){    //30ms帧间隔
			Rec_OK_Flag = 1;
			frame_interval_time_flag=0;   /* 帧间隔计数标志  */
		}
	}
}

/********************************
 * 函数名： UART_RxCallback
 * 说明:  通讯模组 接收中断回调函数
 ********************************/
static void UART_RxCallback(void)
{

	frame_interval_time=0;        /* 帧间隔计数  */
	frame_interval_time_flag=1;   /* 帧间隔计数标志  */

	Host_Rec_buff[Rec_Count] = Host_Rec_byte[0];
	if(Rec_OK_Flag == 0){
	    Rec_Count++;
	}
	if(Rec_Count > 198){
		Rec_OK_Flag = 1;
		frame_interval_time_flag=0;   /* 帧间隔计数标志  */
	}

	while(HAL_OK != HAL_UART_Receive_IT(&huart1,(uint8_t *)Host_Rec_byte,1));
}



/********************************
 * 函数名：HAL_UART_RxCpltCallback
 * 说明: 接收中断回调函数
 ********************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{

	if(UartHandle == &huart1){
		UART_RxCallback();
	}
}


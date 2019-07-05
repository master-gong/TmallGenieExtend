/*
 * control_bl.h
 *
 *  Created on: 2019年5月2日
 *      Author: gongjunhui
 */

#ifndef CONTROL_BL_H_
#define CONTROL_BL_H_

typedef enum {             //蓝牙模块状态
	BL_STATUS_RESET       ,  //出厂恢复中
	BL_STATUS_RESTART     ,  //重启中
    BL_STATUS_LINKING     ,  //连接中
	BL_STATUS_LINK_OK     ,  //连接成功
}_bl_status_em;


extern _bl_status_em  em_bl_state;//蓝牙模块状态

extern void UART_RxTime(void);
extern void bl_control_function(void);
extern void bl_data_handle_init(void);
extern void bl_bind_init(void);
extern void bl_binding(void);

#endif /* CONTROL_BL_H_ */

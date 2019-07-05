/*
 * key_handle.h
 *
 *  Created on: 2019年5月1日
 *      Author: gongjunhui
 */

#ifndef KEY_HANDLE_H_
#define KEY_HANDLE_H_


typedef enum{       //按键键值
	KEY_NONE              ,  //没有键值
    KEY_FM_SHORT_PRESS    ,  //FM键 短按
	KEY_FM_LONG_PRESS     ,  //FM键 长按
	KEY_BL_SHORT_PRESS    ,  //BL键 短按
	KEY_BL_LONG_PRESS     ,  //BL键 长按
	KEY_MAIN_SHORT_PRESS  ,  //主键 短按
	KEY_MAIN_LONG_PRESS   ,  //主键 长按
}_key_value_em;


extern volatile  _key_value_em  uckey_value;   /* 按键键值  */

extern void ISR_key_handle_function(void);

#endif /* KEY_HANDLE_H_ */

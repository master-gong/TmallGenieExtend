/*
 * GB1212.h
 *
 *  Created on: 2018年5月27日
 *      Author: gongjunhui
 */

#ifndef GB1212_H_
#define GB1212_H_
#include "main.h"
//---------------------------------------------------------------------------------
//------------------------------ ASCII字库 ---------------------------------------
//--------------------------------------------------------------------------------
typedef struct{
		unsigned char Head[1];         /* 索引 */
		unsigned char Infor[12];       /* 点阵信息 */
}_GB2312Type6x12;


// ------------------  汉字字模的数据结构定义 ------------------------ //
typedef struct{                // 汉字字模数据结构
      unsigned char   Head[2];               // 汉字内码索引
      unsigned char   Infor[24];             // 点阵码数据
}_GB2312Type12x12;

extern const _GB2312Type6x12  GB2312Code6x12[];
extern const _GB2312Type12x12 GB2312Code12x12[];
extern const uint16_t len_code6x12;
extern const uint16_t len_code12x12;

#endif /* LCD_1_44_GB1212_H_ */

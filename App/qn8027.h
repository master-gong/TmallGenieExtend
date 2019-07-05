/*
 * qn8027.h
 *
 *  Created on: 2019年4月30日
 *      Author: gongjunhui
 */

#ifndef QN8027_H_
#define QN8027_H_

#define I2CDataIn        1
#define I2CDataOut       0

#define IIC_SDA_Data     HAL_GPIO_ReadPin(FM_SDA_GPIO_Port, FM_SDA_Pin)
#define IIC_SDA_1        HAL_GPIO_WritePin(FM_SDA_GPIO_Port, FM_SDA_Pin, GPIO_PIN_SET)
#define IIC_SDA_0        HAL_GPIO_WritePin(FM_SDA_GPIO_Port, FM_SDA_Pin, GPIO_PIN_RESET)
#define IIC_SCL_1        HAL_GPIO_WritePin(FM_SCL_GPIO_Port, FM_SCL_Pin, GPIO_PIN_SET)
#define IIC_SCL_0        HAL_GPIO_WritePin(FM_SCL_GPIO_Port, FM_SCL_Pin, GPIO_PIN_RESET)


extern uint8_t  uc_fm_channel;  //fm发射频率  76Mhz - 108 Mhz 只取整数


extern void qn8027_init_function(void);
extern void task_qn8027_function(void);
extern void qn8027_fm_channel_set_function(void);

#endif /* QN8027_H_ */

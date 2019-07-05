/*
 * qn8027.c
 *
 *  Created on: 2019年4月30日
 *      Author: gongjunhui
 */


#include "main.h"
#include "qn8027.h"
#include "control_main.h"


static uint8_t uc_qn8027_init_flag;   //芯片初始化标志
static uint8_t uc_qn8027_set_channel_flag;   //芯片FM频道标志
static uint8_t uc_qn8027_init_state;  //芯片初始化状态
static uint8_t uc_qn8027_delay_count; //芯片延时计数

static uint16_t us_qn8027_channel=256;//发射频道0-640 =》 FM =(76Mhz + 0.05*channel)
uint8_t  uc_fm_channel = 100;    //fm发射频率  76Mhz - 108 Mhz 只取整数
static uint8_t  first_power_on;  //初次上电标志位

static uint8_t* p_ttt= &uc_fm_channel;

#include "gpio.h"


static void delay_5us(unsigned int nCount)
{
    while (nCount != 0)
    {
     __ASM("NOP");
     __ASM("NOP");
	 __ASM("NOP");
	 __ASM("NOP");
	 __ASM("NOP");
	 __ASM("NOP");

	 //------------
	 __ASM("NOP");
	 __ASM("NOP");
	 __ASM("NOP");
	 //------------

	 nCount--;
    }
    p_ttt= &uc_fm_channel;
}

//***************************************************************
// I2C 引脚初始化
//***************************************************************
static void I2CPort_Init(void)
{
	 GPIO_InitTypeDef GPIO_InitStruct;
	/*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(FM_SCL_GPIO_Port, FM_SCL_Pin, GPIO_PIN_SET);
	//-----------------------_SCL--------------------
	/*Configure GPIO pin : PtPin */
	  GPIO_InitStruct.Pin = FM_SCL_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(FM_SCL_GPIO_Port, &GPIO_InitStruct);
}

//***************************************************************
// I2C Data input/output
// 0-Output, 1-Input
//***************************************************************
static void I2CDataInOu(unsigned char InOu)
{
	  GPIO_InitTypeDef GPIO_InitStruct;
      if(InOu==1){
    	  GPIO_InitStruct.Pin = FM_SDA_Pin;
          GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
          GPIO_InitStruct.Pull = GPIO_NOPULL;
          HAL_GPIO_Init(FM_SDA_GPIO_Port, &GPIO_InitStruct);

      }else{
    	  GPIO_InitStruct.Pin = FM_SDA_Pin;
    	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    	  HAL_GPIO_Init(FM_SDA_GPIO_Port, &GPIO_InitStruct);
      }
}

//***************************************************************
//  Send start condition
//  ensure data is high then issue a start condition
//  see also i2c_Start() macro
//***************************************************************
static void I2C_Start(void)
{
  I2CDataInOu(I2CDataOut);
  IIC_SDA_1;
  IIC_SCL_1;
  delay_5us(10);//4.7us
  IIC_SDA_0;
  delay_5us(10);//4us
  IIC_SCL_0;
}
//***************************************************************
//   Send stop condition
//   data low-high while clock high
//***************************************************************
static void I2C_Stop (void)
{
 I2CDataInOu(I2CDataOut);
 IIC_SDA_0;
 IIC_SCL_1;
 delay_5us(10);
 IIC_SDA_1;
 delay_5us(10);

}

//***************************************************************
//ROUTINE NAME : I2C_Ack
//INPUT/OUTPUT : None.
//DESCRIPTION  : Acknoledge generation fro m now.
//COMMENTS     : Transfer sequence = DATA, ACK.
//***************************************************************
static void I2C_Rack(void)
{
 //I2CDataInOu(I2CDataOut);
 IIC_SDA_0;
 delay_5us(3);
 IIC_SCL_1;
 delay_5us(10);
 IIC_SCL_0;
}

//***************************************************************
//ROUTINE NAME : I2C_nAck
//INPUT/OUTPUT : None.
//DESCRIPTION  : Non acknoledge generation from now.
//COMMENTS     : Transfer sequence = DATA, NACK.
//***************************************************************
static void I2C_nAck (void)
{
 IIC_SDA_1;
 delay_5us(3);
 IIC_SCL_1;
 delay_5us(3);
 IIC_SCL_0;
 delay_5us(3);
}

//***************************************************************
//  Send a byte to the slave 写一个数据没有应答
//  return I2C_ERR OR I2C_CRR//0x8c
//***************************************************************
static unsigned char IIC_SendByte(unsigned char I2cData)
{
  unsigned char i;
  unsigned char I2CStatus;

  for(i=0; i<8; i++)
  {
	IIC_SCL_0;
    delay_5us(6);
    if(I2cData & 0x80) IIC_SDA_1;
    else IIC_SDA_0;
    IIC_SCL_1;
    delay_5us(6);
    I2cData <<= 1;
  }
  IIC_SCL_0;
  I2CDataInOu(I2CDataIn);
  IIC_SCL_1;
  delay_5us(3);
  if(IIC_SDA_Data== 0) I2CStatus= 1;
  else I2CStatus=0;
  IIC_SCL_0;
  return I2CStatus;
}

//***************************************************************
//ROUTINE NAME : I2Cm_RxData
//INPUT/OUTPUT : Last byte to receive flag (active high)/Received data byte.
//DESCRIPTION  : Receive a data byte.
//COMMENTS     : Transfer sequence = DATA, ACK, EV7...
//***************************************************************
static unsigned char IIC_RcvByte(void)
{
    unsigned char i;
    unsigned char ReadByte=0;

   I2CDataInOu(I2CDataIn);
   delay_5us(6);
   for(i=0; i<8; i++)
   {
     ReadByte <<= 1;
     IIC_SCL_0;
     delay_5us(10);
     IIC_SCL_1;
     delay_5us(10);
     if(IIC_SDA_Data) ReadByte |= 0x01;
     delay_5us(3);
   }
   IIC_SCL_0;
   I2CDataInOu(I2CDataOut);
   delay_5us(3);
   return ReadByte;
}

//在指定地址读出一个数据
//ReadAddr:开始读数的地址
//返回值  :读到的数据
static uint8_t IIC_ReadOneByte(uint8_t ReadAddr)
{
	uint8_t temp=0;
    I2C_Start();
    IIC_SendByte(0X58);	   //发送写命令
	I2CDataInOu(I2CDataOut);
	IIC_SendByte(ReadAddr);//发送高地址
	I2C_Stop();//产生一个停止条件
	I2C_Start();
	IIC_SendByte(0X59);     //进入接收模式
    temp=IIC_RcvByte();
    I2C_Rack();
    I2C_Stop();//产生一个停止条件
	return temp;
}
//在指定地址写入一个数据
//WriteAddr  :写入数据的目的地址
//DataToWrite:要写入的数据
static void IIC_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite)
{
    I2C_Start();
    IIC_SendByte(0X58);	         //发送写命令 器件地址
    I2CDataInOu(I2CDataOut);
    IIC_SendByte(WriteAddr);     //发送寄存器地址
    I2CDataInOu(I2CDataOut);
    IIC_SendByte(DataToWrite);   //发送数据
    I2C_Stop();                  //产生一个停止条件
}


/*******************************************
 * 函数名：qn8027_ChipInitialization
 * 描述  ：  qn8027初始化
 * 输入  ：无
 * 输出  ：无
 * 履历  ：V0.00  2019-4-30
 ********************************************/
static void qn8027_ChipInitialization(void)
{
   if(uc_qn8027_init_flag != 0x5A){
	   return;
   }
   if((uc_fm_channel>=76)&&(uc_fm_channel<=108)){
	   us_qn8027_channel = (uc_fm_channel-76)*20;
   }
   //---------芯片初始化状态--------
   if(uc_qn8027_init_state == 0xff){
	   uc_qn8027_delay_count++;
	   if(uc_qn8027_delay_count<=(600/10)){
	   	   return;
	   }
	   uc_qn8027_init_state = 0;
   }
   else if(uc_qn8027_init_state == 0){//发送地址0x00 系统参数数据
	   IIC_WriteOneByte(0x00,((us_qn8027_channel>>8)&0x03)|0x80);
	   uc_qn8027_delay_count =0;
       uc_qn8027_init_state = 1;
   }
   else if(uc_qn8027_init_state == 1){//延时20ms
	   uc_qn8027_delay_count++;
	   if(uc_qn8027_delay_count<(20/10)){
		   return;
	   }
	   uc_qn8027_init_state = 3;
   }
   else if(uc_qn8027_init_state == 3){//时钟源设置
	   IIC_WriteOneByte(0x03,0x10);//选择晶振体
	   IIC_WriteOneByte(0x04,0x03);//03
	   IIC_WriteOneByte(0x00,((us_qn8027_channel>>8)&0x03) | 0x40);
	   IIC_WriteOneByte(0x00,((us_qn8027_channel>>8)&0x03) | 0x00);
	   uc_qn8027_init_state = 4;
	   uc_qn8027_delay_count =0;
   }
   else if(uc_qn8027_init_state == 4){//延时20ms
	   uc_qn8027_delay_count++;
	   if(uc_qn8027_delay_count<(20/10)){
		   return;
	   }
	   uc_qn8027_init_state = 5;
   }
   else if(uc_qn8027_init_state == 5){
	   IIC_WriteOneByte(0x10,0x4B);  //PA output power target is 0.62*PA_TRGT+71dBu. Valid values are 20-75.
	   IIC_WriteOneByte(0x18,0xE4);  //改善信噪比SNR
	   IIC_WriteOneByte(0x1B,0xF0);  //Increase maximum RF output power. (This is an undocumented register, and it really works.)
	   IIC_WriteOneByte(0x02,0xB9); //Disable RF PA off function.
	   IIC_WriteOneByte(0x01,us_qn8027_channel&0xff);//发射频率

	   uc_qn8027_init_state = 6;
   }
   else if(uc_qn8027_init_state == 6){//发射
    	IIC_WriteOneByte(0x00,((us_qn8027_channel>>8)&0x03)|0x20);
    	uc_qn8027_init_state = 11;
    	uc_qn8027_init_flag = 0;
   }
}


/*******************************************
 * 函数名：qn8027_set_channel_function
 * 描述  ：  qn8027频道设置
 * 输入  ：无
 * 输出  ：无
 * 履历  ：V0.00  2019-5-1
 ********************************************/
static void qn8027_set_channel_function(void)
{
	uint8_t REG_SYSTEM;
	if(uc_qn8027_init_flag != 0x5A){
		if(uc_qn8027_set_channel_flag == 0x5A){
			uc_qn8027_set_channel_flag = 0;
			REG_SYSTEM = IIC_ReadOneByte(0x00);
			REG_SYSTEM &= 0xDF;
			IIC_WriteOneByte(0x00, REG_SYSTEM);				//Disable transmission.
			REG_SYSTEM &= 0xFC;
			REG_SYSTEM |= ((us_qn8027_channel>>8)&0x03);
			IIC_WriteOneByte(0x00, REG_SYSTEM);				//Set the highest 2 bits of 10-bit channel index.
			IIC_WriteOneByte(0x01, (us_qn8027_channel&0x00FF));
			REG_SYSTEM |= 0x20;
			IIC_WriteOneByte(0x00, REG_SYSTEM);				//Enable transmission.
		}
   }

}
/*******************************************
 * 函数名：qn8027_init_function
 * 描述  ：  qn8027初始化
 * 输入  ：无
 * 输出  ：无
 * 履历  ：V0.00  2019-4-30
 ********************************************/
void qn8027_init_function(void)
{
	I2CPort_Init();
	uc_qn8027_init_flag = 0x5A;    //芯片初始化标志
	uc_qn8027_init_state = 0xff;   //芯片初始化状态
	uc_qn8027_delay_count = 0;
}

/*******************************************
 * 函数名：qn8027_init_function
 * 描述  ：  qn8027 FM频道设置
 * 输入  ：  频道值
 * 输出  ：  无
 * 履历  ：  V0.00  2019-5-1
 ********************************************/
void qn8027_fm_channel_set_function(void)
{
	if((uc_fm_channel>=76)&&(uc_fm_channel<=108)){
	    us_qn8027_channel = (uc_fm_channel-76)*20;
	    uc_qn8027_set_channel_flag = 0x5A;
	}
}


/*******************************************
 * 函数名：task_qn8027_function
 * 描述  ：  qn8027运行task
 * 输入  ：无
 * 输出  ：无
 * 履历  ：V0.00  2019-4-30
 ********************************************/
void task_qn8027_function(void)
{
	if(em_FM_switch == SWITCH_OPEN){//打开
		FM_POWER_ON;
		if(first_power_on == 0){
			first_power_on = 0x5A;
			qn8027_init_function();
		}
	}else{         //关闭
	    FM_POWER_OFF;
	    first_power_on = 0;  //初次上电标志位
	}

	qn8027_ChipInitialization();
	qn8027_set_channel_function();

}

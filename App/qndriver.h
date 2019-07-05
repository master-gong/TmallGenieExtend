/************************************************Copyright(c)***********************************
**                                   Quintic(Nanjing) Microelectronics Co,Ltd.
**                                   
**                                   http://www.quinticcorp.com
**
**--------------------File Info----------------------------------------------------------------
** File Name:                  qndriver.h
** subversion number:   160      
**----------------------------------------------------------------------------------------
************************************************************************************************/

#ifndef _QNDRIVER_H
#define _QNDRIVER_H
#define QN_8027
#define _QNFUNC_H_

#define CCS_RX  0
#define CCS_TX  1

#define FREQ2CHREG(freq)   ((freq-7600)/5)
#define CHREG2FREQ(ch)     (ch*5+7600)


#define _QNCOMMON_H_




#define QND_REG_NUM_MAX   19  // for qn8027
// crystal setting definition is not ready yet, please read datasheet to do setting accordingly
#define QND_CRYSTAL_REG             0x03 //?
#define QND_CRYSTAL_BIT_MASK        0x3f //?

#define QND_CRYSTAL_24MHZ           0x2b //?
#define QND_CRYSTAL_DEFAULT         QND_CRYSTAL_24MHZ



#define QND_MODE_SLEEP      0     
#define QND_MODE_WAKEUP     1
#define QND_MODE_DEFAULT    2
// RX / TX value is using upper 8 bit

#define QND_MODE_RX         0x8000
#define QND_MODE_TX         0x4000
// AM / FM value is using lower 8 bit 
// need to check datasheet to get right bit 
#define QND_MODE_FM         0x0000

#define BAND_FM        0


// tune
#define QND_FSTEP_50KHZ      0
#define QND_FSTEP_100KHZ      1
#define QND_FSTEP_200KHZ      2
// output format
#define QND_OUTPUT_ANALOG     0
#define QND_OUTPUT_IIS        1

// stereo mode
#define QND_TX_AUDIO_MONO              0x10
#define QND_TX_AUDIO_STEREO            0x00

#define QND_RX_AUDIO_MONO              0x20
#define QND_RX_AUDIO_STEREO            0x00

#define QND_CONFIG_MONO               0x01
#define QND_CONFIG_MUTE               0x02    
#define QND_CONFIG_SOFTCLIP           0x03
#define QND_CONFIG_AUTOAGC               0x04
#define QND_CONFIG_AGCGAIN               0x05    

#define QND_CONFIG_EQUALIZER           0x06    
#define QND_CONFIG_VOLUME               0x07          
#define QND_CONFIG_BASS_QUALITY       0x08
#define QND_CONFIG_BASS_FREQ           0x09
#define QND_CONFIG_BASS_GAIN           0x0a
#define QND_CONFIG_MID_QUALITY        0x0b
#define QND_CONFIG_MID_FREQ           0x0c
#define QND_CONFIG_MID_GAIN           0x0d
#define QND_CONFIG_TREBLE_FREQ        0x0e
#define QND_CONFIG_TREBLE_GAIN        0x0f

#define QND_ENABLE_EQUALIZER          0x10
#define QND_DISABLE_EQUALIZER         0x00


#define QND_CONFIG_AUDIOPEAK_DEV      0x11
#define QND_CONFIG_PILOT_DEV          0x12
#define QND_CONFIG_RDS_DEV            0x13

// input format
#define QND_INPUT_ANALOG     0
#define QND_INPUT_IIS        1

// i2s mode
#define QND_I2S_RX_ANALOG   0x00
#define QND_I2S_RX_DIGITAL  0x40
#define QND_I2S_TX_ANALOG   0x00
#define QND_I2S_TX_DIGITAL  0x20

//i2s clock data rate
#define QND_I2S_DATA_RATE_32K  0x00
#define QND_I2S_DATA_RATE_40K  0x10
#define QND_I2S_DATA_RATE_44K  0x20
#define QND_I2S_DATA_RATE_48K  0x30

//i2s clock Bit Wise
#define QND_I2S_BIT_8    0x00
#define QND_I2S_BIT_16   0x40
#define QND_I2S_BIT_24   0x80
#define QND_I2S_BIT_32   0xc0

//i2s Control mode
#define QND_I2S_MASTER   1
#define QND_I2S_SLAVE    0

//i2s Control mode
#define QND_I2S_MSB   0x00
#define QND_I2S_I2S   0x01
#define QND_I2S_DSP1  0x02
#define QND_I2S_DSP2  0x03
#define QND_I2S_LSB   0x04

#define QND_EQUALIZE_BASS    0x00
#define QND_EQUALIZE_MID    0x01
#define QND_EQUALIZE_TREBLE 0x02
// RDS, TMC
#define QND_EUROPE_FLEXIBILITY_DISABLE  0
#define QND_EUROPE_FLEXIBILITY_ENABLE   1
#define QND_RDS_OFF              0
#define QND_RDS_ON               1
#define QND_RDS_BUFFER_NOT_READY 0
#define QND_RDS_BUFFER_READY     1


#define CHIPID_QN8000    0x00
#define CHIPID_QN8005    0x20
#define CHIPID_QN8005B1  0x21
#define CHIPID_QN8006    0x30
#define CHIPID_QN8006LB  0x71
#define CHIPID_QN8007B1  0x11
#define CHIPID_QN8007    0x10
#define CHIPID_QN8006A1  0x30
#define CHIPID_QN8006B1  0x31
#define CHIPID_QN8016    0xe0
#define CHIPID_QN8016_1  0xb0
#define CHIPID_QN8015    0xa0
#define CHIPID_QN8065    0xa0
#define CHIPID_QN8067    0xd0
#define CHIPID_QN8065N   0xa0
#define CHIPID_QN8027    0x40
#define CHIPID_QN8025    0x80

#define RDS_INT_ENABLE  1
#define RDS_INT_DISABLE 0
//For antena impedance match
#define QND_HIGH_IMPEDANCE         1
#define QND_LOW_IMPEDANCE         0

#define QND_BAND_NUM     6
#define RSSINTHRESHOLD   4


typedef unsigned char  UINT8;             
typedef char           INT8;              
typedef unsigned short UINT16;            
typedef short          INT16;    




#define _QNCONFIG_H_



/********************* country selection**************/
#define COUNTRY_CHINA            0
#define COUNTRY_USA                1
#define COUNTRY_JAPAN            2
/************************EDN******************************/


/**********************************************************************************************
// Performance configuration 
***********************************************************************************************/


/**********************************************************************************************
// limitation configuration 
***********************************************************************************************/

#define QND_READ_RSSI_DELAY    10
#define QND_DELAY_BEFORE_UNMUTE  200
// auto scan
#define QND_MP_THRESHOLD       0x28   
#define PILOT_READ_OUT_DELAY_TIME 70
#define PILOT_SNR_READ_OUT_DELAY_TIME  (150-PILOT_READ_OUT_DELAY_TIME)
#define CH_SETUP_DELAY_TIME    300           


#define assert(str)
#define QND_LOG(a)
#define QND_LOGA(a,b)
#define QND_LOGB(a,b)
#define QND_LOGHEX(a,b)
#define _QNREG_H_
#define SYSTEM1       0x00
#define SYSTEM2       0x00
#define CH            0x01
#define CH_STEP       0x00
#define RDSD0         0x08
#define PAG_CAL       0x1f
#define CID2          0x06
#define RDSEN         0x80
#define TXREQ         0x20
#define CH_CH         0x03
#define RDSTXRDY      0x04
#define TX_FDEV       0x11  // FDEV on datasheet
#define _QNSYS_H_

#define CHANNEL_FILTER

// external driver interface 
// logical layer
/*****************************************************************************
Driver API Macro Definition
*****************************************************************************/
#define QNM_SetCrystal(Crystal)    \
        QNF_SetRegBit(QND_CRYSTAL_REG, QND_CRYSTAL_BIT_MASK, Crystal)     
#define QNM_SetAudioInputImpedance(AudioImpendance) \
        QND_WriteReg(REG_VGA, QND_ReadReg(REG_VGA) | (AudioImpendance & 0x3f))   
#define QNM_ResetToDefault() \
        QND_WriteReg(SYSTEM2, SWRST) 
#define QNM_SetFMWorkingMode(Modemask, Mode) \
        QND_WriteReg(SYSTEM1, Mode|(QND_ReadReg(SYSTEM1) &~ Modemask)
#define QNM_EnableAGC() \
        QND_WriteReg(TXAGC_GAIN, ~TAGC_GAIN_SEL&(QND_ReadReg(TXAGC_GAIN)))
#define QNM_DisableAGC()\
        QND_WriteReg(TXAGC_GAIN,   1|(TAGC_GAIN_SEL|(QND_ReadReg(TXAGC_GAIN)) )
#define QNM_EnableSoftClip() \
        QND_WriteReg(TXAGC_GAIN,    TX_SFTCLPEN |(QND_ReadReg(TXAGC_GAIN)) )
#define QNM_DisableSoftClip() \
        QND_WriteReg(TXAGC_GAIN,    ~TX_SFTCLPEN &(QND_ReadReg(TXAGC_GAIN)) )
#define QNM_GetMonoMode() \
        QND_ReadReg(STATUS1) & ST_MO_RX
#define QNM_SetRxThreshold(db) \
        QND_WriteReg(CCA, db)
#define QNM_SetAudioOutFormatIIS() \
        QND_WriteReg(CCA, (QND_ReadReg(CCA) | RXI2S))
#define QNM_SetAudioOutFormatAnalog() \
        QND_WriteReg(CCA, (QND_ReadReg(CCA) & ~RXI2S))
#define QNM_SetAudioInFormatIIS() \
        QND_WriteReg(CCA, (QND_ReadReg(CCA) | RXI2S))
#define QNM_SetAudioInFormatAnalog() \
        QND_WriteReg(CCA, (QND_ReadReg(CCA) & ~RXI2S))
#define QNM_GetRssi() \
        QND_ReadReg(0)
#define QND_AntenaInputImpedance(impendance) \
        QND_WriteReg(77, impendance)

#define QND_READ(adr)    QND_ReadReg(adr)
#define QND_WRITE(adr, value)  QND_WriteReg(adr, value)
extern void   QNF_SetRegBit(UINT8 reg, UINT8 bitMask, UINT8 data_val) ;

extern UINT8   qnd_Country;
extern UINT16  qnd_CH_START;
extern UINT16  qnd_CH_STOP;
extern UINT8   qnd_CH_STEP;

/*
  System General Control 
*/
extern UINT16 QNF_GetCh();

extern void QND_Delay(UINT16 ms) ;
extern UINT8 QND_Init() ;
extern void  QND_TuneToCH(UINT16 ch) ;
extern void  QND_SetSysMode(UINT16 mode) ;
extern void  QND_SetCountry(UINT8 country) ;



#define QN_TX
#define _QNTX_H_
extern void   QND_TXSetPower(UINT8 gain);
extern void   QND_TXConfigAudio(UINT8 optiontype, UINT8 option );

#define QN_RDS

#define _QNRDS_H_

extern UINT8 QND_RDSEnable(UINT8 on) ;
extern UINT8 QND_RDSCheckBufferReady(void) ;

void QND_RDSHighSpeedEnable(UINT8 on) ;
INT8 QND_RDSModeDetect(void)  ;


extern void  QND_RDSLoadData(UINT8 *rdsRawData, UINT8 upload) ;
#endif

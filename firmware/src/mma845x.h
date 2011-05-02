#ifndef __INCLUDED__MMA845X_H_
#define __INCLUDED__MMA845X_H_

//  MMA845xQ Sensor Internal Registers
typedef enum
{
  STATUS_00 = 0,          // 0x00
  OUT_X_MSB,              // 0x01
  OUT_X_LSB,              // 0x02
  OUT_Y_MSB,              // 0x03
  OUT_Y_LSB,              // 0x04
  OUT_Z_MSB,              // 0x05
  OUT_Z_LSB,              // 0x06
  SYSMOD = 0x0B,          // 0x0B
  INT_SOURCE,             // 0x0C
  WHO_AM_I,               // 0x0D
  XYZ_DATA_CFG,           // 0x0E
  HP_FILTER_CUTOFF,       // 0x0F
  PL_STATUS,              // 0x10
  PL_CFG,                 // 0x11
  PL_COUNT,               // 0x12
  PL_BF_ZCOMP,            // 0x13
  PL_P_L_THS_REG,         // 0x14
  FF_MT_CFG,              // 0x15
  FF_MT_SRC,              // 0x16
  FF_MT_THS,              // 0x17
  FF_MT_COUNT,            // 0x18
  TRANSIENT_CFG = 0x1D,   // 0x1D
  TRANSIENT_SRC,          // 0x1E
  TRANSIENT_THS,          // 0x1F
  TRANSIENT_COUNT,        // 0x20
  PULSE_CFG,              // 0x21
  PULSE_SRC,              // 0x22
  PULSE_THSX,             // 0x23
  PULSE_THSY,             // 0x24
  PULSE_THSZ,             // 0x25
  PULSE_TMLT,             // 0x26
  PULSE_LTCY,             // 0x27
  PULSE_WIND,             // 0x28
  ASLP_COUNT,             // 0x29
  CTRL_REG1,              // 0x2A
  CTRL_REG2,              // 0x2B
  CTRL_REG3,              // 0x2C
  CTRL_REG4,              // 0x2D
  CTRL_REG5,              // 0x2E
  OFF_X,                  // 0x2F
  OFF_Y,                  // 0x30
  OFF_Z,                  // 0x31

  NO_REGISTER = 0xFF    // marks end of registerSetting sequences
} MMA845x_Register_Address;

// STATUS_00_REG         0x00

#define ZYXOW_BIT             7
#define ZOW_BIT               6
#define YOR_BIT               5
#define XOR_BIT               4
#define ZYXDR_BIT             3
#define ZDR_BIT               2
#define YDR_BIT               1
#define XDR_BIT               0

#define ZYXOW_MASK            0x80
#define ZOW_MASK              0x40
#define YOR_MASK              0x20
#define XOR_MASK              0x10
#define ZYXDR_MASK            0x08
#define ZDR_MASK              0x04
#define YDR_MASK              0x02
#define XDR_MASK              0x01

// WHO_AM_I_REG          0x0D
#define MMA8451Q_ID           0x1A
#define MMA8452Q_ID           0x2A
#define MMA8453Q_ID           0x3A

//  F_STATUS FIFO Status Register

#define F_OVF_BIT             7
#define F_WMRK_FLAG_BIT       6
#define F_CNT5_BIT            5
#define F_CNT4_BIT            4
#define F_CNT3_BIT            3
#define F_CNT2_BIT            2
#define F_CNT1_BIT            1
#define F_CNT0_BIT            0

#define F_OVF_MASK            0x80
#define F_WMRK_FLAG_MASK      0x40
#define F_CNT5_MASK           0x20
#define F_CNT4_MASK           0x10
#define F_CNT3_MASK           0x08
#define F_CNT2_MASK           0x04
#define F_CNT1_MASK           0x02
#define F_CNT0_MASK           0x01
#define F_CNT_MASK            0x3F

//  F_SETUP FIFO Setup Register 0x09
#define F_MODE1_BIT           7
#define F_MODE0_BIT           6
#define F_WMRK5_BIT           5
#define F_WMRK4_BIT           4
#define F_WMRK3_BIT           3
#define F_WMRK2_BIT           2
#define F_WMRK1_BIT           1
#define F_WMRK0_BIT           0

#define F_MODE1_MASK          0x80
#define F_MODE0_MASK          0x40
#define F_WMRK5_MASK          0x20
#define F_WMRK4_MASK          0x10
#define F_WMRK3_MASK          0x08
#define F_WMRK2_MASK          0x04
#define F_WMRK1_MASK          0x02
#define F_WMRK0_MASK          0x01
#define F_MODE_MASK           0xC0
#define F_WMRK_MASK           0x3F


// TRIG CFG Register 0x0A
#define TRIG_TRANS_BIT      5
#define TRIG_LNDPRT_BIT     4
#define TRIG_PULSE_BIT      3
#define TRIG_FF_MT_BIT      2

#define TRIG_TRANS_MASK     0x20
#define TRIG_LNDPRT_MASK    0x10
#define TRIG_PULSE_MASK     0x08
#define TRIG_FF_MT_MASK     0x04


//  SYSMOD System Mode Register 0x0B

#define FGERR_BIT             7
#define FGT_4_BIT             6
#define FGT_3_BIT             5
#define FGT_2_BIT             4
#define FGT_1_BIT             3
#define FGT_0_BIT             2
#define SYSMOD1_BIT           1
#define SYSMOD0_BIT           0

#define FGERR_MASK            0x80
#define FGT_4MASK             0x40
#define FGT_3MASK             0x20
#define FGT_2MASK             0x10
#define FGT_1MASK             0x08
#define FGT_0MASK             0x04
#define FGT_MASK              0x7C
#define SYSMOD1_MASK          0x02
#define SYSMOD0_MASK          0x01
#define SYSMOD_MASK           0x03

//  INT_SOURCE System Interrupt Status Register 0x0C

#define SRC_ASLP_BIT          7
#define SRC_FIFO_BIT          6
#define SRC_TRANS_BIT         5
#define SRC_LNDPRT_BIT        4
#define SRC_PULSE_BIT         3
#define SRC_FF_MT_1_BIT       2
#define SRC_FF_MT_2_BIT       1
#define SRC_DRDY_BIT          0

#define SRC_ASLP_MASK         0x80
#define SRC_FIFO_MASK         0x40
#define SRC_TRANS_MASK        0x20
#define SRC_LNDPRT_MASK       0x10
#define SRC_PULSE_MASK        0x08
#define SRC_FF_MT_1_MASK      0x04
#define SRC_FF_MT_2_MASK      0x02
#define SRC_DRDY_MASK         0x01

//  XYZ_DATA_CFG Sensor Data Configuration Register 0x0E

#define HPF_OUT_BIT           4
#define FS1_BIT               1
#define FS0_BIT               0

#define HPF_OUT_MASK          0x10
#define FS1_MASK              0x02
#define FS0_MASK              0x01
#define FS_MASK               0x03

#define FULL_SCALE_8G         FS1_MASK
#define FULL_SCALE_4G         FS0_MASK
#define FULL_SCALE_2G         0x00

//  HP_FILTER_CUTOFF High Pass Filter Register 0x0F

#define PULSE_HPF_BYP         5
#define PULSE_LPF_EN          4
#define SEL1_BIT              1
#define SEL0_BIT              0

#define PULSE_HPF_BYP_MASK    0x20
#define PULSE_LPF_EN_MASK     0x10
#define SEL1_MASK             0x02
#define SEL0_MASK             0x01
#define SEL_MASK              0x03


//  PL_STATUS Portrait/Landscape Status Register 0x10
//  PL_PRE_STATUS Portrait/Landscape Previous Data Status Register

#define NEWLP_BIT             7
#define LO_BIT                6
#define LAPO1_BIT             2
#define LAPO0_BIT             1
#define BAFRO_BIT             0

#define NEWLP_MASK            0x80
#define LO_MASK               0x40
#define LAPO1_MASK            0x04
#define LAPO0_MASK            0x02
#define LAPO_MASK             0x06
#define BAFRO_MASK            0x01

//  PL_CFG Portrait/Landscape Configuration Register 0x11

#define DBCNTM_BIT            7
#define PL_EN_BIT             6

#define DBCNTM_MASK           0x80
#define PL_EN_MASK            0x40


//  PL_COUNT Portrait/Landscape Debounce Register 0x12

//  PL_BF_ZCOMP Back/Front and Z Compensation Register 0x13

#define BKFR1_BIT             7
#define BKFR0_BIT             6
#define ZLOCK2_BIT            2
#define ZLOCK1_BIT            1
#define ZLOCK0_BIT            0

#define BKFR1_MASK            0x80
#define BKFR0_MASK            0x40
#define ZLOCK2_MASK           0x04
#define ZLOCK1_MASK           0x02
#define ZLOCK0_MASK           0x01
#define BKFR_MASK             0xC0
#define ZLOCK_MASK            0x07


//  PL_P_L_THS Portrait to Landscape Threshold Registers 0x14

#define P_L_THS4_Bit          7
#define P_L_THS3_Bit          6
#define P_L_THS2_Bit          5
#define P_L_THS1_Bit          4
#define P_L_THS0_Bit          3
#define HYS2_Bit              2
#define HYS1_Bit              1
#define HYS0_Bit              0

#define P_L_THS4_MASK         0x80
#define P_L_THS3_MASK         0x40
#define P_L_THS2_MASK         0x20
#define P_L_THS1_MASK         0x10
#define P_L_THS0_MASK         0x08
#define P_L_THS_MASK          0xF8
#define HYS2_MASK             0x04
#define HYS1_MASK             0x02
#define HYS0_MASK             0x01
#define HYS_MASK              0x07

//  FF_MT_CFG Freefall and Motion Configuration Registers 0x15, 0x19

#define ELE_BIT               7
#define OAE_BIT               6
#define ZEFE_BIT              5
#define YEFE_BIT              4
#define XEFE_BIT              3

#define ELE_MASK              0x80
#define OAE_MASK              0x40
#define ZEFE_MASK             0x20
#define YEFE_MASK             0x10
#define XEFE_MASK             0x08

//  FF_MT_SRC Freefall and Motion Source Registers 0x16, 0x1A

#define EA_BIT                7
#define ZHE_BIT               5
#define ZHP_BIT               4
#define YHE_BIT               3
#define YHP_BIT               2
#define XHE_BIT               1
#define XHP_BIT               0

#define EA_MASK               0x80
#define ZHE_MASK              0x20
#define ZHP_MASK              0x10
#define YHE_MASK              0x08
#define YHP_MASK              0x04
#define XHE_MASK              0x02
#define XHP_MASK              0x01

//  FF_MT_THS Freefall and Motion Threshold Registers 0x17, 0x1B
//  TRANSIENT_THS Transient Threshold Register 0x1F

#define DBCNTM_BIT            7
#define THS6_BIT              6
#define THS5_BIT              5
#define THS4_BIT              4
#define THS3_BIT              3
#define THS2_BIT              2
#define THS1_BIT              1
#define THS0_BIT              0

#define DBCNTM_MASK           0x80
#define THS6_MASK             0x40
#define THS5_MASK             0x20
#define THS4_MASK             0x10
#define THS3_MASK             0x08
#define THS2_MASK             0x04
#define TXS1_MASK             0x02
#define THS0_MASK             0x01
#define THS_MASK              0x7F

//  FF_MT_COUNT Freefall Motion Count Registers 0x18, 0x1C

//  TRANSIENT_CFG Transient Configuration Register 0x1D

#define TELE_BIT              4
#define ZTEFE_BIT             3
#define YTEFE_BIT             2
#define XTEFE_BIT             1
#define HPF_BYP_BIT           0

#define TELE_MASK             0x10
#define ZTEFE_MASK            0x08
#define YTEFE_MASK            0x04
#define XTEFE_MASK            0x02
#define HPF_BYP_MASK          0x01

//  TRANSIENT_SRC Transient Source Register 0x1E

#define TEA_BIT               6
#define ZTRANSE_BIT           5
#define ZTRANSPOL_BIT         4
#define YTRANSE_BIT           3
#define YTRANSPOL_BIT         2
#define XTRANSE_BIT           1
#define XTRANSPOL_BIT         0


#define TEA_MASK              0x40
#define ZTRANSE_MASK          0x20
#define ZTRANSEPOL_MASK       0x10
#define YTRANSE_MASK          0x08
#define YTRANSEPOL_MASK       0x04
#define XTRANSE_MASK          0x02
#define XTRANSEPOL_MASK       0x01

//  TRANSIENT_COUNT Transient Debounce Register

//   PULSE_CFG Pulse Configuration Register

#define DPA_BIT               7
#define PELE_BIT              6
#define ZDPEFE_BIT            5
#define ZSPEFE_BIT            4
#define YDPEFE_BIT            3
#define YSPEFE_BIT            2
#define XDPEFE_BIT            1
#define XSPEFE_BIT            0

#define DPA_MASK              0x80
#define PELE_MASK             0x40
#define ZDPEFE_MASK           0x20
#define ZSPEFE_MASK           0x10
#define YDPEFE_MASK           0x08
#define YSPEFE_MASK           0x04
#define XDPEFE_MASK           0x02
#define XSPEFE_MASK           0x01

//   PULSE_SRC Pulse Source Register

#define PEA_BIT               7
#define PAXZ_BIT              6
#define PAXY_BIT              5
#define PAXX_BIT              4
#define PDPE_BIT              3
#define POLZ_BIT              2
#define POLY_BIT              1
#define POLX_BIT              0

#define PEA_MASK              0x80
#define PAXZ_MASK             0x40
#define PAXY_MASK             0x20
#define PAXX_MASK             0x10
#define PDPE_MASK             0x08
#define POLZ_MASK             0x04
#define POLY_MASK             0x02
#define POLX_MASK             0x01

//   PULSE_THS XYZ Pulse Threshold Registers

#define PTHS_MASK             0x7F

//   PULSE_TMLT Pulse Time Window Register

//   PULSE_LTCY Pulse Latency Timer Register

//   PULSE_WIND Second Pulse Time Window Register

//   ASLP_COUNT Auto Sleep Inactivity Timer Register

//   CTRL_REG1 System Control 1 Register 0x2A

#define ASLP_RATE1_BIT        7
#define ASLP_RATE0_BIT        6
#define DR2_BIT               5
#define DR1_BIT               4
#define DR0_BIT               3
#define LNOISE_BIT            2
#define FREAD_BIT             1
#define ACTIVE_BIT            0

#define ASLP_RATE1_MASK       0x80
#define ASLP_RATE0_MASK       0x40
#define DR2_MASK              0x20
#define DR1_MASK              0x10
#define DR0_MASK              0x08
#define LNOISE_MASK           0x04
#define FREAD_MASK            0x02
#define ACTIVE_MASK           0x01
#define ASLP_RATE_MASK        0xC0
#define DR_MASK               0x38
//                      
#define ASLP_RATE_20MS        0x00
#define ASLP_RATE_80MS        ASLP_RATE0_MASK
#define ASLP_RATE_160MS       ASLP_RATE1_MASK
#define ASLP_RATE_640MS       ASLP_RATE1_MASK+ASLP_RATE0_MASK

#define DATA_RATE_1250US      0x00
#define DATA_RATE_2500US      DR0_MASK
#define DATA_RATE_5MS         DR1_MASK
#define DATA_RATE_10MS        DR1_MASK+DR0_MASK
#define DATA_RATE_20MS        DR2_MASK
#define DATA_RATE_80MS        DR2_MASK+DR0_MASK
#define DATA_RATE_160MS       DR2_MASK+DR1_MASK
#define DATA_RATE_640MS       DR2_MASK+DR1_MASK+DR0_MASK

//   CTRL_REG2 System Control 2 Register 0x2B

#define ST_BIT                7
#define BOOT_BIT              6
#define SMODS1_BIT            4
#define SMODS0_BIT            3
#define SLPE_BIT              2
#define MODS1_BIT             1
#define MODS0_BIT             0

#define ST_MASK               0x80
#define BOOT_MASK             0x40
#define SMODS1_MASK           0x20
#define SMODS0_MASK           0x10
#define SLPE_MASK             0x04
#define MODS1_MASK            0x02
#define MODS0_MASK            0x01
#define SMODS_MASK            0x18
#define MODS_MASK             0x03

//   CTRL_REG3 Interrupt Control Register 0x2C

#define FIFO_GATE_BIT         7
#define WAKE_TRANS_BIT        6
#define WAKE_LNDPRT_BIT       5
#define WAKE_PULSE_BIT        4
#define WAKE_FF_MT_1_BIT      3
#define WAKE_FF_MT_2_BIT      2
#define IPOL_BIT              1
#define PP_OD_BIT             0

#define FIFO_GATE_MASK        0x80
#define WAKE_TRANS_MASK       0x40
#define WAKE_LNDPRT_MASK      0x20
#define WAKE_PULSE_MASK       0x10
#define WAKE_FF_MT_1_MASK     0x08
#define WAKE_FF_MT_2_MASK     0x04
#define IPOL_MASK             0x02
#define PP_OD_MASK            0x01

//   CTRL_REG4 Interrupt Enable Register 0x2D

#define INT_EN_ASLP_BIT       7
#define INT_EN_FIFO_BIT       6
#define INT_EN_TRANS_BIT      5
#define INT_EN_LNDPRT_BIT     4
#define INT_EN_PULSE_BIT      3
#define INT_EN_FF_MT_1_BIT    2
#define INT_EN_FF_MT_2_BIT    1
#define INT_EN_DRDY_BIT       0

#define INT_EN_ASLP_MASK      0x80
#define INT_EN_FIFO_MASK      0x40
#define INT_EN_TRANS_MASK     0x20
#define INT_EN_LNDPRT_MASK    0x10
#define INT_EN_PULSE_MASK     0x08
#define INT_EN_FF_MT_1_MASK   0x04
#define INT_EN_FF_MT_2_MASK   0x02
#define INT_EN_DRDY_MASK      0x01

//   CTRL_REG5 Interrupt Configuration Register 0x2E

#define INT_CFG_ASLP_BIT      7
#define INT_CFG_FIFO_BIT      6
#define INT_CFG_TRANS_BIT     5
#define INT_CFG_LNDPRT_BIT    4
#define INT_CFG_PULSE_BIT     3
#define INT_CFG_FF_MT_1_BIT   2
#define INT_CFG_FF_MT_2_BIT   1
#define INT_CFG_DRDY_BIT      0

#define INT_CFG_ASLP_MASK     0x80
#define INT_CFG_FIFO_MASK     0x40
#define INT_CFG_TRANS_MASK    0x20
#define INT_CFG_LNDPRT_MASK   0x10
#define INT_CFG_PULSE_MASK    0x08
#define INT_CFG_FF_MT_1_MASK  0x04
#define INT_CFG_FF_MT_2_MASK  0x02
#define INT_CFG_DRDY_MASK     0x01

//   XYZ Offset Correction Registers


/***********************************************************************************************\
* Public type definitions
\***********************************************************************************************/

/***********************************************************************************************\
* Public memory declarations
\***********************************************************************************************/

/***********************************************************************************************\
* Public prototypes
\***********************************************************************************************/

void MMA845x_Init(void);
void MMA845x_Standby(void);
void MMA845x_Active(void);


#endif  /* _MMA845X_H_ */

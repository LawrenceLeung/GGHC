#include "stm32f10x.h"
#include "I2CRoutines.h"
#include <string.h>
#include "jigbox.h"
#include "i2c.h"

#if !defined(USE_MMA8452_ACCELEROMETER)
#  error "LISL3V02 code not written yet!"
#endif

bool readAccelerometerRegister(uint8_t regAddr, uint8_t *buffer, uint8_t nBytes)
{
    // write register address
    if (I2C_Master_BufferWrite(ACCEL_I2C_CHANNEL, &regAddr, 1, Polling, ACCEL_I2C_ADDR, true) == Error)
        return false;                  // TODO do we need repeated-start?

    // read data
    return I2C_Master_BufferRead(ACCEL_I2C_CHANNEL, buffer, nBytes, Polling, ACCEL_I2C_ADDR) == Success;
}

bool writeAccelerometerRegister(uint8_t regAddr, uint8_t *buffer, uint8_t nBytes)
{
    uint8_t buf[ ACCEL_MAX_WRITE_BYTES + 1 ];
    buf[0] = regAddr;
    memcpy(buf+1, buffer, nBytes);
    // write register address and data
    return I2C_Master_BufferWrite(ACCEL_I2C_CHANNEL, buf, nBytes+1, Polling, ACCEL_I2C_ADDR, false) == Success;
}

bool writeAccelerometerRegisterSingle(uint8_t regAddr, uint8_t byte)
{
    uint8_t buf[ 2 ];
    buf[0] = regAddr;
    buf[1] = byte;
    // write register address and data
    return I2C_Master_BufferWrite(ACCEL_I2C_CHANNEL, buf, 2, Polling, ACCEL_I2C_ADDR, false) == Success;
}

bool readMotionDetectSource(MotionDetectSource *buffer)
{
    return readAccelerometerRegister(FF_MT_SRC, (uint8_t*)buffer, 1);
}

bool readTransientSource(TransientSource *buffer)
{
    return readAccelerometerRegister(TRANSIENT_SRC, (uint8_t*)buffer, 1);
}

bool readPulseSource(PulseSource *buffer)
{
    return readAccelerometerRegister(PULSE_SRC, (uint8_t*)buffer, 1);
}

static void Init_Accelerometer_Interrupt(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // Assign PC3 and PC4 as inputs
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;  // pull up open-drain outputs
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    EXTI_ClearITPendingBit(EXTI_Line3);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource3);

    // set up PC3 (INT1) as interrupt pin
    EXTI_InitTypeDef EXTI_InitStruct =
    {
        .EXTI_Line    = EXTI_Line3,
        .EXTI_Mode    = EXTI_Mode_Interrupt,
        .EXTI_Trigger = EXTI_Trigger_Falling,
        .EXTI_LineCmd = ENABLE
    };

    EXTI_Init(&EXTI_InitStruct);

    NVIC_SetPriority(EXTI3_IRQn, 0x02);
    NVIC_EnableIRQ(EXTI3_IRQn);
}

static void NVIC_Configuration(void)
{
    /* 1 bit for pre-emption priority, 3 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    NVIC_SetPriority(I2C1_EV_IRQn, 0x00);
    NVIC_EnableIRQ(I2C1_EV_IRQn);

    NVIC_SetPriority(I2C1_ER_IRQn, 0x01);
    NVIC_EnableIRQ(I2C1_ER_IRQn);

    NVIC_SetPriority(I2C2_EV_IRQn, 0x00);
    NVIC_EnableIRQ(I2C2_EV_IRQn);

    NVIC_SetPriority(I2C2_ER_IRQn, 0x01);
    NVIC_EnableIRQ(I2C2_ER_IRQn);

}

typedef struct {
    MMA845x_Register_Address address;
    uint8_t data;
} registerSetting;

static bool sendRegisterSequence(registerSetting *settings)
{
    for (;;)
    {
        registerSetting s = *settings++;
        if (s.address == 0xFF)
            break;
        if (!writeAccelerometerRegisterSingle(s.address, s.data)) return false;
    }
    return true;
}

// init part
registerSetting initSequence[] =
{
    { CTRL_REG1, 0x00 },                               // set CTRL_REG1 = STANDBY

    { CTRL_REG3, PP_OD_MASK | WAKE_TRANS_MASK | WAKE_FF_MT_1_MASK },    // INT pins open drain active low, wake on motion or trans
    { XYZ_DATA_CFG, FULL_SCALE_2G },   // 2g not HPF

    // motion detect interrupt
    // { FF_MT_CFG, ELE_MASK | OAE_MASK | ZEFE_MASK | YEFE_MASK | XEFE_MASK },     // motion detect latch all axes
    // { FF_MT_THS, 4 },     // 1/4g detect (1/16 g/count)
    // { FF_MT_COUNT, 3 },   // debounce for 3 samples = 3.75msec
    // { CTRL_REG4, INT_EN_FF_MT_1_MASK }, // enable INT on motion detect
    // { CTRL_REG5, INT_CFG_FF_MT_1_MASK }, // on INT1

    // transient detect interrupt
    { TRANSIENT_CFG, TELE_MASK | ZTEFE_MASK | YTEFE_MASK | XTEFE_MASK },    // all 3 axes, latched into TRANSIENT_SRC
    { TRANSIENT_THS, 6 },   // 10/16g detect
    { TRANSIENT_COUNT, 15 }, // 5 msec debounce
    { CTRL_REG4, INT_EN_TRANS_MASK }, // enable INT on transient detect
    { CTRL_REG5, INT_CFG_TRANS_MASK }, // on INT1

    { CTRL_REG1, FREAD_MASK | ACTIVE_MASK },           // turn on Fast Read mode, make part active

    { NO_REGISTER, 0xFF }                              // mark END
};

bool Init_Accelerometer(void)
{
    NVIC_Configuration();
    I2C_LowLevel_Init(ACCEL_I2C_CHANNEL);
    Init_Accelerometer_Interrupt();

    // try reading WHO_AM_I 0x0D
    uint8_t whoami;
    if (!readAccelerometerRegister(WHO_AM_I, &whoami, 1))
        return false;

    // should be 0x2A
    if (whoami != MMA8452Q_ID)
        return false;

    return sendRegisterSequence(initSequence);
}

void EXTI3_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line3);
        HitEvent *hit = Q_NEW(HitEvent, HIT_SIG);
        readAccelerometer(&hit->xyz);
        readTransientSource(&hit->transient);
        QActive_postFIFO(AO_IOEventListener, (QEvent *)hit);
    }
}

// read x,y,z
// assumes in 8-bit FREAD mode
bool readAccelerometer(AccelerometerReport_t *buffer)
{
    return readAccelerometerRegister(OUT_X_MSB, (uint8_t*)buffer, 3);
}

void testEXTIInterrupt(void)
{
    EXTI_GenerateSWInterrupt(EXTI_Line3);
}

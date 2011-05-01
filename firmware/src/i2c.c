#include "stm32f10x.h"
#include "I2CRoutines.h"
#include <string.h>
#include "jigbox.h"
#include "i2c.h"

#if !defined(USE_MMA8452_ACCELEROMETER)
#  error "LISL3V02 code not written yet!"
#endif

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

bool readTransientSource(TransientSource *buffer)
{
    return readAccelerometerRegister(0x1E, (uint8_t*)buffer, 1);
}

bool readPulseSource(PulseSource *buffer)
{
    return readAccelerometerRegister(0x22, (uint8_t*)buffer, 1);
}

bool Init_Accelerometer(void)
{
    NVIC_Configuration();
    I2C_LowLevel_Init(ACCEL_I2C_CHANNEL);

    // set up PC3 (INT1) as interrupt pin
    EXTI_InitTypeDef EXTI_InitStruct =
    {
        .EXTI_Line    = EXTI_Line3,
        .EXTI_Mode    = EXTI_Mode_Interrupt,
        .EXTI_Trigger = EXTI_Trigger_Rising,
        .EXTI_LineCmd = ENABLE
    };

    EXTI_Init(&EXTI_InitStruct);

    // try reading WHO_AM_I 0x0D
    uint8_t whoami;
    if (!readAccelerometerRegister(0x0D, &whoami, 1))
        return false;

    // should be 0x2A
    if (whoami != 0x2A)
        return false;

    // init part
    // set CTRL_REG1 = STANDBY
    if (!writeAccelerometerRegisterSingle(0x2A, 0x00)) return false;

    // set XYZ_DATA_CFG to enable hi-pass filtering, 2g full scale
    if (!writeAccelerometerRegisterSingle(0x0E, 0x10)) return false;

    // set TRANSIENT_CFG to enable hi-pass filtering, all axes, latch into TRANSIENT_SRC
    if (!writeAccelerometerRegisterSingle(0x1D, 0x1E)) return false;

    // set TRANSIENT_THS to set threshold (1/16g/LSB) to 1/2g
    if (!writeAccelerometerRegisterSingle(0x1F, 0x08)) return false;

    // set PULSE_CFG for single and double pulse detection on all axes
    if (!writeAccelerometerRegisterSingle(0x21, 0x7F)) return false;

    // set PULSE_THSX, Y, Z
    if (!writeAccelerometerRegisterSingle(0x1F, 0x08)) return false;

    // set CTRL_REG4 to enable INT_EN_TRANS and INT_EN_PULSE
    if (!writeAccelerometerRegisterSingle(0x2D, 0x28)) return false;

    // set CTRL_REG5 to set INT_EN_TRANS and INT_EN_PULSE for INT1 pin
    if (!writeAccelerometerRegisterSingle(0x2E, 0x28)) return false;

    // turn on Fast Read mode, make part active
    if (!writeAccelerometerRegisterSingle(0x2A, 0x03)) return false;

    return true;
}

void EXTI3_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}

bool readAccelerometer(AccelerometerReport_t *buffer)
{
    return readAccelerometerRegister(0x00, (uint8_t*)buffer, 4);
}

void testEXTIInterrupt(void)
{
    EXTI_GenerateSWInterrupt(EXTI_Line3);
}

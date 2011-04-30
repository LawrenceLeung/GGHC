#include "stm32f10x.h"
#include "I2CRoutines.h"
#include "i2c.h"
#include <string.h>
#include <stdbool.h>


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
    if (I2C_Master_BufferWrite(ACCEL_I2C_CHANNEL, &regAddr, 1, Polling, ACCEL_I2C_ADDR) == Error)
        return false;
    // TODO do we need repeated-start?
    // read data
    return I2C_Master_BufferRead(ACCEL_I2C_CHANNEL, buffer, nBytes, Polling, ACCEL_I2C_ADDR) == Success;
}

bool writeAccelerometerRegister(uint8_t regAddr, uint8_t *buffer, uint8_t nBytes)
{
    uint8_t buf[ ACCEL_MAX_WRITE_BYTES + 1 ];
    buf[0] = regAddr;
    memcpy(buf+1, buffer, nBytes);
    // write register address and data
    return I2C_Master_BufferWrite(ACCEL_I2C_CHANNEL, buf, nBytes+1, Polling, ACCEL_I2C_ADDR) == Success;
}

bool Init_Accelerometer(void)
{
    NVIC_Configuration();
    I2C_LowLevel_Init(ACCEL_I2C_CHANNEL);

    // try reading WHO_AM_I 0x0D
    uint8_t whoami;
    if (!readAccelerometerRegister(0x0D, &whoami, 1))
    	return false;

    // should be 0x2A
    if (whoami != 0x2A)
        return false;

    // init part
    uint8_t ctrl_reg;
    // set CTRL_REG1 = STANDBY
    ctrl_reg = 0x00;
    if (!writeAccelerometerRegister(0x2A, &ctrl_reg, 1))
    	return false;
    // turn on Fast Read mode, make part active
    ctrl_reg = 0x03;
    if (!writeAccelerometerRegister(0x2A, &ctrl_reg, 1))
    	return false;

    return true;
}

bool readAccelerometer(AccelerometerReport_t *buffer)
{
    return readAccelerometerRegister(0x00, (uint8_t*)buffer, 4);
}

#ifndef __INCLUDED_I2C_H
#define __INCLUDED_I2C_H

#define USE_MMA8452_ACCELEROMETER 1

#include <stdint.h>
#include <stdbool.h>

#if USE_MMA8452_ACCELEROMETER

// MMA8452Q accelerometer, default (read) address
#  define ACCEL_I2C_ADDR        0x38
#  define ACCEL_I2C_CHANNEL     I2C2
#  define ACCEL_MAX_WRITE_BYTES 8
typedef uint8_t AccelerometerValue_t;

#else

// LISL3V02 on Olimex board
#  define ACCEL_I2C_ADDR        0x3A
#  define ACCEL_I2C_CHANNEL     I2C1
#  define ACCEL_MAX_WRITE_BYTES 1      // ?
typedef uint8_t AccelerometerValue_t;

#endif

typedef struct __attribute__((__packed__))
{
    uint8_t status;
    AccelerometerValue_t x;
    AccelerometerValue_t y;
    AccelerometerValue_t z;
} AccelerometerReport_t;

extern bool Init_Accelerometer(void);

extern bool readAccelerometerRegister(uint8_t regAddr, uint8_t *buffer, uint8_t nBytes);
extern bool writeAccelerometerRegister(uint8_t regAddr, uint8_t *buffer, uint8_t nBytes);

extern bool readAccelerometer(AccelerometerValue_t *buffer);

#endif

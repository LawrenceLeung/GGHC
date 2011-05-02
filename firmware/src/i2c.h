#ifndef __INCLUDED_I2C_H
#define __INCLUDED_I2C_H

#define USE_MMA8452_ACCELEROMETER 1

#include <stdint.h>
#include <stdbool.h>
#include "qp_port.h"

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

// read from register 0x1E, TRANSIENT_SRC
typedef struct {
    uint8_t X_Trans_Pol: 1;     // X event was negative g
    uint8_t X_Trans_Evt: 1;       // there was an X Transient event
    uint8_t Y_Trans_Pol: 1;     // Y event was negative g
    uint8_t Y_Trans_Evt: 1;       // there was an Y Transient event
    uint8_t Z_Trans_Pol: 1;     // Z event was negative g
    uint8_t Z_Trans_Evt: 1;       // there was an Z Transient event
    uint8_t Event_Active: 1;    // there was at least one flag
    uint8_t unused: 3;
} TransientSource;

// read from register 0x22, PULSE_SRC
typedef struct {
    uint8_t NegativePol_X: 1;
    uint8_t NegativePol_Y: 1;
    uint8_t NegativePol_Z: 1;
    uint8_t DoublePulseOnFirstEvent: 1;
    uint8_t X_Pulse_Evt: 1;
    uint8_t Y_Pulse_Evt: 1;
    uint8_t Z_Pulse_Evt: 1;
    uint8_t Event_Active: 1;    // something happened
} PulseSource;

extern bool Init_Accelerometer(void);

extern bool readAccelerometerRegister(uint8_t regAddr, uint8_t *buffer, uint8_t nBytes);
extern bool writeAccelerometerRegister(uint8_t regAddr, uint8_t *buffer, uint8_t nBytes);
bool writeAccelerometerRegisterSingle(uint8_t regAddr, uint8_t byte);

extern bool readAccelerometer(AccelerometerReport_t *buffer);

extern bool readTransientSource(TransientSource *buffer);
extern bool readPulseSource(PulseSource *buffer);

extern void testEXTIInterrupt(void);

// generated on interrupt from Accelerometer
// EV_HIT_SIG
typedef struct
{
    QEvent super;
    TransientSource transient;
    PulseSource pulse;
} HitEvent;

#endif

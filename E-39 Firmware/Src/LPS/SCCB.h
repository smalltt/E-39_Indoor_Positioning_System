#ifndef __SCCB_H
#define __SCCB_H

#include "stm32f4xx_i2c.h"
#include "stm32f4xx_gpio.h"

#define I2C_PAGESIZE 4

#define Open407V_SCCB_1 I2C1
#define Open407V_SCCB_2 I2C2

/* Use this define to set the maximum delay timeout for the I2C DCMI_SingleRandomWrite()
and DCMI_SingleRandomRead() operations. Exeeding this timeout delay, the read/write
functions will be aborted and return error code (0xFF).
The period of the delay will depend on the system operating frequency. The following
value has been set for system running at 120MHz. */
#define SCCB_Open407V_FLAG_TIMEOUT     10000000

#define OV2640_DEVICE_WRITE_ADDRESS    0x60

#define OV2640_DEVICE_READ_ADDRESS     0x61

#define SCCB_SPEED               100000

#define SCCB_SLAVE_ADDRESS7      0xFE

void SCCB_GPIO_Config(void);
uint8_t DCMI_SingleRandomWrite(I2C_TypeDef* i2c, uint8_t Addr, uint8_t Data);
uint8_t DCMI_SingleRandomRead(I2C_TypeDef* i2c, uint8_t Addr, uint8_t *Data);

#else
#error Only to be used once!
#endif 
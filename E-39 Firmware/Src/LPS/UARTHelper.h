#ifndef __UARTHelper
#define __UARTHelper

#include <stm32f4xx_usart.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_dma.h>
#include "misc.h"
#include "IOHelper.h"
#include "EZUtils.h"

void InitUART(uint32_t baud);
uint32_t GetRxBufferPosition();
uint8_t  fgetc();
uint16_t fgetc16();
uint32_t fgetc32();
void fputc(uint8_t val);
void fputc32(uint32_t val);
void fputc16(uint16_t val);
void fputs(const char string[]);
void fputsnl(const char string[]);
void fputsnl();
bool kbhit();
void fputi(uint64_t n);
void fputh(uint64_t dec);
void fputArrayViaDMA(uint8_t *data, int32_t size);

#else
#error Only to be used once!
#endif
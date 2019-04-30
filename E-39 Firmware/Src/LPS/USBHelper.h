#ifndef __USBHelper
#define __USBHelper

#include <stm32f4xx_usart.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_dma.h>
#include "misc.h"
#include "EZUtils.h"
#include "tm_stm32f4_usb_vcp.h"

void InitUSB();
void UsbPutc(uint8_t val);
void UsbPutArrayViaDMA(uint8_t *data, int32_t size);
void UsbPuts(const char string[]);
void UsbPutc32(uint32_t val);
void UsbPuti(uint64_t n);

#else
#error Only to be used once!
#endif
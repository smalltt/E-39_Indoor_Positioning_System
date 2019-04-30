#ifndef __IOHelper
#define __IOHelper

#include <stm32f4xx_gpio.h>

void SetIODirection(GPIO_TypeDef *bank, uint32_t port, GPIOMode_TypeDef direction);

void SetIODirection(GPIO_TypeDef *bank, uint32_t port, GPIOMode_TypeDef direction, GPIOPuPd_TypeDef Pull);

void SetIODirection(GPIO_TypeDef *bank, uint32_t pin, GPIOMode_TypeDef direction, GPIOPuPd_TypeDef PuPd, GPIOOType_TypeDef GPIO_OType);

void SetHigh(GPIO_TypeDef *bank, uint32_t pin);

void SetLow(GPIO_TypeDef *bank, uint32_t pin);

bool GetPin(GPIO_TypeDef *bank, uint32_t pin);

#endif
#include "IOHelper.h"

void SetIODirection(GPIO_TypeDef *bank, uint32_t pin, GPIOMode_TypeDef direction) {

  SetIODirection(bank, pin, direction, GPIO_PuPd_NOPULL);
}

void SetIODirection(GPIO_TypeDef *bank, uint32_t pin, GPIOMode_TypeDef direction, GPIOPuPd_TypeDef PuPd) {

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // Push/Pull or Open Drain
  GPIO_InitStructure.GPIO_PuPd = PuPd;

  GPIO_InitStructure.GPIO_Pin = pin;
  GPIO_InitStructure.GPIO_Mode = direction;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(bank, &GPIO_InitStructure);
}

void SetIODirection(GPIO_TypeDef *bank, uint32_t pin, GPIOMode_TypeDef direction, GPIOPuPd_TypeDef PuPd, GPIOOType_TypeDef GPIO_OType) {

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_OType = GPIO_OType;
  GPIO_InitStructure.GPIO_PuPd = PuPd;

  GPIO_InitStructure.GPIO_Pin = pin;
  GPIO_InitStructure.GPIO_Mode = direction;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(bank, &GPIO_InitStructure);
}

bool GetPin(GPIO_TypeDef *bank, uint32_t pin) {
  
  return GPIO_ReadInputDataBit(bank, pin);
}

void SetHigh(GPIO_TypeDef *bank, uint32_t pin) {

  bank->BSRRL = pin;
}

void SetLow(GPIO_TypeDef *bank, uint32_t pin) {

  bank->BSRRH = pin;
}
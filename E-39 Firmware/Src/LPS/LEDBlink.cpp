#include "stm32f4xx.h"
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_i2c.h>
#include <stm32f4xx_dcmi.h>
#include <stm32f4xx_dma.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_flash.h>

#include "IOHelper.h"
#include <misc.h>
#include "UARTHelper.h"
#include "USBHelper.h"
#include "EZUtils.h"
#include "SCCB.h"
#include "DCMI_OV2640_INITTABLE.h"

#include "ErrorCommunicating.jpg.h"
#include "ImageIntro.jpg.h"

#define ADDR_WRITE 0x42

#define ADDR_READ 0x43

#define IMAGE_SIZE 65534

uint8_t JpegBuffer[IMAGE_SIZE];

bool _OutputInterface; // high is USB, low is UART

uint8_t _selectedCamera = 0;

void OV2640_DCMI_Config(void) {

  /* VSYNC*/
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_DCMI);
  /* HSYNC*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_DCMI);
  /* PCLK*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_DCMI);
  /* data0*/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_DCMI);
  /* data1*/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_DCMI);
  /* data2*/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_DCMI);
  /* data3*/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_DCMI);
  /* data4*/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_DCMI);
  /* data5*/
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_DCMI);
  /* data6*/
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_DCMI);
  /* data7*/
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_DCMI);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  DCMI_InitTypeDef DCMI_InitStructure;
  DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_SnapShot;
  DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;
  DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
  DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_Low;
  DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_Low;
  DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;
  DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;

  DCMI_Init(&DCMI_InitStructure);

  DMA_Cmd(DMA2_Stream1, ENABLE);

  DCMI_JPEGCmd(ENABLE);

  DCMI_Cmd(ENABLE);
}

void OV2640_DMA_Init(void) {

  /* DMA2 Stream1 Configuration */
  DMA_DeInit(DMA2_Stream1);

  DMA_InitTypeDef  DMA_InitStructure;
  DMA_InitStructure.DMA_Channel = DMA_Channel_1;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DCMI->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)JpegBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = IMAGE_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  DMA_Init(DMA2_Stream1, &DMA_InitStructure);
}

void MCO1_Init(void) {

  RCC_ClockSecuritySystemCmd(ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_5);
}

bool OV2640_JPEGConfig(I2C_TypeDef* i2c) {

  uint32_t i;

  //  fputsnl("Sending JPEG Init");
  for (i = 0; i < (sizeof(OV2640_JPEG_INIT) / 2); i++) {

    uint8_t r = DCMI_SingleRandomWrite(i2c, OV2640_JPEG_INIT[i][0], OV2640_JPEG_INIT[i][1]);

    if (r == 0xff)
      return false;

    DelayMS(1);
  }

  //  fputsnl("Sending YUV422 Init");
  for (i = 0; i < (sizeof(OV2640_YUV422) / 2); i++) {

    uint8_t r = DCMI_SingleRandomWrite(i2c, OV2640_YUV422[i][0], OV2640_YUV422[i][1]);

    if (r == 0xff)
      return false;

    DelayMS(1);
  }

  //  fputsnl("Sending JPEG but not init");
  for (i = 0; i < (sizeof(OV2640_JPEG) / 2); i++) {

    uint8_t r = DCMI_SingleRandomWrite(i2c, OV2640_JPEG[i][0], OV2640_JPEG[i][1]);

    if (r == 0xff)
      return false;

    DelayMS(1);
  }

  for (i = 0; i < (sizeof(OV2640_640x480_JPEG) / 2); i++) {

    uint8_t r = DCMI_SingleRandomWrite(i2c, OV2640_640x480_JPEG[i][0], OV2640_640x480_JPEG[i][1]);

    if (r == 0xff)
      return false;

    DelayMS(1);
  }

  return true;
}

void SCCB_WriteRegs(I2C_TypeDef* i2c, const uint8_t* pbuf) {

  while (1) {

    if ((*pbuf == 0) && (*(pbuf + 1) == 0)) {

      break;
    }
    else {

      DCMI_SingleRandomWrite(i2c, *pbuf++, *pbuf++);
    }
  }
}

void StatusLED(bool red, bool green, bool blue) {

  if (red)
    SetLow(GPIOD, GPIO_Pin_8);
  else
    SetHigh(GPIOD, GPIO_Pin_8);

  if (green)
    SetLow(GPIOB, GPIO_Pin_13);
  else
    SetHigh(GPIOB, GPIO_Pin_13);

  if (blue)
    SetLow(GPIOB, GPIO_Pin_12);
  else
    SetHigh(GPIOB, GPIO_Pin_12);
}

void OV2640_AutoExposure(I2C_TypeDef* i2c, uint8_t level) {

  switch (level) {

  case 0:
    SCCB_WriteRegs(i2c, OV2640_AUTOEXPOSURE_LEVEL0);
    break;
  case 1:
    SCCB_WriteRegs(i2c, OV2640_AUTOEXPOSURE_LEVEL1);
    break;
  case 2:
    SCCB_WriteRegs(i2c, OV2640_AUTOEXPOSURE_LEVEL2);
    break;
  case 3:
    SCCB_WriteRegs(i2c, OV2640_AUTOEXPOSURE_LEVEL3);
    break;
  case 4:
    SCCB_WriteRegs(i2c, OV2640_AUTOEXPOSURE_LEVEL4);
    break;
  default:
    SCCB_WriteRegs(i2c, OV2640_AUTOEXPOSURE_LEVEL0);
    break;
  }

  DelayMS(1);
}

void flashStatus(int delay, int count) {

  for (int x = 0; x < count; x++) {

    StatusLED(false, true, false);
    DelayMS(delay);

    StatusLED(false, false, false);
    DelayMS(delay);
  }
}

void OV2640_BrightnessContrastConfig(I2C_TypeDef* i2c, uint8_t Contrast, uint8_t brightness) {

  DCMI_SingleRandomWrite(i2c, 0xff, 0x00);
  DCMI_SingleRandomWrite(i2c, 0x7c, 0x00);
  DCMI_SingleRandomWrite(i2c, 0x7d, 0x04);
  DCMI_SingleRandomWrite(i2c, 0x7c, 0x07);
  DCMI_SingleRandomWrite(i2c, 0x7d, 0x20);
  DCMI_SingleRandomWrite(i2c, 0x7d, Contrast);
  DCMI_SingleRandomWrite(i2c, 0x7d, brightness);
  DCMI_SingleRandomWrite(i2c, 0x7d, 0x06);
}

void OV2640_BrightnessConfig(I2C_TypeDef* i2c, uint8_t Brightness) {

  DCMI_SingleRandomWrite(i2c, 0xff, 0x00);
  DCMI_SingleRandomWrite(i2c, 0x7c, 0x00);
  DCMI_SingleRandomWrite(i2c, 0x7d, 0x04);
  DCMI_SingleRandomWrite(i2c, 0x7c, 0x09);
  DCMI_SingleRandomWrite(i2c, 0x7d, Brightness);
  DCMI_SingleRandomWrite(i2c, 0x7d, 0x00);
}

void SetCompressionValue(I2C_TypeDef* i2c, int value) {

  //if (value < 8)
  //  value = 8;
  //else if (value > 55)
  //  value = 55;

  DCMI_SingleRandomWrite(i2c, 0xFF, 0x00);
  DelayMS(1);
  DCMI_SingleRandomWrite(i2c, 0x44, value);
  DelayMS(1);
  //DCMI_SingleRandomWrite(i2c, 0xda, 0x10);
  //DelayMS(1);
  //DCMI_SingleRandomWrite(i2c, 0xe0, 0x00);
  //DelayMS(1);
  //DCMI_SingleRandomWrite(i2c, 0xFF, 0x01);
  //DelayMS(1);
  //DCMI_SingleRandomWrite(i2c, 0x04, 0x08);
  //DelayMS(1);
}

void SetBandingConfig(I2C_TypeDef* i2c, int value) {

  // 0 = 60
  // 1 = 50
  // 2 = Auto

  DCMI_SingleRandomWrite(i2c, 0xff, 0x01);
  DelayMS(1);

  switch (value) {
  case 0:
    DCMI_SingleRandomWrite(i2c, 0x0c, 0x3c);
    break;
  case 1:
    DCMI_SingleRandomWrite(i2c, 0x0c, 0x38);
    break;
  case 2:
    DCMI_SingleRandomWrite(i2c, 0x0c, 0x32);
    break;
  }

  DelayMS(1);
}

void ImageFlip(I2C_TypeDef* i2c, bool vFlip, bool hFlip) {

  uint8_t defaultVal;

  if (vFlip)
    defaultVal = defaultVal | 64;

  if (hFlip)
    defaultVal = defaultVal | 128;

  defaultVal = defaultVal | 16;
  defaultVal = defaultVal | 8;

  DCMI_SingleRandomWrite(i2c, 0xff, 0x01); // program sensor (not dsp)
  DelayMS(1);

  DCMI_SingleRandomWrite(i2c, 0x04, defaultVal);
  DelayMS(1);
}

// Returns true if the OrigVal is within the LowLimit and HighLimit range
bool isBetween(uint64_t origVal, uint64_t lowLimit, uint64_t highLimit) {

  if (origVal > lowLimit && origVal < highLimit)
    return true;

  return false;
}

void putImage(uint8_t *dataBuffer, int32_t size) {

  if (_selectedCamera) {

    if (_OutputInterface) {

      UsbPuts("EZIRC");
      UsbPutc32(size);
      VCP_DataTx(dataBuffer, size);
    }
    else {

      fputs("EZIRC");
      fputc32(size);
      fputArrayViaDMA(dataBuffer, size);
    }
  }
  else {

    if (_OutputInterface) {

      UsbPuts("EZIMG");
      UsbPutc32(size);
      VCP_DataTx(dataBuffer, size);
    }
    else {

      fputs("EZIMG");
      fputc32(size);
      fputArrayViaDMA(dataBuffer, size);
    }
  }
}

void sendCameraErrorInitImage() {

  bool flashy = false;

  while (true) {

    putImage((uint8_t *)ErrorCommunicating, ErrorCommunicating_Size);

    if (flashy)
      StatusLED(true, false, false);
    else
      StatusLED(false, false, false);

    flashy = !flashy;
  }
}

void EZBLoop() {

  // always start with the first camera
  _selectedCamera = 0;

  while (true) {

    _selectedCamera = !_selectedCamera;

    StatusLED(false, false, true);

    if (_selectedCamera)
      SetHigh(GPIOE, GPIO_Pin_4);
    else
      SetLow(GPIOE, GPIO_Pin_4);

    DMA2_Stream1->NDTR = IMAGE_SIZE;

    DMA_Cmd(DMA2_Stream1, ENABLE);

    while (DMA_GetCmdStatus(DMA2_Stream1) == DISABLE)
      ;

    DCMI_CaptureCmd(ENABLE);

    while ((DCMI->CR & DCMI_CR_CAPTURE) != 0)
      ;

    StatusLED(false, true, false);

    DMA_Cmd(DMA2_Stream1, DISABLE);

    while (DMA_GetCmdStatus(DMA2_Stream1) == ENABLE)
      ;

    int tmpSize = (IMAGE_SIZE - DMA2_Stream1->NDTR) * 4;

    putImage(JpegBuffer, tmpSize);
  }
}

int main() {

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);   // DCMI
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // USART1
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);   // USART1 DCMI
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);   // Camera #1 I2C
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);   // Camera #2 I2C

  // Camera Select
  // LOW = i2c1
  // HIGH = i2c2
  SetIODirection(GPIOE, GPIO_Pin_4, GPIO_Mode_OUT);

  // Output Select Switch
  // LOW = USB
  // HIGH = UART (3,333,333 bps)
  SetIODirection(GPIOA, GPIO_Pin_3, GPIO_Mode_IN);

  // Set and Initalize the output device
  _OutputInterface = GetPin(GPIOA, GPIO_Pin_3);

  if (_OutputInterface)
    InitUSB();
  else
    InitUART(3333333);

  // Status LED
  SetIODirection(GPIOD, GPIO_Pin_8, GPIO_Mode_OUT); // red
  SetIODirection(GPIOB, GPIO_Pin_13, GPIO_Mode_OUT); // green
  SetIODirection(GPIOB, GPIO_Pin_12, GPIO_Mode_OUT); // blue

  for (int x = 0; x < 3; x++) {

    StatusLED(true, false, false);
    DelayMS(250);
    StatusLED(false, true, false);
    DelayMS(250);
    StatusLED(false, false, true);
    DelayMS(250);
  }

  StatusLED(false, false, false);

  SCCB_GPIO_Config();

  StatusLED(true, true, true);

  MCO1_Init();

  OV2640_DCMI_Config();

  OV2640_DMA_Init();

  _selectedCamera = 0;
  if (!OV2640_JPEGConfig(Open407V_SCCB_1))
    sendCameraErrorInitImage();

  _selectedCamera = 1;
  if (!OV2640_JPEGConfig(Open407V_SCCB_2))
    sendCameraErrorInitImage();

  OV2640_BrightnessContrastConfig(Open407V_SCCB_1, 30, 30);
  OV2640_BrightnessContrastConfig(Open407V_SCCB_2, 12, 12);

  SetCompressionValue(Open407V_SCCB_1, 10);
  SetCompressionValue(Open407V_SCCB_2, 10);

  // auto light banding
  SetBandingConfig(Open407V_SCCB_1, 2);
  SetBandingConfig(Open407V_SCCB_2, 2);

  ImageFlip(Open407V_SCCB_1, true, true);

  EZBLoop();
}









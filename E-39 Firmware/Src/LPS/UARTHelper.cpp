#include "UARTHelper.h"

#define RX_CMD_BUFFER_SIZE 1024

volatile uint8_t  RxBuffer[RX_CMD_BUFFER_SIZE];
volatile uint16_t RxBufferPosition = 0;

void InitUART(uint32_t baud) {

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_USART1);

  SetIODirection(GPIOA, GPIO_Pin_9, GPIO_Mode_AF, GPIO_PuPd_UP); // TX
  SetIODirection(GPIOA, GPIO_Pin_10, GPIO_Mode_AF, GPIO_PuPd_UP); // RX
  SetIODirection(GPIOA, GPIO_Pin_11, GPIO_Mode_AF, GPIO_PuPd_DOWN); // CTS
  SetIODirection(GPIOA, GPIO_Pin_12, GPIO_Mode_AF, GPIO_PuPd_DOWN); // RTS

  USART_InitTypeDef USART_InitStructure;
  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = baud;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);

  DMA_InitTypeDef DMA_InitStructure;

  // Receive
  DMA_StructInit(&DMA_InitStructure);
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)RxBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = RX_CMD_BUFFER_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream2, &DMA_InitStructure);

  // Transmit
  DMA_StructInit(&DMA_InitStructure);
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream7, &DMA_InitStructure);

  USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
  USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

  USART_Cmd(USART1, ENABLE);

  DMA_Cmd(DMA2_Stream2, ENABLE);  // RX
  DMA_Cmd(DMA2_Stream7, DISABLE); // TX
}

void fputArrayViaDMA(uint8_t *data, int32_t size) {

  while (!(USART1->SR & USART_FLAG_TC));

  DMA2_Stream7->NDTR = size;
  DMA2_Stream7->M0AR = (uint32_t)data;

  DMA_Cmd(DMA2_Stream7, ENABLE);

  while (!DMA_GetFlagStatus(DMA2_Stream7, DMA_FLAG_TCIF7))
    ;

  DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7);

  DMA_Cmd(DMA2_Stream7, DISABLE);
}

uint32_t GetRxBufferPosition() {

  return RxBufferPosition;
}

bool kbhit() {

  return (RX_CMD_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA2_Stream2) != RxBufferPosition);
}

uint32_t fgetc32() {

  return
    ((fgetc()) |
    (fgetc() << 8) |
    (fgetc() << 16) |
    (fgetc() << 24));
}

uint16_t fgetc16() {

  return
    ((fgetc()) |
    (fgetc() << 8));
}

uint8_t fgetc() {

  while (RX_CMD_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA2_Stream2) == RxBufferPosition);

  int retVal = RxBuffer[RxBufferPosition];

  RxBufferPosition++;

  if (RxBufferPosition == RX_CMD_BUFFER_SIZE)
    RxBufferPosition = 0;

  return retVal;
}

void fputc64(uint64_t val) {

  fputc(Convert8(val, 0));
  fputc(Convert8(val, 8));
  fputc(Convert8(val, 16));
  fputc(Convert8(val, 24));
  fputc(Convert8(val, 32));
  fputc(Convert8(val, 40));
  fputc(Convert8(val, 48));
  fputc(Convert8(val, 56));
}

void fputc32(uint32_t val) {

  fputc(Convert8(val, 0));
  fputc(Convert8(val, 8));
  fputc(Convert8(val, 16));
  fputc(Convert8(val, 24));
}

void fputc16(uint16_t val) {

  fputc(Convert8(val, 0));
  fputc(Convert8(val, 8));
}

void fputc(uint8_t val) {

  while (!(USART1->SR & USART_FLAG_TC));

  USART1->DR = (val & (uint16_t)0x01FF);
}

void fputs(const char string[]) {

  for (int i = 0; string[i] != 0; i++) {

    while (!(USART1->SR & USART_FLAG_TC));

    USART1->DR = (string[i] & (uint16_t)0x01FF);
  }
}

void fputsnl() {

  while (!(USART1->SR & USART_FLAG_TC));

  USART1->DR = ('\r' & (uint16_t)0x01FF);

  while (!(USART1->SR & USART_FLAG_TC));

  USART1->DR = ('\n' & (uint16_t)0x01FF);
}

void fputsnl(const char string[]) {

  for (int i = 0; string[i] != 0; i++) {

    while (!(USART1->SR & USART_FLAG_TC));

    USART1->DR = (string[i] & (uint16_t)0x01FF);
  }

  while (!(USART1->SR & USART_FLAG_TC));

  USART1->DR = ('\r' & (uint16_t)0x01FF);

  while (!(USART1->SR & USART_FLAG_TC));

  USART1->DR = ('\n' & (uint16_t)0x01FF);
}

void fputi(uint64_t n) {

  int  i;
  int  sign;
  char s[10];

  if ((sign = n) < 0)  /* record sign */
    n = -n;          /* make n positive */

  i = 0;

  do {                       /* generate digits in reverse order */

    s[i++] = n % 10 + '0';   /* get next digit */
  } while ((n /= 10) > 0);   /* delete it */

  if (sign < 0)
    s[i++] = '-';

  s[i] = '\0';

  reverse(s);

  fputs(s);
}


void fputh(uint64_t dec) {

  int c = 10, c2 = 0, c3 = 0;  //counter variables
  const char letter[6] = { 65, 66, 67, 68, 69, 70 };
  const char number[10] = { 48, 49, 50, 51, 52, 53, 54, 55, 56, 57 };
  char hex[8];

  fputs("0x");

  while (dec >= 1) {

    c = 10; c2 = 0;

    if ((dec % 16) < 10) {

      while (c2 != dec % 16)
        c2++;

      hex[c3] = number[c2];
    }

    c2 = 0;

    if ((dec % 16) >= 10) {

      while (dec % 16 != c) {

        c2++; // to get actual letter
        c++;  // to help get place of letter[c] in letter[6] array
      }

      hex[c3] = letter[c2];
    }

    dec /= 16;
    c3++;  // going on to next position in the array for the output of hex conversion
  }

  if (c3 == 0) {

    fputs("00");

    return;
  }

  if (c3 == 1)
    fputc('0');

  while (c3 >= 0) {

    // Array needs to be displayed backwards
    if (c3 <= 0)
      break;

    c3--;

    fputc(hex[c3]);
  }
}
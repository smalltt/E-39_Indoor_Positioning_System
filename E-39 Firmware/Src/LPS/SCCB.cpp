#include "SCCB.h"
#include "UARTHelper.h"

volatile uint32_t  DCMI_TIMEOUT_MAX = SCCB_Open407V_FLAG_TIMEOUT;

void SCCB_GPIO_Config(void) {

  GPIO_InitTypeDef  GPIO_InitStructure;

  // I2C #1
  // -------------------------------------------------------------------------------------------
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  I2C_DeInit(Open407V_SCCB_1);
  I2C_InitTypeDef  SCCB_InitStructure;
  SCCB_InitStructure.I2C_Mode = I2C_Mode_I2C;
  SCCB_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  SCCB_InitStructure.I2C_OwnAddress1 = SCCB_SLAVE_ADDRESS7;
  SCCB_InitStructure.I2C_Ack = I2C_Ack_Enable;
  SCCB_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  SCCB_InitStructure.I2C_ClockSpeed = SCCB_SPEED;

  I2C_Init(Open407V_SCCB_1, &SCCB_InitStructure);
  I2C_Cmd(Open407V_SCCB_1, ENABLE);

  I2C_AcknowledgeConfig(Open407V_SCCB_1, ENABLE);
  
  // I2C #1
  // -------------------------------------------------------------------------------------------
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  I2C_DeInit(Open407V_SCCB_2);
  SCCB_InitStructure.I2C_Mode = I2C_Mode_I2C;
  SCCB_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  SCCB_InitStructure.I2C_OwnAddress1 = SCCB_SLAVE_ADDRESS7;
  SCCB_InitStructure.I2C_Ack = I2C_Ack_Enable;
  SCCB_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  SCCB_InitStructure.I2C_ClockSpeed = SCCB_SPEED;

  I2C_Init(Open407V_SCCB_2, &SCCB_InitStructure);
  I2C_Cmd(Open407V_SCCB_2, ENABLE);

  I2C_AcknowledgeConfig(Open407V_SCCB_2, ENABLE);
}

uint8_t DCMI_SingleRandomWrite(I2C_TypeDef* i2c, uint8_t Reg, uint8_t Data) {

  //fputs("Reg: ");
  //fputh(Reg);
  //fputs(" = ");
  //fputh(Data);
  //fputsnl();

  uint32_t timeout = DCMI_TIMEOUT_MAX;

  /* Generate the Start Condition */
  I2C_GenerateSTART(i2c, ENABLE);

  /* Test on I2C2 EV5 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */

  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_MODE_SELECT))
    if ((timeout--) == 0) return 0xFF;

  /*-----------------------------------------------------------------------------------*/
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(i2c, OV2640_DEVICE_WRITE_ADDRESS, I2C_Direction_Transmitter);

  /* Test on I2C2 EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */

  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    if ((timeout--) == 0) return 0xFF;

  /*-----------------------------------------------------------------------------------*/
  /* Send I2C2 location address LSB */
  I2C_SendData(i2c, (uint8_t)(Reg));

  /* Test on I2C2 EV8 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */

  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    if ((timeout--) == 0) return 0xFF;

  /*-----------------------------------------------------------------------------------*/
  /* Send Data */
  I2C_SendData(i2c, Data);

  /* Test on I2C2 EV8 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */

  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    if ((timeout--) == 0) return 0xFF;

  /*-----------------------------------------------------------------------------------*/
  /* Send I2C2 STOP Condition */
  I2C_GenerateSTOP(i2c, ENABLE);

  /* If operation is OK, return 0 */
  return 0;
}

/**
  * @brief  Reads a byte from a specific Camera register
  * @param  Device: OV2640 write address.
  * @param  Addr: OV2640 register address.
  * @retval data read from the specific register or 0xFF if timeout condition
  *         occured.
  */
uint8_t DCMI_SingleRandomRead(I2C_TypeDef* i2c, uint8_t Reg, uint8_t *Data) {

  uint32_t timeout = DCMI_TIMEOUT_MAX;

  /* Clear AF flag if arised */
  I2C2->SR1 |= (uint16_t)0x0400;

  /* Generate the Start Condition */
  I2C_GenerateSTART(i2c, ENABLE);

  /* Test on I2C2 EV5 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */

  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_MODE_SELECT)) {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }

  /*-----------------------------------------------------------------------------------*/
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(i2c, OV2640_DEVICE_READ_ADDRESS, I2C_Direction_Transmitter);

  /* Test on I2C2 EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */

  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }

  /*-----------------------------------------------------------------------------------*/
  /* Send I2C2 location address LSB */
  I2C_SendData(i2c, (uint8_t)(Reg));

  /* Test on I2C2 EV8 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */

  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }

  /* Clear AF flag if arised */
  I2C2->SR1 |= (uint16_t)0x0400;
  /*-----------------------------------------------------------------------------------*/
  /* Prepare Stop after receiving data */
  I2C_GenerateSTOP(i2c, ENABLE);

  /*-----------------------------------------------------------------------------------*/
  /* Generate the Start Condition */
  I2C_GenerateSTART(i2c, ENABLE);

  /* Test on I2C2 EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */

  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_MODE_SELECT)) {

    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }

  /*-----------------------------------------------------------------------------------*/
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(i2c, OV2640_DEVICE_READ_ADDRESS, I2C_Direction_Receiver);

  /* Test on I2C2 EV6 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */

  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }

  /*-----------------------------------------------------------------------------------*/
  /* Prepare an NACK for the next data received */
  I2C_AcknowledgeConfig(i2c, DISABLE);

  /* Test on Open407V_SCCB EV7 and clear it */
  timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */

  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
    /* If the timeout delay is exeeded, exit with error code */
    if ((timeout--) == 0) return 0xFF;
  }

  /*-----------------------------------------------------------------------------------*/
  /* Prepare Stop after receiving data */
  I2C_GenerateSTOP(i2c, ENABLE);

  /*-----------------------------------------------------------------------------------*/
  /* Receive the Data */
  *Data = I2C_ReceiveData(i2c);

  /* Clear AF flag if arised */
  I2C2->SR1 |= (uint16_t)0x0400;

  /* return the read data */
  return 0;
}

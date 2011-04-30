/**
  ******************************************************************************
  * @file OptimizedI2Cexamples/src/I2CRoutines.c
  * @author  MCD Application Team
  * @version  V4.0.0
  * @date  06/18/2010
  * @brief  Contains the I2Cx slave/Master read and write routines.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "I2CRoutines.h"

/* Buffer of data to be received by I2C1 */
static uint8_t Buffer_Rx1[255];
/* Buffer of data to be transmitted by I2C1 */
static uint8_t Buffer_Tx1[255] = {0x5, 0x6,0x8,0xA};
/* Buffer of data to be received by I2C2 */
static uint8_t Buffer_Rx2[255];
/* Buffer of data to be transmitted by I2C2 */
static uint8_t Buffer_Tx2[255] = {0xF, 0xB, 0xC,0xD};
static __IO uint8_t Tx_Idx1=0, Rx_Idx1=0;
static __IO uint8_t Tx_Idx2=0, Rx_Idx2=0;


/** @addtogroup Optimized I2C examples
  * @{
  */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
DMA_InitTypeDef  I2CDMA_InitStructure;
__IO uint32_t I2CDirection = I2C_DIRECTION_TX;
__IO uint32_t NumbOfBytes1;
__IO uint32_t NumbOfBytes2;
__IO uint8_t Address;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Reads buffer of bytes  from the slave.
  * @param pBuffer: Buffer of bytes to be read from the slave.
  * @param NumByteToRead: Number of bytes to be read by the Master.
  * @param Mode: Polling or DMA or Interrupt having the highest priority in the application.
  * @param SlaveAddress: The address of the slave to be addressed by the Master.
  * @retval : None.
  */
Status I2C_Master_BufferRead(I2C_TypeDef* I2Cx, uint8_t* pBuffer,  uint32_t NumByteToRead, I2C_ProgrammingModel Mode, uint8_t SlaveAddress)

{
    __IO uint32_t temp = 0;
    __IO uint32_t Timeout = 0;

    /* Enable I2C errors interrupts (used in all modes: Polling, DMA and Interrupts */
    I2Cx->CR2 |= I2C_IT_ERR;

    if (Mode == DMA) /* I2Cx Master Reception using DMA */
    {
        /* Configure I2Cx DMA channel */
        I2C_DMAConfig(I2Cx, pBuffer, NumByteToRead, I2C_DIRECTION_RX);
        /* Set Last bit to have a NACK on the last received byte */
        I2Cx->CR2 |= CR2_LAST_Set;
        /* Enable I2C DMA requests */
        I2Cx->CR2 |= CR2_DMAEN_Set;
        Timeout = 0xFFFF;
        /* Send START condition */
        I2Cx->CR1 |= CR1_START_Set;
        /* Wait until SB flag is set: EV5  */
        while ((I2Cx->SR1&0x0001) != 0x0001)
        {
            if (Timeout-- == 0)
                return Error;
        }
        Timeout = 0xFFFF;
        /* Send slave address */
        /* Set the address bit0 for read */
        SlaveAddress |= OAR1_ADD0_Set;
        Address = SlaveAddress;
        /* Send the slave address */
        I2Cx->DR = Address;
        /* Wait until ADDR is set: EV6 */
        while ((I2Cx->SR1&0x0002) != 0x0002)
        {
            if (Timeout-- == 0)
                return Error;
        }
        /* Clear ADDR flag by reading SR2 register */
        temp = I2Cx->SR2;
        if (I2Cx == I2C1)
        {
            /* Wait until DMA end of transfer */
            while (!DMA_GetFlagStatus(DMA1_FLAG_TC7));
            /* Disable DMA Channel */
            DMA_Cmd(I2C1_DMA_CHANNEL_RX, DISABLE);
            /* Clear the DMA Transfer Complete flag */
            DMA_ClearFlag(DMA1_FLAG_TC7);

        }

        else /* I2Cx = I2C2*/
        {
            /* Wait until DMA end of transfer */
            while (!DMA_GetFlagStatus(DMA1_FLAG_TC5));
            /* Disable DMA Channel */
            DMA_Cmd(I2C2_DMA_CHANNEL_RX, DISABLE);
            /* Clear the DMA Transfer Complete flag */
            DMA_ClearFlag(DMA1_FLAG_TC5);
        }
        /* Program the STOP */
        I2Cx->CR1 |= CR1_STOP_Set;
        /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
        while ((I2Cx->CR1&0x200) == 0x200);
    }

    else if (Mode == Polling) /* I2Cx Master Reception using Polling */
    {


        if (NumByteToRead == 1)
        {
            Timeout = 0xFFFF;
            /* Send START condition */
            I2Cx->CR1 |= CR1_START_Set;
            /* Wait until SB flag is set: EV5  */
            while ((I2Cx->SR1&0x0001) != 0x0001)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            /* Send slave address */
            /* Reset the address bit0 for read */
            SlaveAddress |= OAR1_ADD0_Set;
            Address = SlaveAddress;
            /* Send the slave address */
            I2Cx->DR = Address;
            /* Wait until ADDR is set: EV6_3, then program ACK = 0, clear ADDR
            and program the STOP just after ADDR is cleared. The EV6_3 
            software sequence must complete before the current byte end of transfer.*/
            /* Wait until ADDR is set */
            Timeout = 0xFFFF;
            while ((I2Cx->SR1&0x0002) != 0x0002)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            /* Clear ACK bit */
            I2Cx->CR1 &= CR1_ACK_Reset;
            /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
            software sequence must complete before the current byte end of transfer */
            __disable_irq();
            /* Clear ADDR flag */
            temp = I2Cx->SR2;
            /* Program the STOP */
            I2Cx->CR1 |= CR1_STOP_Set;
            /* Re-enable IRQs */
            __enable_irq();
            /* Wait until a data is received in DR register (RXNE = 1) EV7 */
            while ((I2Cx->SR1 & 0x00040) != 0x000040);
            /* Read the data */
            *pBuffer = I2Cx->DR;
            /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
            while ((I2Cx->CR1&0x200) == 0x200);
            /* Enable Acknowledgement to be ready for another reception */
            I2Cx->CR1 |= CR1_ACK_Set;

        }

        else if (NumByteToRead == 2)
        {
            /* Set POS bit */
            I2Cx->CR1 |= CR1_POS_Set;
            Timeout = 0xFFFF;
            /* Send START condition */
            I2Cx->CR1 |= CR1_START_Set;
            /* Wait until SB flag is set: EV5 */
            while ((I2Cx->SR1&0x0001) != 0x0001)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            Timeout = 0xFFFF;
            /* Send slave address */
            /* Set the address bit0 for read */
            SlaveAddress |= OAR1_ADD0_Set;
            Address = SlaveAddress;
            /* Send the slave address */
            I2Cx->DR = Address;
            /* Wait until ADDR is set: EV6 */
            while ((I2Cx->SR1&0x0002) != 0x0002)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            /* EV6_1: The acknowledge disable should be done just after EV6,
            that is after ADDR is cleared, so disable all active IRQs around ADDR clearing and 
            ACK clearing */
            __disable_irq();
            /* Clear ADDR by reading SR2 register  */
            temp = I2Cx->SR2;
            /* Clear ACK */
            I2Cx->CR1 &= CR1_ACK_Reset;
            /*Re-enable IRQs */
            __enable_irq();
            /* Wait until BTF is set */
            while ((I2Cx->SR1 & 0x00004) != 0x000004);
            /* Disable IRQs around STOP programming and data reading because of the limitation ?*/
            __disable_irq();
            /* Program the STOP */
            I2C_GenerateSTOP(I2Cx, ENABLE);
            /* Read first data */
            *pBuffer = I2Cx->DR;
            /* Re-enable IRQs */
            __enable_irq();
            /**/
            pBuffer++;
            /* Read second data */
            *pBuffer = I2Cx->DR;
            /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
            while ((I2Cx->CR1&0x200) == 0x200);
            /* Enable Acknowledgement to be ready for another reception */
            I2Cx->CR1  |= CR1_ACK_Set;
            /* Clear POS bit */
            I2Cx->CR1  &= CR1_POS_Reset;

        }

        else

        {

            Timeout = 0xFFFF;
            /* Send START condition */
            I2Cx->CR1 |= CR1_START_Set;
            /* Wait until SB flag is set: EV5 */
            while ((I2Cx->SR1&0x0001) != 0x0001)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            Timeout = 0xFFFF;
            /* Send slave address */
            /* Reset the address bit0 for write */
            SlaveAddress |= OAR1_ADD0_Set;;
            Address = SlaveAddress;
            /* Send the slave address */
            I2Cx->DR = Address;
            /* Wait until ADDR is set: EV6 */
            while ((I2Cx->SR1&0x0002) != 0x0002)
            {
                if (Timeout-- == 0)
                    return Error;
            }
            /* Clear ADDR by reading SR2 status register */
            temp = I2Cx->SR2;
            /* While there is data to be read */
            while (NumByteToRead)
            {
                /* Receive bytes from first byte until byte N-3 */
                if (NumByteToRead != 3)
                {
                    /* Poll on BTF to receive data because in polling mode we can not guarantee the
                    EV7 software sequence is managed before the current byte transfer completes */
                    while ((I2Cx->SR1 & 0x00004) != 0x000004);
                    /* Read data */
                    *pBuffer = I2Cx->DR;
                    /* */
                    pBuffer++;
                    /* Decrement the read bytes counter */
                    NumByteToRead--;
                }

                /* it remains to read three data: data N-2, data N-1, Data N */
                if (NumByteToRead == 3)
                {

                    /* Wait until BTF is set: Data N-2 in DR and data N -1 in shift register */
                    while ((I2Cx->SR1 & 0x00004) != 0x000004);
                    /* Clear ACK */
                    I2Cx->CR1 &= CR1_ACK_Reset;

                    /* Disable IRQs around data reading and STOP programming because of the
                    limitation ? */
                    __disable_irq();
                    /* Read Data N-2 */
                    *pBuffer = I2Cx->DR;
                    /* Increment */
                    pBuffer++;
                    /* Program the STOP */
                    I2Cx->CR1 |= CR1_STOP_Set;
                    /* Read DataN-1 */
                    *pBuffer = I2Cx->DR;
                    /* Re-enable IRQs */
                    __enable_irq();
                    /* Increment */
                    pBuffer++;
                    /* Wait until RXNE is set (DR contains the last data) */
                    while ((I2Cx->SR1 & 0x00040) != 0x000040);
                    /* Read DataN */
                    *pBuffer = I2Cx->DR;
                    /* Reset the number of bytes to be read by master */
                    NumByteToRead = 0;

                }
            }
            /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
            while ((I2Cx->CR1&0x200) == 0x200);
            /* Enable Acknowledgement to be ready for another reception */
            I2Cx->CR1 |= CR1_ACK_Set;

        }

    }

    else /* I2Cx Master Reception using Interrupts with highest priority in an application */
    {
        /* Enable EVT IT*/
        I2Cx->CR2 |= I2C_IT_EVT;
        /* Enable BUF IT */
        I2Cx->CR2 |= I2C_IT_BUF;
        /* Set the I2C direction to reception */
        I2CDirection = I2C_DIRECTION_RX;
        SlaveAddress |= OAR1_ADD0_Set;
        Address = SlaveAddress;
        if (I2Cx == I2C1)    NumbOfBytes1 = NumByteToRead;
        else NumbOfBytes2 = NumByteToRead;
        /* Send START condition */
        I2Cx->CR1 |= CR1_START_Set;
        /* Wait until the START condition is generated on the bus: START bit is cleared by hardware */
        while ((I2Cx->CR1&0x100) == 0x100);
        /* Wait until BUSY flag is reset (until a STOP is generated) */
        while ((I2Cx->SR2 &0x0002) == 0x0002);
        /* Enable Acknowledgement to be ready for another reception */
        I2Cx->CR1 |= CR1_ACK_Set;
    }

    return Success;
}



/**
  * @brief  Writes buffer of bytes.
  * @param pBuffer: Buffer of bytes to be sent to the slave.
  * @param NumByteToWrite: Number of bytes to be sent by the Master.
  * @param Mode: Polling or DMA or Interrupt having the highest priority in the application.
  * @param SlaveAddress: The address of the slave to be addressed by the Master.
  * @retval : None.
  */
Status I2C_Master_BufferWrite(I2C_TypeDef* I2Cx, uint8_t* pBuffer,  uint32_t NumByteToWrite, I2C_ProgrammingModel Mode, uint8_t SlaveAddress )

{

    __IO uint32_t temp = 0;
    __IO uint32_t Timeout = 0;

    /* Enable Error IT (used in all modes: DMA, Polling and Interrupts */
    I2Cx->CR2 |= I2C_IT_ERR;
    if (Mode == DMA)  /* I2Cx Master Transmission using DMA */
    {
        Timeout = 0xFFFF;
        /* Configure the DMA channel for I2Cx transmission */
        I2C_DMAConfig (I2Cx, pBuffer, NumByteToWrite, I2C_DIRECTION_TX);
        /* Enable the I2Cx DMA requests */
        I2Cx->CR2 |= CR2_DMAEN_Set;
        /* Send START condition */
        I2Cx->CR1 |= CR1_START_Set;
        /* Wait until SB flag is set: EV5 */
        while ((I2Cx->SR1&0x0001) != 0x0001)
        {
            if (Timeout-- == 0)
                return Error;
        }
        Timeout = 0xFFFF;
        /* Send slave address */
        /* Reset the address bit0 for write */
        SlaveAddress &= OAR1_ADD0_Reset;
        Address = SlaveAddress;
        /* Send the slave address */
        I2Cx->DR = Address;
        /* Wait until ADDR is set: EV6 */
        while ((I2Cx->SR1&0x0002) != 0x0002)
        {
            if (Timeout-- == 0)
                return Error;
        }

        /* Clear ADDR flag by reading SR2 register */
        temp = I2Cx->SR2;
        if (I2Cx == I2C1)
        {
            /* Wait until DMA end of transfer */
            while (!DMA_GetFlagStatus(DMA1_FLAG_TC6));
            /* Disable the DMA1 Channel 6 */
            DMA_Cmd(I2C1_DMA_CHANNEL_TX, DISABLE);
            /* Clear the DMA Transfer complete flag */
            DMA_ClearFlag(DMA1_FLAG_TC6);
        }
        else  /* I2Cx = I2C2 */
        {
            /* Wait until DMA end of transfer */
            while (!DMA_GetFlagStatus(DMA1_FLAG_TC4));
            /* Disable the DMA1 Channel 4 */
            DMA_Cmd(I2C2_DMA_CHANNEL_TX, DISABLE);
            /* Clear the DMA Transfer complete flag */
            DMA_ClearFlag(DMA1_FLAG_TC4);
        }

        /* EV8_2: Wait until BTF is set before programming the STOP */
        while ((I2Cx->SR1 & 0x00004) != 0x000004);
        /* Program the STOP */
        I2Cx->CR1 |= CR1_STOP_Set;
        /* Make sure that the STOP bit is cleared by Hardware */
        while ((I2Cx->CR1&0x200) == 0x200);

    }
    else if (Mode == Polling) /* I2Cx Master Transmission using Polling */
    {

        Timeout = 0xFFFF;
        /* Send START condition */
        I2Cx->CR1 |= CR1_START_Set;
        /* Wait until SB flag is set: EV5 */
        while ((I2Cx->SR1&0x0001) != 0x0001)
        {
            if (Timeout-- == 0)
                return Error;
        }

        /* Send slave address */
        /* Reset the address bit0 for write*/
        SlaveAddress &= OAR1_ADD0_Reset;
        Address = SlaveAddress;
        /* Send the slave address */
        I2Cx->DR = Address;
        Timeout = 0xFFFF;
        /* Wait until ADDR is set: EV6 */
        while ((I2Cx->SR1 &0x0002) != 0x0002)
        {
            if (Timeout-- == 0)
                return Error;
        }

        /* Clear ADDR flag by reading SR2 register */
        temp = I2Cx->SR2;
        /* Write the first data in DR register (EV8_1) */
        I2Cx->DR = *pBuffer;
        /* Increment */
        pBuffer++;
        /* Decrement the number of bytes to be written */
        NumByteToWrite--;
        /* While there is data to be written */
        while (NumByteToWrite--)
        {
            /* Poll on BTF to receive data because in polling mode we can not guarantee the
              EV8 software sequence is managed before the current byte transfer completes */
            while ((I2Cx->SR1 & 0x00004) != 0x000004);
            /* Send the current byte */
            I2Cx->DR = *pBuffer;
            /* Point to the next byte to be written */
            pBuffer++;
        }
        /* EV8_2: Wait until BTF is set before programming the STOP */
        while ((I2Cx->SR1 & 0x00004) != 0x000004);
        /* Send STOP condition */
        I2Cx->CR1 |= CR1_STOP_Set;
        /* Make sure that the STOP bit is cleared by Hardware */
        while ((I2Cx->CR1&0x200) == 0x200);

    }

    else /* I2Cx Master Transmission using Interrupt with highest priority in the application */

    {
        /* Enable EVT IT*/
        I2Cx->CR2 |= I2C_IT_EVT;
        /* Enable BUF IT */
        I2Cx->CR2 |= I2C_IT_BUF;
        /* Set the I2C direction to Transmission */
        I2CDirection = I2C_DIRECTION_TX;
        SlaveAddress &= OAR1_ADD0_Reset;
        Address = SlaveAddress;
        if (I2Cx == I2C1)    NumbOfBytes1 = NumByteToWrite;
        else NumbOfBytes2 = NumByteToWrite;
        /* Send START condition */
        I2Cx->CR1 |= CR1_START_Set;
        /* Wait until the START condition is generated on the bus: the START bit is cleared by hardware */
        while ((I2Cx->CR1&0x100) == 0x100);
        /* Wait until BUSY flag is reset: a STOP has been generated on the bus signaling the end
        of transmission */
        while ((I2Cx->SR2 &0x0002) == 0x0002);
    }

    return Success;

}


/**
  * @brief Prepares the I2Cx slave for transmission.
  * @param I2Cx: I2C1 or I2C2.
  * @param Mode: DMA or Interrupt having the highest priority in the application.
  * @retval : None.
  */

void I2C_Slave_BufferReadWrite(I2C_TypeDef* I2Cx,I2C_ProgrammingModel Mode)

{
    /* Enable Event IT needed for ADDR and STOPF events ITs */
    I2Cx->CR2 |= I2C_IT_EVT ;
    /* Enable Error IT */
    I2Cx->CR2 |= I2C_IT_ERR;

    if (Mode == DMA)  /* I2Cx Slave Transmission using DMA */
    {
        /* Enable I2Cx DMA requests */
        I2Cx->CR2 |= CR2_DMAEN_Set;
    }

    else  /* I2Cx Slave Transmission using Interrupt with highest priority in the application */
    {
        /* Enable Buffer IT (TXE and RXNE ITs) */
        I2Cx->CR2 |= I2C_IT_BUF;

    }

}

/**
* @brief  Initializes peripherals: I2Cx, GPIO, DMA channels .
  * @param  None
  * @retval None
  */
void I2C_LowLevel_Init(I2C_TypeDef* I2Cx)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    /* GPIOB clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    /* Enable the DMA1 clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    if (I2Cx == I2C1)
    {
        /* I2C1 clock enable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
        /* I2C1 SDA and SCL configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* Enable I2C1 reset state */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
        /* Release I2C1 from reset state */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
    }

    else /* I2Cx = I2C2 */

    {

        /* I2C2 clock enable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
        /* I2C1 SDA and SCL configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* Enable I2C2 reset state */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, ENABLE);
        /* Release I2C2 from reset state */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE);
    }

    /* I2C1 and I2C2 configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = OwnAddress1;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = ClockSpeed;
    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_InitStructure.I2C_OwnAddress1 = OwnAddress2;
    I2C_Init(I2C2, &I2C_InitStructure);

    if (I2Cx == I2C1)

    {   /* I2C1 TX DMA Channel configuration */
        DMA_DeInit(I2C1_DMA_CHANNEL_TX);
        I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
        I2CDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;   /* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;    /* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_BufferSize = 0xFFFF;            /* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        I2CDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        I2CDMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
        I2CDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        I2CDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        I2CDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
        I2CDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init(I2C1_DMA_CHANNEL_TX, &I2CDMA_InitStructure);

        /* I2C1 RX DMA Channel configuration */
        DMA_DeInit(I2C1_DMA_CHANNEL_RX);
        DMA_Init(I2C1_DMA_CHANNEL_RX, &I2CDMA_InitStructure);
    }

    else /* I2Cx = I2C2 */

    {
        /* I2C2 TX DMA Channel configuration */
        DMA_DeInit(I2C2_DMA_CHANNEL_TX);
        I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C2_DR_Address;
        I2CDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;   /* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;    /* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_BufferSize = 0xFFFF;            /* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        I2CDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        I2CDMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
        I2CDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        I2CDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        I2CDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
        I2CDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init(I2C2_DMA_CHANNEL_TX, &I2CDMA_InitStructure);

        /* I2C2 RX DMA Channel configuration */
        DMA_DeInit(I2C2_DMA_CHANNEL_RX);
        DMA_Init(I2C2_DMA_CHANNEL_RX, &I2CDMA_InitStructure);

    }
}

/**
  * @brief  Initializes DMA channel used by the I2C Write/read routines.
  * @param  None.
  * @retval None.
  */
void I2C_DMAConfig(I2C_TypeDef* I2Cx, uint8_t* pBuffer, uint32_t BufferSize, uint32_t Direction)
{
    /* Initialize the DMA with the new parameters */
    if (Direction == I2C_DIRECTION_TX)
    {
        /* Configure the DMA Tx Channel with the buffer address and the buffer size */
        I2CDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer;
        I2CDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        I2CDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;

        if (I2Cx == I2C1)
        {
            I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
            DMA_Cmd(I2C1_DMA_CHANNEL_TX, DISABLE);
            DMA_Init(I2C1_DMA_CHANNEL_TX, &I2CDMA_InitStructure);
            DMA_Cmd(I2C1_DMA_CHANNEL_TX, ENABLE);
        }
        else
        {
            I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C2_DR_Address;
            DMA_Cmd(I2C2_DMA_CHANNEL_TX, DISABLE);
            DMA_Init(I2C2_DMA_CHANNEL_TX, &I2CDMA_InitStructure);
            DMA_Cmd(I2C2_DMA_CHANNEL_TX, ENABLE);
        }
    }
    else /* Reception */
    {
        /* Configure the DMA Rx Channel with the buffer address and the buffer size */
        I2CDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer;
        I2CDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        I2CDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;
        if (I2Cx == I2C1)
        {

            I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
            DMA_Cmd(I2C1_DMA_CHANNEL_RX, DISABLE);
            DMA_Init(I2C1_DMA_CHANNEL_RX, &I2CDMA_InitStructure);
            DMA_Cmd(I2C1_DMA_CHANNEL_RX, ENABLE);
        }

        else
        {
            I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C2_DR_Address;
            DMA_Cmd(I2C2_DMA_CHANNEL_RX, DISABLE);
            DMA_Init(I2C2_DMA_CHANNEL_RX, &I2CDMA_InitStructure);
            DMA_Cmd(I2C2_DMA_CHANNEL_RX, ENABLE);
        }

    }
}


/**
  * @brief  This function handles I2C1 Event interrupt request.
  * @param  None
  * @retval : None
  */
void I2C1_EV_IRQHandler(void)
{

    __IO uint32_t SR1Register =0;
    __IO uint32_t SR2Register =0;
    


#ifdef SLAVE_DMA_USE
    /* Read SR1 register */
    SR1Register = I2C1->SR1;

    /* If ADDR is set */
    if ((SR1Register & 0x0002) == 0x0002)
    {
        /* In slave Transmitter/Receiver mode, when using DMA, it is recommended to update the buffer 
          base address and the buffer size before clearing ADDR flag. In fact, the only
          period when the slave has control  on the bus(SCL is stretched so master can not initiate 
          transfers) is the period between ADDR is set and ADDR is cleared. Otherwise, the master can
          initiate transfers and the buffer size & the buffer address have not yet been updated.*/

        /* Update the DMA channels memory base address and count */
        I2C_DMAConfig (I2C1, Buffer_Tx1, 0xFFFF, I2C_DIRECTION_TX);
        I2C_DMAConfig (I2C1, Buffer_Rx1, 0xFFFF, I2C_DIRECTION_RX);
        /* Clear ADDR by reading SR2 register */
        SR2Register = I2C1->SR2;
    }
#else
    /* Read the I2C1 SR1 and SR2 status registers */
    SR1Register = I2C1->SR1;
    SR2Register = I2C1->SR2;

    /* If I2C1 is slave (MSL flag = 0) */
    if ((SR2Register &0x0001) != 0x0001)
    {
        /* If ADDR = 1: EV1 */
        if ((SR1Register & 0x0002) == 0x0002)
        {
            /* Clear SR1Register and SR2Register variables to prepare for next IT */
            SR1Register = 0;
            SR2Register = 0;
            /* Initialize the transmit/receive counters for next transmission/reception
            using Interrupt  */
            Tx_Idx1 = 0;
            Rx_Idx1 = 0;
        }
        /* If TXE = 1: EV3 */
        if ((SR1Register & 0x0080) == 0x0080)
        {
            /* Write data in data register */
            I2C1->DR = Buffer_Tx1[Tx_Idx1++];
            SR1Register = 0;
            SR2Register = 0;
        }
        /* If RXNE = 1: EV2 */
        if ((SR1Register & 0x0040) == 0x0040)
        {
            /* Read data from data register */
            Buffer_Rx1[Rx_Idx1++] = I2C1->DR;
            SR1Register = 0;
            SR2Register = 0;

        }
        /* If STOPF =1: EV4 (Slave has detected a STOP condition on the bus */
        if (( SR1Register & 0x0010) == 0x0010)
        {
            I2C1->CR1 |= CR1_PE_Set;
            SR1Register = 0;
            SR2Register = 0;

        }
    } /* End slave mode */

#endif

    /* If SB = 1, I2C1 master sent a START on the bus: EV5) */
    if ((SR1Register &0x0001) == 0x0001)
    {

        /* Send the slave address for transmssion or for reception (according to the configured value
            in the write master write routine */
        I2C1->DR = Address;
        SR1Register = 0;
        SR2Register = 0;
    }
    /* If I2C1 is Master (MSL flag = 1) */

    if ((SR2Register &0x0001) == 0x0001)
    {
        /* If ADDR = 1, EV6 */
        if ((SR1Register &0x0002) == 0x0002)
        {
            /* Write the first data in case the Master is Transmitter */
            if (I2CDirection == I2C_DIRECTION_TX)
            {
                /* Initialize the Transmit counter */
                Tx_Idx1 = 0;
                /* Write the first data in the data register */
                I2C1->DR = Buffer_Tx1[Tx_Idx1++];
                /* Decrement the number of bytes to be written */
                NumbOfBytes1--;
                /* If no further data to be sent, disable the I2C BUF IT
                in order to not have a TxE  interrupt */
                if (NumbOfBytes1 == 0)
                {
                    I2C1->CR2 &= (uint16_t)~I2C_IT_BUF;
                }

            }
            /* Master Receiver */
            else

            {
                /* Initialize Receive counter */
                Rx_Idx1 = 0;
                /* At this stage, ADDR is cleared because both SR1 and SR2 were read.*/
                /* EV6_1: used for single byte reception. The ACK disable and the STOP
                Programming should be done just after ADDR is cleared. */
                if (NumbOfBytes1 == 1)
                {
                    /* Clear ACK */
                    I2C1->CR1 &= CR1_ACK_Reset;
                    /* Program the STOP */
                    I2C1->CR1 |= CR1_STOP_Set;
                }
            }
            SR1Register = 0;
            SR2Register = 0;

        }
        /* Master transmits the remaing data: from data2 until the last one.  */
        /* If TXE is set */
        if ((SR1Register &0x0084) == 0x0080)
        {
            /* If there is still data to write */
            if (NumbOfBytes1!=0)
            {
                /* Write the data in DR register */
                I2C1->DR = Buffer_Tx1[Tx_Idx1++];
                /* Decrment the number of data to be written */
                NumbOfBytes1--;
                /* If  no data remains to write, disable the BUF IT in order
                to not have again a TxE interrupt. */
                if (NumbOfBytes1 == 0)
                {
                    /* Disable the BUF IT */
                    I2C1->CR2 &= (uint16_t)~I2C_IT_BUF;
                }
            }
            SR1Register = 0;
            SR2Register = 0;
        }
        /* If BTF and TXE are set (EV8_2), program the STOP */
        if ((SR1Register &0x0084) == 0x0084)
        {

            /* Program the STOP */
            I2C1->CR1 |= CR1_STOP_Set;
            /* Disable EVT IT In order to not have again a BTF IT */
            I2C1->CR2 &= (uint16_t)~I2C_IT_EVT;
            SR1Register = 0;
            SR2Register = 0;
        }
        /* If RXNE is set */
        if ((SR1Register &0x0040) == 0x0040)
        {
            /* Read the data register */
            Buffer_Rx1[Rx_Idx1++] = I2C1->DR;
            /* Decrement the number of bytes to be read */
            NumbOfBytes1--;
            /* If it remains only one byte to read, disable ACK and program the STOP (EV7_1) */
            if (NumbOfBytes1 == 1)
            {
                /* Clear ACK */
                I2C1->CR1 &= CR1_ACK_Reset;
                /* Program the STOP */
                I2C1->CR1 |= CR1_STOP_Set;
            }
            SR1Register = 0;
            SR2Register = 0;
        }

    }


}

/**
  * @}
  */

/**
  * @brief  This function handles I2C1 Event interrupt request.
  * @param  None
  * @retval : None
  */
void I2C2_EV_IRQHandler(void)
{

    __IO uint32_t SR1Register =0;
    __IO uint32_t SR2Register =0;
    

#ifdef SLAVE_DMA_USE

    /* Read SR1 register */
    SR1Register = I2C2->SR1;

    /* If ADDR is set */
    if ((SR1Register & 0x0002) == 0x0002)
    {
        /* In slave Transmitter/Receiver mode, when using DMA, the update of the buffer base address
          and the buffer size should be done before clearing ADDR flag. In fact, the only
          period when the slave has control  on the bus(SCL is stretched so master can not initiate 
          transfers) is the period between ADDR is set and ADDR is cleared. otherwise, the master can
          initiate transfers and the buffer size & the buffer address have not yet updated.*/

        /* Update the DMA channels memory base address and count */
        I2C_DMAConfig (I2C2, Buffer_Tx2, 0xFFFF, I2C_DIRECTION_TX);
        I2C_DMAConfig (I2C2, Buffer_Rx2, 0xFFFF, I2C_DIRECTION_RX);
        /* Clear ADDR by reading SR2 register */
        SR2Register = I2C2->SR2;
    }
#else
    /* Read the I2C1 SR1 and SR2 status registers */
    SR1Register = I2C2->SR1;
    SR2Register = I2C2->SR2;

    /* If I2C2 is slave (MSL flag = 0) */
    if ((SR2Register &0x0001) != 0x0001)
    {
        /* If ADDR = 1: EV1 */
        if ((SR1Register & 0x0002) == 0x0002)
        {
            /* Clear SR1Register SR2Register variables to prepare for next IT*/
            SR1Register = 0;
            SR2Register = 0;
            /* Initialize the transmit/receive counters for next transmission/reception
            using Interrupt  */
            Tx_Idx2 = 0;
            Rx_Idx2 = 0;
        }
        /* If TXE = 1: EV3 */
        if ((SR1Register & 0x0080) == 0x0080)
        {
            /* Write data in data register */
            I2C2->DR = Buffer_Tx2[Tx_Idx2++];
            SR1Register = 0;
            SR2Register = 0;
        }
        /* If RXNE = 1: EV2 */
        if ((SR1Register & 0x0040) == 0x0040)
        {
            /* Read data from data register */
            Buffer_Rx2[Rx_Idx2++] = I2C2->DR;
            SR1Register = 0;
            SR2Register = 0;

        }
        /* If STOPF =1: EV4 (Slave has detected a STOP condition on the bus */
        if (( SR1Register & 0x0010) == 0x0010)
        {
            I2C2->CR1 |= CR1_PE_Set;
            SR1Register = 0;
            SR2Register = 0;
        }
    } /* End slave mode */

#endif

    /* If SB = 1, I2C1 master sent a START on the bus: EV5) */
    if ((SR1Register &0x0001) == 0x0001)
    {

        /* Send the slave address for transmssion or for reception (according to the configured value
            in the write master write routine */
        I2C2->DR = Address;
        SR1Register = 0;
        SR2Register = 0;
    }
    /* If I2C2 is Master (MSL flag = 1) */

    if ((SR2Register &0x0001) == 0x0001)
    {
        /* If ADDR = 1, EV6 */
        if ((SR1Register &0x0002) == 0x0002)
        {
            /* Write the first data in case the Master is Transmitter */
            if (I2CDirection == I2C_DIRECTION_TX)
            {
                /* Initialize the Transmit counter */
                Tx_Idx2 = 0;
                /* Write the first data in the data register */
                I2C2->DR = Buffer_Tx1[Tx_Idx2++];
                /* Decrement the number of bytes to be written */
                NumbOfBytes2--;
                /* If no further data to be sent, disable the I2C BUF IT
                in order to not have a TxE  interrupt */
                if (NumbOfBytes2 == 0)
                {
                    I2C2->CR2 &= (uint16_t)~I2C_IT_BUF;
                }

            }
            /* Master Receiver */
            else

            {
                /* Initialize Receive counter */
                Rx_Idx2 = 0;
                /* At this stage, ADDR is cleared because both SR1 and SR2 were read.*/
                /* EV6_1: used for single byte reception. The ACK disable and the STOP
                Programming should be done just after ADDR is cleared. */
                if (NumbOfBytes2 == 1)
                {
                    /* Clear ACK */
                    I2C2->CR1 &= CR1_ACK_Reset;
                    /* Program the STOP */
                    I2C2->CR1 |= CR1_STOP_Set;
                }
            }
            SR1Register = 0;
            SR2Register = 0;

        }
        /* Master transmits the remaing data: from data2 until the last one.  */
        /* If TXE is set */
        if ((SR1Register &0x0084) == 0x0080)
        {
            /* If there is still data to write */
            if (NumbOfBytes2!=0)
            {
                /* Write the data in DR register */
                I2C2->DR = Buffer_Tx2[Tx_Idx2++];
                /* Decrment the number of data to be written */
                NumbOfBytes2--;
                /* If  no data remains to write, disable the BUF IT in order
                to not have again a TxE interrupt. */
                if (NumbOfBytes2 == 0)
                {
                    /* Disable the BUF IT */
                    I2C2->CR2 &= (uint16_t)~I2C_IT_BUF;
                }
            }
            SR1Register = 0;
            SR2Register = 0;
        }
        /* If BTF and TXE are set (EV8_2), program the STOP */
        if ((SR1Register &0x0084) == 0x0084)
        {

            /* Program the STOP */
            I2C2->CR1 |= CR1_STOP_Set;
            /* Disable EVT IT In order to not have again a BTF IT */
            I2C2->CR2 &= (uint16_t)~I2C_IT_EVT;
            SR1Register = 0;
            SR2Register = 0;
        }
        /* If RXNE is set */
        if ((SR1Register &0x0040) == 0x0040)
        {
            /* Read the data register */
            Buffer_Rx2[Rx_Idx2++] = I2C2->DR;
            /* Decrement the number of bytes to be read */
            NumbOfBytes2--;

            /* If it remains only one byte to read, disable ACK and program the STOP (EV7_1) */
            if (NumbOfBytes2 == 1)
            {
                /* Clear ACK */
                I2C2->CR1 &= CR1_ACK_Reset;
                /* Program the STOP */
                I2C2->CR1 |= CR1_STOP_Set;
            }
            SR1Register = 0;
            SR2Register = 0;
        }



    }


}
/**
  * @}
  */

/**
  * @brief  This function handles I2C2 Error interrupt request.
  * @param  None
  * @retval : None
  */
void I2C2_ER_IRQHandler(void)
{

    __IO uint32_t SR1Register =0;

    /* Read the I2C1 status register */
    SR1Register = I2C2->SR1;
    /* If AF = 1 */
    if ((SR1Register & 0x0400) == 0x0400)
    {
        I2C2->SR1 &= 0xFBFF;
        SR1Register = 0;
    }
    /* If ARLO = 1 */
    if ((SR1Register & 0x0200) == 0x0200)
    {
        I2C2->SR1 &= 0xFBFF;
        SR1Register = 0;
    }
    /* If BERR = 1 */
    if ((SR1Register & 0x0100) == 0x0100)
    {
        I2C2->SR1 &= 0xFEFF;
        SR1Register = 0;
    }

    /* If OVR = 1 */

    if ((SR1Register & 0x0800) == 0x0800)
    {
        I2C2->SR1 &= 0xF7FF;
        SR1Register = 0;
    }
}



/**
  * @brief  This function handles I2C1 Error interrupt request.
  * @param  None
  * @retval : None
  */
void I2C1_ER_IRQHandler(void)
{

    __IO uint32_t SR1Register =0;

    /* Read the I2C1 status register */
    SR1Register = I2C1->SR1;
    /* If AF = 1 */
    if ((SR1Register & 0x0400) == 0x0400)
    {
        I2C1->SR1 &= 0xFBFF;
        SR1Register = 0;
    }
    /* If ARLO = 1 */
    if ((SR1Register & 0x0200) == 0x0200)
    {
        I2C1->SR1 &= 0xFBFF;
        SR1Register = 0;
    }
    /* If BERR = 1 */
    if ((SR1Register & 0x0100) == 0x0100)
    {
        I2C1->SR1 &= 0xFEFF;
        SR1Register = 0;
    }

    /* If OVR = 1 */

    if ((SR1Register & 0x0800) == 0x0800)
    {
        I2C1->SR1 &= 0xF7FF;
        SR1Register = 0;
    }
}






/**
  * @}
  */


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

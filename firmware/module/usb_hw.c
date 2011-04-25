/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2007
 *
 *    File name   : STM32F_usb.c
 *    Description : usb module (HAL)
 *
 *    History :
 *    1. Date        : July 28, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *    2. Date        : November 20, 2006
 *       Author      : Stanimir Bonev
 *       Description : Modify
 *      Fix problems with double buffered EPs
 *    3. Date        : April 30, 2007
 *       Author      : Stanimir Bonev
 *       Description : Modify
 *      Adapt for STM32F
 *    4. Date        : July 20, 2007
 *       Author      : Stanimir Bonev
 *       Description : Modify
 *      Adapt for USB framework 2
 *
 *    $Revision: 1.2.2.4 $
 **************************************************************************/

#define USB_HW_GLOBAL
#include "usb_hw.h"

#pragma section="USB_PACKET_MEMORY"

#pragma location="USB_PACKET_MEMORY"
__root __no_init Int32U USB_PacketMemory[256];

pPacketMemUse_t pPacketMemUse;

PacketMemUse_t PacketMemBuff[EP_MAX_SLOTS*2];

Int32U DlyCnt;

static const UsbStandardEpDescriptor_t USB_CtrlEpDescr0 =
{
  sizeof(UsbStandardEpDescriptor_t),
  UsbDescriptorEp,
  UsbEpOut(CTRL_ENP_OUT>>1),
  {(Int8U)UsbEpTransferControl | (Int8U)UsbEpSynchNoSynchronization | (Int8U)UsbEpUsageData},
  Ep0MaxSize,
  0
};

static const UsbEP_ExtData_t USB_CtrlEpExt0 =
{
  EP_SLOT0, FALSE
};

static const UsbStandardEpDescriptor_t USB_CtrlEpDescr1 =
{
  sizeof(UsbStandardEpDescriptor_t),
  UsbDescriptorEp,
  UsbEpIn(CTRL_ENP_IN>>1),
  {(Int8U)UsbEpTransferControl | (Int8U)UsbEpSynchNoSynchronization | (Int8U)UsbEpUsageData},
  Ep0MaxSize,
  0
};

static const UsbEP_ExtData_t USB_CtrlEpExt1 =
{
  EP_SLOT0, FALSE
};

/*************************************************************************
 * Function Name: EpCtrlSet_EA
 * Parameters: __IO pInt32U pReg, Int32U Data
 *
 * Return: none
 *
 * Description: Set EP Addr
 *
 *************************************************************************/
void EpCtrlSet_EA(__IO pInt32U pReg, Int32U Data)
{
  Data &= 0xF;
  Data |= *pReg & 0x0700 ;
  Data |= USB_CTRL_CLEAR_ONLY_MASK;
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlGet_EA
 * Parameters: __IO pInt32U pReg
 *
 * Return: Int16U
 *
 * Description: Get EP Addr
 *
 *************************************************************************/
inline
Int16U EpCtrlGet_EA(__IO pInt32U pReg)
{
  return(*pReg & 0x0F);
}

/*************************************************************************
 * Function Name: EpCtrlSet_STAT_TX
 * Parameters: __IO pInt32U pReg, EpState_t Data
 *
 * Return: none
 *
 * Description: Set EP IN State
 *
 *************************************************************************/
void EpCtrlSet_STAT_TX (__IO pInt32U pReg, EpState_t Data)
{
Int32U State;
  // The EP can change state between read and write
  // operations from VALID to NAK and result of
  // set operation will be invalid!
  if(EpCtrlGet_STAT_TX(pReg) != (Int32U)Data)
  {
    State = Data;
    State <<= 4;
    State ^= *pReg;
    State |= USB_CTRL_CLEAR_ONLY_MASK;
    State &= ~0x7040;  // Clear the toggle bits without STAT_TX (4,5)
    *pReg = State;
  }
  else
  {
    return;
  }
  if(EpCtrlGet_STAT_TX(pReg) != (Int32U)Data)
  {
    State = Data;
    State <<= 4;
    State ^= *pReg;
    State |= USB_CTRL_CLEAR_ONLY_MASK;
    State &= ~0x7040;  // Clear the toggle bits without STAT_TX (4,5)
    *pReg = State;
  }
}

/*************************************************************************
 * Function Name: EpCtrlGet_STAT_TX
 * Parameters: __IO pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP IN Status
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_STAT_TX (__IO pInt32U pReg)
{
Int32U Data = *pReg;
  return((Data & (0x3UL << 4)) >> 4);
}

/*************************************************************************
 * Function Name: EpCtrlSet_DTOG_TX
 * Parameters: __IO pInt32U pReg, Int32U Data
 *
 * Return: none
 *
 * Description: Set EP DTOG_TX bit (IN)
 *
 *************************************************************************/
void EpCtrlSet_DTOG_TX(__IO pInt32U pReg, Int32U Data)
{
  Data  = Data?(*pReg ^ (1UL<<6)):*pReg;
  Data &= ~0x7030;   // Clear the toggle bits without DTOG_TX (6)
  Data |= USB_CTRL_CLEAR_ONLY_MASK;
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlToggle_DTOG_TX
 * Parameters: __IO pInt32U pReg
 *
 * Return: none
 *
 * Description: Toggle EP DTOG_TX bit (IN)
 *
 *************************************************************************/
void EpCtrlToggle_DTOG_TX(__IO pInt32U pReg)
{
Int32U Data = *pReg | (1UL << 6);
  Data &= ~0x7030;   // Clear the toggle bits without DTOG_TX (6)
  Data |= USB_CTRL_CLEAR_ONLY_MASK;
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlGet_DTOG_TX
 * Parameters: __IO pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP DTOG_TX bit (IN)
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_DTOG_TX(__IO pInt32U pReg)
{
  return(((*pReg & (1UL<<6))?1:0));
}

/*************************************************************************
 * Function Name: EpCtrlClr_CTR_TX
 * Parameters: __IO pInt32U pReg
 *
 * Return: none
 *
 * Description: Clear EP CTR_TX bit (IN)
 *
 *************************************************************************/
void EpCtrlClr_CTR_TX(__IO pInt32U pReg)
{
Int32U Data = *pReg;
  Data &= ~(USB_CTRL_TOGGLE_MASK | 1UL << 7);
  Data |= 1UL <<15;   // Set RX_CTR
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlGet_CTR_TX
 * Parameters: __IO pInt32U pReg
 *
 * Return: UserFunc_t
 *
 * Description: Get EP CTR_TX bit (IN)
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_CTR_TX(__IO pInt32U pReg)
{
  return(((*pReg & (0x1UL << 7))?1:0));
}

/*************************************************************************
 * Function Name: EpCtrlSet_EP_KIND
 * Parameters: __IO pInt32U pReg, Int32U Data
 *
 * Return: none
 *
 * Description: Set EP KIND bit
 *
 *************************************************************************/
void EpCtrlSet_EP_KIND (__IO pInt32U pReg, Int32U Data)
{
  Data  = Data ? (1UL << 8):0;
  Data |= *pReg & ~(USB_CTRL_TOGGLE_MASK | (1UL << 8));
  Data |= USB_CTRL_CLEAR_ONLY_MASK;
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlGet_EP_KIND
 * Parameters: __IO pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP KIND bit
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_EP_KIND(__IO pInt32U pReg)
{
  return(*pReg & (0x1UL << 8)?1:0);
}

/*************************************************************************
 * Function Name: EpCtrlSet_EP_TYPE
 * Parameters: __IO pInt32U pReg, EpType_t Data
 *
 * Return: none
 *
 * Description: Set EP type
 *
 *************************************************************************/
void EpCtrlSet_EP_TYPE(__IO pInt32U pReg, EpType_t Data)
{
Int32U Type;
  Type = Data;
  Type <<= 9;
  Type  |= *pReg & ~(USB_CTRL_TOGGLE_MASK | (0x3UL << 9));
  Type  |= USB_CTRL_CLEAR_ONLY_MASK;
  *pReg = Type;
}

/*************************************************************************
 * Function Name: EpCtrlGet_EP_TYPE
 * Parameters: __IO pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP type
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_EP_TYPE(__IO pInt32U pReg)
{
Int32U Data = *pReg;
  Data &= (3UL<<9);
  return(Data >> 9);
}

/*************************************************************************
 * Function Name: EpCtrlGet_SETUP
 * Parameters: __IO pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP SETUP bit
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_SETUP(__IO pInt32U pReg)
{
Int32U Data = *pReg & (0x1UL << 11);
  return(Data?1:0);
}

/*************************************************************************
 * Function Name: EpCtrlSet_STAT_RX
 * Parameters: __IO pInt32U pReg, EpState_t Data
 *
 * Return: none
 *
 * Description: Set EP STAT_RX (OUT)
 *
 *************************************************************************/
void EpCtrlSet_STAT_RX (__IO pInt32U pReg, EpState_t Data)
{
Int32U State;
  // The EP can change state between read and write
  // operations from VALID to NAK and result of
  // set operation will be invalid!
  if(EpCtrlGet_STAT_RX(pReg) != (Int32U)Data)
  {
    State = Data;
    State <<= 12;
    State  ^= *pReg;
    State  |= USB_CTRL_CLEAR_ONLY_MASK;
    State  &= ~0x4070;  // Clear the toggle bits without STAT_RX (12,13)
    *pReg  = State;
  }
  else
  {
    return;
  }
  if(EpCtrlGet_STAT_RX(pReg) != (Int32U)Data)
  {
    State = Data;
    State <<= 12;
    State  ^= *pReg;
    State  |= USB_CTRL_CLEAR_ONLY_MASK;
    State  &= ~0x4070;  // Clear the toggle bits without STAT_RX (12,13)
    *pReg  = State;
  }
}

/*************************************************************************
 * Function Name: EpCtrlGet_STAT_RX
 * Parameters: __IO pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP STAT_RX (OUT)
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_STAT_RX(__IO pInt32U pReg)
{
Int32U Data = *pReg &( 0x3UL << 12);
  return(Data >> 12);
}

/*************************************************************************
 * Function Name: EpCtrlSet_DTOG_RX
 * Parameters: __IO pInt32U pReg, Int32U Data
 *
 * Return: none
 *
 * Description: Set DTOG_RX bit
 *
 *************************************************************************/
void EpCtrlSet_DTOG_RX(__IO pInt32U pReg, Int32U Data)
{
  Data  = Data?(*pReg ^ (1UL<<14)):*pReg;
  Data &= ~0x3070;   // Clear the toggle bits without DTOG_RX (14)
  Data |= USB_CTRL_CLEAR_ONLY_MASK;
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlToggle_DTOG_RX
 * Parameters: __IO Int16U pReg
 *
 * Return: none
 *
 * Description: Toggle DTOG_RX bit
 *
 *************************************************************************/
void EpCtrlToggle_DTOG_RX(__IO pInt32U pReg)
{
Int32U Data = *pReg | (1UL << 14);
  Data &= ~0x3070;   // Clear the toggle bits without DTOG_TX (14)
  Data |= USB_CTRL_CLEAR_ONLY_MASK;
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlGet_DTOG_RX
 * Parameters: __IO pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP DTOG_RX bit (OUT)
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_DTOG_RX(__IO pInt32U pReg)
{
  return(((*pReg & (1UL<<14))?1:0));
}

/*************************************************************************
 * Function Name: EpCtrlClr_CTR_RX
 * Parameters: __IO pInt32U pReg
 *
 * Return: none
 *
 * Description: Clear EP CTR_RX bit (OUT)
 *
 *************************************************************************/
void EpCtrlClr_CTR_RX(__IO pInt32U pReg)
{
Int32U Data = *pReg;
  Data &= ~(USB_CTRL_TOGGLE_MASK | 1UL << 15);
  Data |= 1UL << 7;   // Set TX_CTR
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlGet_CTR_RX
 * Parameters: __IO pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP CTR_RX bit (OUT)
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_CTR_RX(__IO pInt32U pReg)
{
  return(*pReg & (0x1UL << 15)?1:0);
}

/*************************************************************************
 * Function Name: USB_HwInit
 * Parameters: none
 *
 * Return: none
 *
 * Description: Init USB
 *
 *************************************************************************/
void USB_HwInit(void)
{
NVIC_InitTypeDef NVIC_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

  // Reset USB Engine
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_USB,ENABLE);
  // Init USB Clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB,ENABLE);
  // Release reset USB Engine
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_USB,DISABLE);
  // Enable 48MHz clock
  RCC_USBCLKConfig(USB_DIVIDER);

  // Force USB Reset &  Disable USB interrupts
  USB_CNTR = 1;

  // GPIO assign to the USB engine
  RCC_APB2PeriphResetCmd(  RCC_APB2Periph_GPIOA
                         | RCC_APB2Periph_GPIOC
                         | RCC_APB2Periph_AFIO, DISABLE);
  RCC_APB2PeriphClockCmd(  RCC_APB2Periph_GPIOA
                         | RCC_APB2Periph_GPIOC
                         | RCC_APB2Periph_AFIO, ENABLE);

  GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE);  // xxx
  // Configure PA11, PA12 as USB lines
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // Configure PC11 USB connect.
  GPIO_WriteBit(GPIOC,GPIO_Pin_11,Bit_SET);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  // Disconnect device
  USB_ConnectRes(FALSE);

  // Init controls endpoints
  USB_HwReset();

  // Clear spurious pending interrupt
  USB_ISTR = 0;

  // USB interrupt connect to NVIC
#if USB_HIGH_PRIORITY_EVENT > 0
  NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USB_INTR_HIGH_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif // USB_HIGH_PRIORITY_EVENT > 0

  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USB_INTR_LOW_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

/*************************************************************************
 * Function Name: USB_HwReset
 * Parameters: none
 *
 * Return: none
 *
 * Description: Reset USB engine
 *
 *************************************************************************/
void USB_HwReset (void)
{
Int32U Count;

  // Clear realized EP flag
  for(Count = 0; Count < ENP_MAX_NUMB; Count++)
  {
    EpCnfg[Count].pEpCtrl = NULL;
  }

  // Free entire USB packet memory
  for (Count = 0; Count < EP_MAX_SLOTS*2; Count++)
  {
    PacketMemBuff[Count].Size = 0;
  }

  // Resume delay counter
  DlyCnt = 0;

  // USB_Configure
  USB_Configure(FALSE);
  // Set Buffer table address
  USB_BTABLE = USB_OffsetOfDBT;
  // Init Packet memory table
  pPacketMemUse = NULL;
  // Control EP Init
  USB_RealizeEp(&USB_CtrlEpDescr0,&USB_CtrlEpExt0,TRUE);
  USB_RealizeEp(&USB_CtrlEpDescr1,&USB_CtrlEpExt1,TRUE);

  // Set address 0
  USB_SetDefAdd();

  // Enable Device interrupts
  USB_CNTR = bmCTRM | bmRESETM | bmSUSPM | bmWKUPM |
            (USB_SOF_EVENT    ? bmSOFM    : 0) |
            (USB_ERROR_EVENT  ? bmERRM    : 0) |
            (USB_PMAOVR_EVENT ? bmPMAOVRM : 0) ;
}

/*************************************************************************
 * Function Name: UsbMalloc
 * Parameters: none
 *
 * Return: void *
 *
 * Description: Return address of free element of the PacketMemBuff massive
 *
 *************************************************************************/
static
void * UsbMalloc(void)
{
Int32U Count;
  for (Count = 0; Count < EP_MAX_SLOTS*2; Count++)
  {
    if (PacketMemBuff[Count].Size == 0)
    {
      return(&PacketMemBuff[Count]);
    }
  }
  return(NULL);
}

/*************************************************************************
 * Function Name: UsbFree
 * Parameters: pPacketMemUse_t pPntr
 *
 * Return: none
 *
 * Description: Free element of the PacketMemBuff massive
 *
 *************************************************************************/
inline static
void UsbFree(pPacketMemUse_t pPntr)
{
  pPntr->Size = 0;
}

/*************************************************************************
 * Function Name: USB_AllocateBuffer
 * Parameters: pInt16U pOffset,Int32U PacketSize,
 *             USB_Endpoint_t EndPoint
 *
 * Return: Boolean
 *
 * Description: Allocate a new buffer
 *
 *************************************************************************/
static
Boolean USB_AllocateBuffer (pInt16U pOffset,pInt32U pPacketSize,
                            USB_Endpoint_t EndPoint)
{
pPacketMemUse_t  pPacketMem = pPacketMemUse, pPacketMemNext, pPacketMemUseNew;
//Int32U MaxPacketSize = *pPacketSize+4;  // for CRC
Int32U MaxPacketSize = *pPacketSize;
  if(MaxPacketSize > 62)
  {
    // coarse-granularity size alignment by 32
    MaxPacketSize +=  (32-1);
    MaxPacketSize &= ~(32-1);
  }
  else
  {
    // fine-granularity size alignment by 2
    MaxPacketSize +=  (2-1);
    MaxPacketSize &= ~(2-1);
  }
  *pOffset = 0;
  // Finding free memory block from the USB packet memory
  while(pPacketMem != NULL)
  {
    *pOffset = pPacketMem->Start + pPacketMem->Size;
    // Offset alignment by 4
    *pOffset += (4-1);
    *pOffset &= ~(4-1);
    pPacketMemNext = pPacketMem->pNext;
    if((pPacketMem->pNext == NULL) ||
       (pPacketMemNext->Start >= *pOffset + MaxPacketSize))
    {
      break;
    }
    pPacketMem = pPacketMem->pNext;
  }
  // Is enough free space for new block?
  if((*pOffset + MaxPacketSize) >=
     (((Int32U)__segment_size("USB_PACKET_MEMORY") -
       sizeof(USB_BuffDeskTbl_t) * (MaxIndOfRealizeEp+1))&~7))
  {
    // Not enough space in the USB packet memory
    return(FALSE);
  }

  pPacketMemUseNew = UsbMalloc();
  if(pPacketMemUseNew == NULL)
  {
    // too many point are realized
    return(FALSE);
  }
  if(pPacketMemUse == NULL)
  {
    pPacketMemUse = pPacketMemUseNew;
    pPacketMemUse->pNext = NULL;
  }
  else
  {
    pPacketMemUseNew->pNext  = pPacketMem->pNext;
    pPacketMem->pNext        = pPacketMemUseNew;
  }
  pPacketMemUseNew->RpAddr = EndPoint;
  pPacketMemUseNew->Start  = *pOffset;
  pPacketMemUseNew->Size   = MaxPacketSize;
  *pPacketSize = MaxPacketSize;
  return(TRUE);
}

/*************************************************************************
 * Function Name: USB_ReleaseBuffer
 * Parameters: USB_Endpoint_t EndPoint
 *
 * Return: none
 *
 * Description: Release the buffer/s
 *
 *************************************************************************/
static
void USB_ReleaseBuffer (USB_Endpoint_t EndPoint)
{
pPacketMemUse_t  pPacketMem, pPacketMemPrev;
  pPacketMem = pPacketMemUse;
  while(pPacketMem != NULL)
  {
    if(pPacketMem->RpAddr == EndPoint)
    {
      pPacketMemPrev->pNext = pPacketMem->pNext;
      UsbFree(pPacketMem);
      pPacketMem = pPacketMemPrev->pNext;
    }
    else
    {
      pPacketMemPrev = pPacketMem;
      pPacketMem = pPacketMem->pNext;
    }
    if (pPacketMem == NULL)
    {
      break;
    }
  }
}

/*************************************************************************
 * Function Name: USB_RealizeEp
 * Parameters: const UsbStandardEpDescriptor_t * pEP_Desc,
 *             const UsbEP_ExtData_t * pUsbEP_ExtData, Boolean Enable
 *
 * Return: USB_ErrorCodes_t
 *
 * Description: Enable or disable an endpoint
 *
 *************************************************************************/
USB_ErrorCodes_t USB_RealizeEp(const UsbStandardEpDescriptor_t * pEP_Desc,
                               const UsbEP_ExtData_t * pUsbEP_ExtData,
                               Boolean Enable)
{
USB_Endpoint_t EP;
pEpCnfg_t pEP;
__IO pInt32U pEpCtrlBaseAddr;
Int16U Offset,Offset1;
Int32U MaxPacketSizeTmp;

  assert(pEP_Desc);

  EP = (USB_Endpoint_t)USB_EpLogToPhysAdd(pEP_Desc->bEndpointAddress);
  pEP = &EpCnfg[EP];

  if (Enable)
  {
    assert(pUsbEP_ExtData);

    // Allocate packet memory for EP buffer/s
    // calculate actual size only for the OUT EPs
    MaxPacketSizeTmp = pEP_Desc->wMaxPacketSize;
    if(!USB_AllocateBuffer(&Offset,&MaxPacketSizeTmp,EP))
    {
      return(USB_MEMORY_FULL);
    }
    if(pUsbEP_ExtData->DoubleBuff)
    {
      // To use double buffered and ISO EP the USB_HIGH_INTR must be enabled
      assert(USB_HIGH_PRIORITY_EVENT);
      // Allocate packet second buffer
      MaxPacketSizeTmp = pEP_Desc->wMaxPacketSize;
      if(!USB_AllocateBuffer(&Offset1,&MaxPacketSizeTmp,EP))
      {
        // release buffer
        USB_ReleaseBuffer(EP);
        return(USB_MEMORY_FULL);
      }
    }

    // Set EP status
    pEP->Status  = NOT_READY;
    // Init EP flags
    pEP->Flags = 0;

    // Set endpoint type
    pEP->EpType = (UsbEpTransferType_t)pEP_Desc->bmAttributes.TransferType;
    // Init EP max packet size
    pEP->MaxSize = pEP_Desc->wMaxPacketSize;
    // Set Extra data
    pEP->bDoubleBuffered = pUsbEP_ExtData->DoubleBuff;
    pEP->EpSlot = pUsbEP_ExtData->EpSlot;

    if (EP & 1)
    {
      pEP->AvbBuff = EpCnfg[EP].bDoubleBuffered + 1;
    }
    else
    {
      pEP->AvbBuff = 0;
    }

    pEpCtrlBaseAddr  = (pInt32U)&USB_EP0R;
    pEpCtrlBaseAddr +=  pUsbEP_ExtData->EpSlot;

    // Set Ep Address
    EpCtrlSet_EA(pEpCtrlBaseAddr,EP >> 1);

    pEP->pEpCtrl = pEpCtrlBaseAddr;

    if(EP & 1)
    {
      // IN EP
      // Disable EP
      EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_DISABLED);
      // Clear Tx toggle
      EpCtrlSet_DTOG_TX(pEpCtrlBaseAddr,0);
      // Clear Correct Transfer for transmission flag
      EpCtrlClr_CTR_TX(pEpCtrlBaseAddr);
      // Update EP description table
      WriteEpDTB_AddrTx(pEP->EpSlot,Offset);
      WriteEpDTB_CountTx(pEP->EpSlot,0);
      // Set EP Kind & enable
      switch(pEP->EpType)
      {
      case UsbEpTransferControl:
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_CTRL);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        // Enable EP
        EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_NAK);
        break;
      case UsbEpTransferIsochronous:
        assert(pEP->bDoubleBuffered);
        // Disable receiving (only singe direction is possible)
        EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_DISABLED);
        WriteEpDTB_AddrRx(pEP->EpSlot,Offset1);
        WriteEpDTB_CountRx(pEP->EpSlot,0);
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_ISO);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        // Enable EP
        EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_VALID);
        break;
      case UsbEpTransferBulk:
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_BULK);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,pEP->bDoubleBuffered);
        if (pEP->bDoubleBuffered)
        {
          // EP_BULK_DOUB_BUF
          // Disable receiving (only singe direction is possible)
          EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_DISABLED);
          // Clear Tx Software toggle
          WriteEpDTB_AddrRx(pEP->EpSlot,Offset1);
          WriteEpDTB_CountRx(pEP->EpSlot,0);
          EpCtrlSet_DTOG_RX(pEpCtrlBaseAddr,0);
        }
        // Enable EP
        EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_NAK);

        break;
      case UsbEpTransferInterrupt:
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_INTERRUPT);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        // Enable EP
        EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_NAK);
        break;
      default:
        assert(0);
      }
      // Clear Correct Transfer for transmission flag
      EpCtrlClr_CTR_TX(pEpCtrlBaseAddr);
    }
    else
    {
      RxCount_t RxCount = {0};
      // OUT EP
      // Disable EP
      EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_DISABLED);
      // Clear Rx toggle
      EpCtrlSet_DTOG_RX(pEpCtrlBaseAddr,0);
      // Clear Correct Transfer for reception flag
      EpCtrlClr_CTR_RX(pEpCtrlBaseAddr);
      // Update EP description table
      MaxPacketSizeTmp = pEP_Desc->wMaxPacketSize;
      RxCount.BlSizeField = (MaxPacketSizeTmp > 62);
      RxCount.NubBlockField = (MaxPacketSizeTmp > 62) ? (MaxPacketSizeTmp>>5)-1:MaxPacketSizeTmp>>1;
      WriteEpDTB_AddrRx(pEP->EpSlot,Offset);
      WriteEpDTB_CountRx(pEP->EpSlot,RxCount.Count);

      // Set EP Kind & enable
      switch(pEP->EpType)
      {
      case UsbEpTransferControl:
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_CTRL);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        // Enable EP
        EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_NAK);
        break;
      case UsbEpTransferIsochronous:
        WriteEpDTB_CountTx(pEP->EpSlot,RxCount.Count);
        // Disable transmitting (only singe direction is possible)
        EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_DISABLED);
        WriteEpDTB_AddrTx(pEP->EpSlot,Offset1);
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_ISO);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        // Enable EP
        EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_VALID);
        break;
      case UsbEpTransferBulk:
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_BULK);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,pEP->bDoubleBuffered);
        if (pEP->bDoubleBuffered)
        {
          // EP_BULK_DOUB_BUF
          // Disable transmitting (only singe direction is possible)
          EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_DISABLED);
          WriteEpDTB_CountTx(pEP->EpSlot,RxCount.Count);
          WriteEpDTB_AddrTx(pEP->EpSlot,Offset1);
          EpCtrlSet_DTOG_TX(pEpCtrlBaseAddr,0);
        }
        // Enable EP
        EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_VALID);
        break;
      case UsbEpTransferInterrupt:
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_INTERRUPT);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        // Enable EP
        EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_VALID);
        break;
      default:
        assert(0);
      }
    }
  }
  else
  {
    if (EpCnfg[EP].pEpCtrl)
    {
      pEpCtrlBaseAddr  = (pInt32U)&USB_EP0R;
      pEpCtrlBaseAddr += EpCnfg[EP].EpSlot;

      if(EP & 1)
      {
        // Disable IN EP
        EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_DISABLED);
        // Clear Correct Transfer for reception flag
        EpCtrlClr_CTR_TX(pEpCtrlBaseAddr);
      }
      else
      {
        // Disable OUT EP
        EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_DISABLED);
        // Clear Correct Transfer for reception flag
        EpCtrlClr_CTR_RX(pEpCtrlBaseAddr);
      }
      // release buffer
      USB_ReleaseBuffer(EP);
      EpCnfg[EP].pEpCtrl = NULL;
    }
  }
  return(USB_OK);
}

/*************************************************************************
 * Function Name: USB_SetAdd
 * Parameters: Int32U DevAdd - device address between 0 - 127
 *
 * Return: none
 *
 * Description: Set device address
 *
 *************************************************************************/
void USB_SetAdd(Int32U DevAdd)
{
  USB_DADDR = DevAdd | 0x80;
}

/*************************************************************************
 * Function Name: USB_ConnectRes
 * Parameters: Boolean Conn
 *
 * Return: none
 *
 * Description: Enable Pull-Up resistor
 *
 *************************************************************************/
void USB_ConnectRes (Boolean Conn)
{
  GPIO_WriteBit(GPIOC,GPIO_Pin_11,((Conn)?Bit_RESET:Bit_SET));
}

/*************************************************************************
 * Function Name: USB_Suspend
 * Parameters: void
 *
 * Return: none
 *
 * Description: Suspend the USB engine
 *
 *************************************************************************/
static
void USB_Suspend(void)
{
  USB_CNTR_bit.FSUSP  = 1;
  USB_CNTR_bit.LPMODE = 1;
}

#if USB_REMOTE_WAKEUP != 0
/*************************************************************************
 * Function Name: USB_WakeUp
 * Parameters: none
 *
 * Return: none
 *
 * Description: Wake up Usb
 *
 *************************************************************************/
void USB_WakeUp (void)
{
  USB_Resume(USB_RESUME_SOFT_EVENT);
}
#endif // USB_REMOTE_WAKEUP != 0

/*************************************************************************
 * Function Name: USB_Resume
 * Parameters: UsbResumeEvent_t UsbResumeEvent
 *
 * Return: none
 *
 * Description: USB Resume implement
 *
 *************************************************************************/
static
void USB_Resume (UsbResumeEvent_t UsbResumeEvent)
{
Int32U LineStates;
  switch(UsbResumeEvent)
  {
  case USB_RESUME_SOF_EVENT:
    if(DlyCnt)
    {
      // Waiting 8ms (not take more than 10ms) before release signal to host
      if(--DlyCnt == 0)
      {
        USB_CNTR_bit.ESOFM  = 0;
        USB_CNTR_bit.RESUME = 0;
        USB_CNTR_bit.FSUSP  = 0;  // Must be 0
      }
    }
    break;
  case USB_RESUME_SOFT_EVENT: // resume by USER
    USB_CNTR_bit.LPMODE = 0;
    DlyCnt = USB_RESUME_DLY;
    USB_CNTR_bit.RESUME = 1;
    USB_CNTR_bit.ESOFM  = 1;
    UsbDevSuspendCallback(FALSE);
    break;
  case USB_RESUME_WAKE_UP_EVENT:  // resume by HOST
    LineStates = USB_FNR & 0xC000;
    if((LineStates == 0x8000) && (LineStates == 0x3000))
    {
      // noise return to suspend state again
      USB_Suspend();
    }
    else if(LineStates)
    {
      // wake-up
      USB_CNTR_bit.LPMODE = 0;
      USB_CNTR_bit.FSUSP  = 0;  // Must be 0
      UsbDevSuspendCallback(FALSE);
    }
    break;
  }
}

/*************************************************************************
 * Function Name: USB_GetDevStatus
 * Parameters: USB_DevStatusReqType_t Type
 *
 * Return: Boolean
 *
 * Description: Return USB device status
 *
 *************************************************************************/
Boolean USB_GetDevStatus (USB_DevStatusReqType_t Type)
{
  switch (Type)
  {
  case USB_DevConnectStatus:
    return(TRUE);
  case USB_SuspendStatus:
    return(USB_CNTR_bit.FSUSP);
  case USB_ResetStatus:
    return(USB_CNTR_bit.FRES);
  }
  return(FALSE);
}

/*************************************************************************
 * Function Name: USB_SetStallEP
 * Parameters: USB_Endpoint_t EP, Boolean Stall
 *
 * Return: USB_ErrorCodes_t
 *
 * Description: The endpoint stall/unstall
 *
 *************************************************************************/
USB_ErrorCodes_t USB_SetStallEP (USB_Endpoint_t EP, Boolean Stall)
{
  if (EpCnfg[EP].pEpCtrl == NULL)
  {
    return(USB_EP_NOT_VALID);
  }

  if(Stall)
  {
    EpCnfg[EP].Status = STALLED;
    if(EP & 1)
    {
      // IN EP
      EpCtrlSet_STAT_TX(EpCnfg[EP].pEpCtrl,EP_STALL);
      EpCnfg[EP].AvbBuff = EpCnfg[EP].bDoubleBuffered+1;
    }
    else
    {
      // OUT EP
      EpCtrlSet_STAT_RX(EpCnfg[EP].pEpCtrl,EP_STALL);
      EpCnfg[EP].AvbBuff = 0;
    }
  }
  else
  {
    EpCnfg[EP].Status = NOT_READY;
    if(EP & 1)
    {
      // IN EP
      EpCnfg[EP].AvbBuff = EpCnfg[EP].bDoubleBuffered+1;
      // reset Data Toggle bit
      if (EpCnfg[EP].bDoubleBuffered)
      {
        EpCtrlSet_DTOG_RX(EpCnfg[EP].pEpCtrl,0);
      }
      EpCtrlSet_DTOG_TX(EpCnfg[EP].pEpCtrl,0);
      EpCtrlSet_STAT_TX(EpCnfg[EP].pEpCtrl,EP_NAK);
    }
    else
    {
      // OUT EP
      EpCnfg[EP].AvbBuff = 0;
      // reset Data Toggle bit
      if (EpCnfg[EP].bDoubleBuffered)
      {
        EpCtrlSet_DTOG_TX(EpCnfg[EP].pEpCtrl,0);
      }
      EpCtrlSet_DTOG_RX(EpCnfg[EP].pEpCtrl,0);
      EpCtrlSet_STAT_RX(EpCnfg[EP].pEpCtrl,EP_VALID);
    }
  }
  return(USB_OK);
}

/*************************************************************************
 * Function Name: USB_StallCtrlEP
 * Parameters: none
 *
 * Return: none
 *
 * Description: Stall both direction of the CTRL EP
 *
 *************************************************************************/
void USB_StallCtrlEP (void)
{
  EpCnfg[CTRL_ENP_IN].AvbBuff  = EpCnfg[CTRL_ENP_IN].bDoubleBuffered + 1;
  EpCnfg[CTRL_ENP_IN].Status = STALLED;
  EpCnfg[CTRL_ENP_OUT].AvbBuff = 0;
  EpCnfg[CTRL_ENP_OUT].Status = STALLED;
  USB_SetStallEP(CTRL_ENP_IN,TRUE);
  USB_SetStallEP(CTRL_ENP_OUT,TRUE);
}

/*************************************************************************
 * Function Name: USB_GetStallEP
 * Parameters: USB_Endpoint_t EP, pBoolean pStall
 *
 * Return: USB_ErrorCodes_t
 *
 * Description: Get stall state of the endpoint
 *
 *************************************************************************/
USB_ErrorCodes_t USB_GetStallEP (USB_Endpoint_t EP, pBoolean pStall)
{
  if(EpCnfg[EP].pEpCtrl == NULL)
  {
    return(USB_EP_NOT_VALID);
  }
  *pStall = (EP & 1)?
            (EpCtrlGet_STAT_TX(EpCnfg[EP].pEpCtrl) == EP_STALL):  // IN EP
            (EpCtrlGet_STAT_RX(EpCnfg[EP].pEpCtrl) == EP_STALL);  // OUT EP
  return (USB_OK);
}

/*************************************************************************
 * Function Name: USB_EP_IO
 * Parameters: USB_Endpoint_t EndPoint
 *
 * Return: none
 *
 * Description: Endpoints IO
 *
 *************************************************************************/
void USB_EP_IO(USB_Endpoint_t EP)
{
Int32U Data, Count, CountHold;
pInt32U pDst;
pInt32U pSrc;
Boolean CurrentBuffer;
pEpCnfg_t pEP = &EpCnfg[EP];

  assert(pEP->pEpCtrl);

  if(pEP->Status != BEGIN_SERVICED &&
     pEP->Status != NO_SERVICED)
  {
    return;
  }
  if(EP & 1)
  {
    // IN
    Count = pEP->Size - pEP->Offset;

    while(pEP->AvbBuff)
    {
      if(Count == 0 && !pEP->bZeroPacket)
      {
        break;
      }

      // Set Status
      pEP->Status = BEGIN_SERVICED;
      // Get data size
      if(pEP->bZeroPacketPossible && Count == pEP->MaxSize)
      {
        pEP->bZeroPacketPossible = 0;
        pEP->bZeroPacket = 1;
      }

      CountHold = Count = MIN(Count,pEP->MaxSize);
      if(Count == 0)
      {
        pEP->bZeroPacket = 0;
      }
      Int32U Offset = pEP->Offset;
      pEP->Offset += Count;
      // Write data to SIE buffer
      CurrentBuffer = TRUE;
      switch (pEP->EpType)
      {
      case UsbEpTransferControl:
      case UsbEpTransferInterrupt:
        pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(pEP->EpSlot));
        break;
      case UsbEpTransferBulk:
        if (!pEP->bDoubleBuffered)
        {
          pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(pEP->EpSlot));
        }
        else
        {
          if ((CurrentBuffer = (*pEP->pEpCtrl & (1UL << 14)) == 0))  // DTOGRX
          {
            // Tx Buffer
            pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(pEP->EpSlot));
          }
          else
          {
            // Rx Buffer
            pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(pEP->EpSlot));
          }
        }
        break;
      case UsbEpTransferIsochronous:
        if((CurrentBuffer = (*pEP->pEpCtrl & (1UL << 6)) != 0))  // DTOGTX
        {
          // Tx Buffer
          pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(pEP->EpSlot));
        }
        else
        {
          // Rx Buffer
          pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(pEP->EpSlot));
        }
        break;
      default:
        assert(0);
        return;
      }

      while (Count)
      {
        Data = *(pEP->pBuffer+Offset++);
        if (--Count)
        {
          Data |= (Int32U)(*(pEP->pBuffer+Offset++))<<8;
          --Count;
        }
        *pDst++ = Data;
      }

      // Validate buffer
      if(pEP->EpType == UsbEpTransferBulk && pEP->bDoubleBuffered)
      {
        // Toggle Filled buffer bit
        EpCtrlToggle_DTOG_RX(pEP->pEpCtrl);
      }

      if (CurrentBuffer)
      {
        WriteEpDTB_CountTx(pEP->EpSlot,CountHold);
      }
      else
      {
        WriteEpDTB_CountRx(pEP->EpSlot,CountHold);
      }

      EpCtrlSet_STAT_TX(pEP->pEpCtrl,EP_VALID);

      --EpCnfg[EP].AvbBuff;
      Count = pEP->Size - pEP->Offset;
    }

    if(!pEP->bZeroPacket && !Count)
    {
      pEP->Status = COMPLETE;
      //call callback function
      if(pEP->pFn)
      {
        ((void(*)(USB_Endpoint_t))pEP->pFn)(EP);
      }
    }
  }
  else
  {
    // OUT
    while(pEP->AvbBuff)
    {
      // Get data size and buffer begin address
      switch (pEP->EpType)
      {
      case UsbEpTransferControl:
      case UsbEpTransferInterrupt:
        // Get received bytes number
        Count = ReadEpDTB_CountRx(pEP->EpSlot) & 0x3FF;
        // Get address of the USB packet buffer for corresponding EP
        pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(pEP->EpSlot));
        break;
      case UsbEpTransferBulk:
        if (!pEP->bDoubleBuffered)
        {
          // Get received bytes number
          Count = ReadEpDTB_CountRx(pEP->EpSlot) & 0x3FF;
          // Get address of the USB packet buffer for corresponding EP
          pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(pEP->EpSlot));
        }
        else
        {
          if (!(*pEP->pEpCtrl & (1UL << 6)))  // DTOGTX
          {
            // Tx Buffer
            Count = ReadEpDTB_CountTx(pEP->EpSlot) & 0x3FF;
            pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(pEP->EpSlot));
          }
          else
          {
            // Rx Buffer
            Count = ReadEpDTB_CountRx(pEP->EpSlot) & 0x3FF;
            pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(pEP->EpSlot));
          }
        }
        break;
      case UsbEpTransferIsochronous:
        if(*pEP->pEpCtrl & (1UL <<14))  // DTOGRX
        {
          // Tx Buffer
          Count = ReadEpDTB_CountTx(pEP->EpSlot) & 0x3FF;
          pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(pEP->EpSlot));
        }
        else
        {
          // Rx Buffer
          Count = ReadEpDTB_CountRx(pEP->EpSlot) & 0x3FF;
          pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(pEP->EpSlot));
        }
        break;
      default:
        assert(0);
        return;
      }

      if(Count > (pEP->Size - pEP->Offset))
      {
        pEP->Status = BUFFER_OVERRUN;
        pEP->Size = EpCnfg[EP].Offset;
        break;
      }
      else if (Count < EpCnfg[EP].MaxSize)
      {
        pEP->Status = BUFFER_UNDERRUN;
        pEP->Size = EpCnfg[EP].Offset + Count;
      }
      else
      {
        pEP->Status = BEGIN_SERVICED;
      }

      Int32U Offset = pEP->Offset;
      pEP->Offset += Count;

      // Read data from SIE buffer
      while (Count)
      {
        Data = *pSrc++;
        *(pEP->pBuffer+Offset++) = Data;
        if (--Count)
        {
          Data >>= 8;
          *(pEP->pBuffer+Offset++) = Data;
          --Count;
        }
      }

      if(pEP->EpType == UsbEpTransferBulk && pEP->bDoubleBuffered)
      {
        EpCtrlToggle_DTOG_TX(pEP->pEpCtrl);
      }

      EpCtrlSet_STAT_RX(pEP->pEpCtrl,EP_VALID);

      --EpCnfg[EP].AvbBuff;

      if(*pEP->pEpCtrl & (1UL<<11))
      {
        EpCnfg[EP].Status = SETUP_OVERWRITE;
        return;
      }
      if (!(Count = (pEP->Size - pEP->Offset)))
      {
        pEP->Status = COMPLETE;
        break;
      }
    }

    if (pEP->Status != BEGIN_SERVICED && pEP->Status != NO_SERVICED)
    {
      //call callback function
      if(pEP->pFn)
      {
        ((void(*)(USB_Endpoint_t))pEP->pFn)(EP);
      }
    }
  }
}

/*************************************************************************
 * Function Name: USB_EpLogToPhysAdd
 * Parameters: Int8U EpLogAdd
 *
 * Return: USB_Endpoint_t
 *
 * Description: Convert the logical to physical address
 *
 *************************************************************************/
USB_Endpoint_t USB_EpLogToPhysAdd (Int8U EpLogAdd)
{
USB_Endpoint_t Address = (USB_Endpoint_t)((EpLogAdd & 0x0F)<<1);
  if(EpLogAdd & 0x80)
  {
    ++Address;
  }
  return(Address);
}

#if USB_SOF_EVENT > 0
/*************************************************************************
 * Function Name: USB_GetFrameNumb
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Return current value of SOF number
 *
 *************************************************************************/
Int32U USB_GetFrameNumb (void)
{
  return(USB_FNR_bit.FN);
}
#endif // USB_SOF_EVENT > 0

/*************************************************************************
 * Function Name: USB_StatusPhase
 * Parameters: Boolean In
 *
 * Return: none
 *
 * Description: Prepare status phase
 *
 *************************************************************************/
void USB_StatusPhase (Boolean In)
{
  if(In)
  {
    USB_IO_Data(CTRL_ENP_IN,NULL,0,NULL);
  }
}

#if USB_HIGH_PRIORITY_EVENT > 0
/*************************************************************************
 * Function Name: USB_HIGH_ISR
 * Parameters: none
 *
 * Return: none
 *
 * Description: High priority USB interrupt subroutine
 *
 *************************************************************************/
void USB_HIGH_ISR (void)
{
IntrStatus_t IntrStatus;
Int32U Status;

  USB_HIGH_INTR_ENTRY_HOOK();

  IntrStatus.Status  = USB_ISTR;
  IntrStatus.Status &= USB_CNTR | 0x1F;
  // Get masked interrupt flags
  if(IntrStatus.CTR)
  {

    USB_Endpoint_t EP;
    // find corresponding EP
    pInt32U pReg = (pInt32U)&USB_EP0R;
    pReg += IntrStatus.EP_ID;
    EP = (USB_Endpoint_t)(((*pReg & 0xF) << 1) + (IntrStatus.DIR?0:1));
    // The ISO and Double buffered endpoints have own interrupt (HIGH_INTR)
    if(EpCnfg[EP].bDoubleBuffered)
    {
      if(EP & 1)
      {
        // IN EP
        assert(EpCnfg[EP].pEpCtrl);
        EpCtrlClr_CTR_TX(EpCnfg[EP].pEpCtrl);
        Status = EpCtrlGet_DTOG_RX(EpCnfg[EP].pEpCtrl);
        EpCnfg[EP].AvbBuff = 1;
        if(  (EpCnfg[EP].EpType != UsbEpTransferIsochronous)
           &&(Status == EpCtrlGet_DTOG_TX(EpCnfg[EP].pEpCtrl)))
        {
          // The both buffers are drained
          ++EpCnfg[EP].AvbBuff;
        }
        USB_EP_IO(EP);
      }
      else
      {
        // OUT EP
        assert(EpCnfg[EP].pEpCtrl);
        EpCtrlClr_CTR_RX(EpCnfg[EP].pEpCtrl);
        Status = EpCtrlGet_DTOG_TX(EpCnfg[EP].pEpCtrl);
        EpCnfg[EP].AvbBuff = 1;
        if(  (EpCnfg[EP].EpType != UsbEpTransferIsochronous)
           &&(Status == EpCtrlGet_DTOG_RX(EpCnfg[EP].pEpCtrl)))
        {
          // The both buffers are drained
          ++EpCnfg[EP].AvbBuff;
        }
        USB_EP_IO(EP);
      }
    }
  }
  USB_HIGH_INTR_EXIT_HOOK();
}
#endif // USB_HIGH_PRIORITY_EVENT > 0

/*************************************************************************
 * Function Name: USB_ISR
 * Parameters: none
 *
 * Return: none
 *
 * Description: USB interrupt subroutine
 *
 *************************************************************************/
void USB_ISR (void)
{
IntrStatus_t IntrStatus;

  USB_INTR_ENTRY_HOOK();

  // Get masked interrupt flags
  IntrStatus.Status  = USB_ISTR;
  IntrStatus.Status &= USB_CNTR | 0x1F;

#if USB_DOVR_EVENT > 0
  // DMA over / underrun (for speed up retry process)
  if(IntrStatus.PMAOVR)
  {
    USB_ISTR = ~bmPMAOVRM;
    USB_ERR_HOOK(PMAOVR_ERROR);
  }
#endif

#if USB_ERROR_EVENT > 0
  // USB engine error interrupt
  if(IntrStatus.ERR)
  {
    USB_ISTR = ~bmERRM;
    USB_ERR_HOOK(GENERAL_ERROR);
  }
#endif

  // Device reset
  if(IntrStatus.RESET)
  {
    USB_ISTR = ~bmRESETM;
    USB_HwReset();
    UsbDevSuspendCallback(FALSE);
    UsbDevResetCallback();
  }

#if USB_SOF_EVENT > 0
  // Frame interrupt
  if(IntrStatus.SOF)
  {
    USB_ISTR = ~bmSOFM;
  #if USB_SOF_FRAME_NUMB > 0
    USB_FRAME_HOOK(USB_GetFrameNumb());
  #else
    USB_FRAME_HOOK(0);
  #endif
  }
#endif

  // Wake-up interrupt
  if(IntrStatus.WKUP)
  {
    USB_ISTR = ~(bmSUSPM | bmWKUPM);
    USB_Resume(USB_RESUME_WAKE_UP_EVENT);
  }

  // Device suspend
  if(IntrStatus.SUSP)
  {
    USB_Suspend();
    USB_ISTR = ~(bmSUSPM | bmWKUPM);
    UsbDevSuspendCallback(TRUE);
  }

  // Expected Frame interrupt
  if(IntrStatus.ESOF)
  {
    USB_ISTR = ~bmESOFM;
    USB_Resume(USB_RESUME_SOF_EVENT);
  }

  // Low priority EP interrupt
  if(IntrStatus.CTR)
  {
    USB_Endpoint_t EP;
    // find corresponding EP
    pInt32U pReg = (pInt32U)&USB_EP0R;
    pReg += IntrStatus.EP_ID;
    EP = (USB_Endpoint_t)(((*pReg & 0xF) << 1) + (IntrStatus.DIR?0:1));
    // The ISO and Double buffered endpoints have own interrupt (HIGH_INTR)
    if(!EpCnfg[EP].bDoubleBuffered)
    {
      EpCnfg[EP].AvbBuff = 1;
      if (EP & 1)
      {
        assert(EpCnfg[EP].pEpCtrl);
        EpCtrlClr_CTR_TX(EpCnfg[EP].pEpCtrl);
      }
      else
      {
        assert(EpCnfg[EP].pEpCtrl);
        EpCtrlClr_CTR_RX(EpCnfg[EP].pEpCtrl);
      }
      if(EP == CTRL_ENP_OUT)
      {
        // determinate type of packet (only for control EP)
        Boolean SetupPacket = EpCtrlGet_SETUP(EpCnfg[CTRL_ENP_OUT].pEpCtrl);
        if (SetupPacket)
        {
          EpCnfg[CTRL_ENP_IN].AvbBuff  = 1;
          // init IO to receive Setup packet
          USB_IO_Data(CTRL_ENP_IN,NULL,(Int32U)-1,NULL);
          USB_IO_Data(CTRL_ENP_OUT,UsbEp0SetupPacket.Data,sizeof(UsbSetupPacket_t),NULL);

          // reset EP IO ctrl
          if (UsbEp0SetupPacket.mRequestType.Dir == UsbDevice2Host)
          {
            USB_StatusHandler(CTRL_ENP_OUT);
          }

          USB_SetupHandler();

          if(EpCnfg[CTRL_ENP_OUT].Status == STALLED)
          {
            USB_StallCtrlEP();
          }
        }
        else
        {
          if(UsbEp0SetupPacket.mRequestType.Dir == UsbDevice2Host &&
             EpCnfg[CTRL_ENP_OUT].pFn)
          {
            ((void(*)(USB_Endpoint_t))EpCnfg[CTRL_ENP_OUT].pFn)(CTRL_ENP_OUT);
          }
          else
          {
            USB_EP_IO(EP);
          }
        }
      }
      else if (EP == CTRL_ENP_IN)
      {
        if(UsbEp0SetupPacket.mRequestType.Dir == UsbHost2Device &&
           EpCnfg[CTRL_ENP_IN].pFn)
        {
          ((void(*)(USB_Endpoint_t))EpCnfg[CTRL_ENP_IN].pFn)(CTRL_ENP_IN);
        }
        else
        {
          USB_EP_IO(EP);
        }
      }
      else
      {
        USB_EP_IO(EP);
      }
    }
  }
  USB_INTR_EXIT_HOOK();
}

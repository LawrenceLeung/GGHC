/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2007
 *
 *    File name   : usb_cnfg.h
 *    Description : USB config file
 *
 *    History :
 *    1. Date        : June 16, 2007
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: $
 **************************************************************************/

#include <includes.h>

#ifndef __USB_CNFG_H
#define __USB_CNFG_H

/* USB High Speed support*/
#define USB_HIGH_SPEED                  0

/* USB interrupt priority */
#define USB_INTR_HIGH_PRIORITY          2
#define USB_INTR_LOW_PRIORITY           3

/* USB Events */
#define USB_SOF_EVENT                   1
#define USB_ERROR_EVENT                 0   // for debug
#define USB_HIGH_PRIORITY_EVENT         1   // ISO and Double buffered bulk
#define USB_PMAOVR_EVENT                0   // for debug

/* USB Clock settings */
#define USB_DIVIDER                     RCC_USBCLKSource_PLLCLK_1Div5 // when PLL clk 72MHz

/* Device power atrb  */
#define USB_SELF_POWERED                0
#define USB_REMOTE_WAKEUP               0

/* Max Interfaces number*/
#define USB_MAX_INTERFACE               3

/* Endpoint definitions */
#define MaxIndOfRealizeEp               ENP2_IN   // be careful this is very important const
#define Ep0MaxSize                      8

#define SpkEp         	                ENP1_OUT
#define SpkEpMaxSize   	                (SampPerFrame * SubFrameSize)
#define SpkDDInd                        0

#define MicEp         	                ENP2_IN
#define MicEpMaxSize   	                (SampPerFrame * SubFrameSize * 2)
#define MicDDInd                        1

/* Class definitions*/
#define USB_INTR_AUDIO_SAMP_TIMER_PRIORITY  1

#define USB_CTRL_INTERFACE              0
#define USB_SPK_INTERFACE               1
#define USB_MIC_INTERFACE               2

#define SubFrameSize                    2
#define SampPerFrame                    48
#define SampFreq                        (SampPerFrame KHZ)


/* Other definitions */

#endif //__USB_CNFG_H

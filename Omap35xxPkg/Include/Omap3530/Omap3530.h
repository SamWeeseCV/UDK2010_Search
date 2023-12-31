/** @file

  Copyright (c) 2008-2009 Apple Inc. All rights reserved.<BR>

  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __OMAP3530_H__
#define __OMAP3530_H__

#include "Omap3530Gpio.h"
#include "Omap3530Interrupt.h"
#include "Omap3530Prcm.h"
#include "Omap3530Timer.h"
#include "Omap3530Uart.h"
#include "Omap3530Usb.h"
#include "Omap3530MMCHS.h"
#include "Omap3530I2c.h"
#include "Omap3530PadConfiguration.h"
#include "Omap3530Gpmc.h"

//CONTROL_PBIAS_LITE
#define CONTROL_PBIAS_LITE    0x48002520
#define PBIASLITEVMODE0       (0x1UL << 0)
#define PBIASLITEPWRDNZ0      (0x1UL << 1)
#define PBIASSPEEDCTRL0       (0x1UL << 2)
#define PBIASLITEVMODE1       (0x1UL << 8)
#define PBIASLITEWRDNZ1       (0x1UL << 9)

#endif // __OMAP3530_H__


/** @file

  Copyright (c) 2008-2009 Apple Inc. All rights reserved.<BR>

  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __OMAP3530UART_H__
#define __OMAP3530UART_H__

#define UART1_BASE  (0x4806A000)
#define UART2_BASE  (0x4806C000)
#define UART3_BASE  (0x49020000)

#define UART_DLL_REG  (0x0000)
#define UART_RBR_REG  (0x0000)
#define UART_THR_REG  (0x0000)
#define UART_DLH_REG  (0x0004)
#define UART_FCR_REG  (0x0008)
#define UART_LCR_REG  (0x000C)
#define UART_MCR_REG  (0x0010)
#define UART_LSR_REG  (0x0014)
#define UART_MDR1_REG (0x0020)

#define UART_FCR_TX_FIFO_CLEAR          (1UL << 3)
#define UART_FCR_RX_FIFO_CLEAR          (1UL << 3)
#define UART_FCR_FIFO_ENABLE            (1UL << 3)

#define UART_LCR_DIV_EN_ENABLE          (1UL << 7)
#define UART_LCR_DIV_EN_DISABLE         (0UL << 7)
#define UART_LCR_CHAR_LENGTH_8          (3UL << 0)

#define UART_MCR_RTS_FORCE_ACTIVE       (1UL << 1)
#define UART_MCR_DTR_FORCE_ACTIVE       (1UL << 0)

#define UART_LSR_TX_FIFO_E_MASK         (1UL << 5)
#define UART_LSR_TX_FIFO_E_NOT_EMPTY    (0UL << 5)
#define UART_LSR_TX_FIFO_E_EMPTY        (1UL << 5)
#define UART_LSR_RX_FIFO_E_MASK         (1UL << 0)
#define UART_LSR_RX_FIFO_E_NOT_EMPTY    (1UL << 0)
#define UART_LSR_RX_FIFO_E_EMPTY        (0UL << 0)

#define UART_MDR1_MODE_SELECT_DISABLE   (7UL << 0)
#define UART_MDR1_MODE_SELECT_UART_16X  (0UL << 0)

#endif // __OMAP3530UART_H__

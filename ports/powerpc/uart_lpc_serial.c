/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019, Michael Neuling, IBM Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * This is the LPC serial UART used by POWER9 boxes. This is modelled
 * in the qemu POWER9 machine.
 */

#include <unistd.h>
#include <stdbool.h>
#include "py/mpconfig.h"

#define PROC_FREQ       100000000
#define UART_BAUDS	115200
#define LPC_UART_BASE       0xc0002000

/* Taken from skiboot */
#define REG_RBR         0
#define REG_THR         0
#define REG_DLL         0
#define REG_IER         4
#define REG_DLM         4
#define REG_FCR         8
#define   REG_FCR_EN_FIFO  0x01
#define   REG_FCR_CLR_RCVR 0x02
#define   REG_FCR_CLR_XMIT 0x04
#define REG_IIR         8
#define REG_LCR         12
#define   REG_LCR_8BIT     0x03
#define REG_MCR         16
#define   REG_MCR_DTR      0x01
#define   REG_MCR_RTS      0x02
#define REG_LSR         20
#define REG_MSR         24
#define REG_SCR         28

#define LSR_DR          0x01  /* Data ready */
#define LSR_OE          0x02  /* Overrun */
#define LSR_PE          0x04  /* Parity error */
#define LSR_FE          0x08  /* Framing error */
#define LSR_BI          0x10  /* Break */
#define LSR_THRE        0x20  /* Xmit holding register empty */
#define LSR_TEMT        0x40  /* Xmitter empty */
#define LSR_ERR         0x80  /* Error */

#define LCR_DLAB        0x80  /* DLL access */

#define IER_RX          0x01
#define IER_THRE        0x02
#define IER_ALL         0x0f

static uint64_t lpc_uart_base;

static void lpc_uart_reg_write(uint64_t offset, uint8_t val) {
    uint64_t addr;

    addr = lpc_uart_base + offset;

    *(volatile uint8_t *)addr = val;
}

static uint8_t lpc_uart_reg_read(uint64_t offset) {
    uint64_t addr;
    uint8_t val;

    addr = lpc_uart_base + offset;

    val = *(volatile uint8_t *)addr;

    return val;
}

static int lpc_uart_tx_full(void) {
    return !(lpc_uart_reg_read(REG_LSR) & LSR_THRE);
}

static int lpc_uart_rx_empty(void) {
    return !(lpc_uart_reg_read(REG_LSR) & LSR_DR);
}

static unsigned long uart_divisor(unsigned long uart_freq, unsigned long bauds) {
    return uart_freq / (bauds * 16);
}

void uart_init_ppc(void) {
    unsigned long div = uart_divisor(PROC_FREQ, UART_BAUDS);

    lpc_uart_base = LPC_UART_BASE;

    lpc_uart_reg_write(REG_LCR, LCR_DLAB);
    lpc_uart_reg_write(REG_DLL, div & 0xff);
    lpc_uart_reg_write(REG_DLM, div >> 8);
    lpc_uart_reg_write(REG_LCR, REG_LCR_8BIT);
    lpc_uart_reg_write(REG_MCR, REG_MCR_DTR | REG_MCR_RTS);
    lpc_uart_reg_write(REG_FCR, REG_FCR_EN_FIFO | REG_FCR_CLR_RCVR | REG_FCR_CLR_XMIT);
}

int mp_hal_stdin_rx_chr(void) {
    while (lpc_uart_rx_empty()) {
        ;
    }
    return lpc_uart_reg_read(REG_THR);
}


void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    int i;
    for (i = 0; i < len; i++) {
        while (lpc_uart_tx_full()) {
            ;
        }
        lpc_uart_reg_write(REG_RBR, str[i]);
    }
}

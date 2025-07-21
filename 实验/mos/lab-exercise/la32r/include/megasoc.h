#ifndef MEGASOC_H
#define MEGASOC_H

/*
 * QEMU MMIO address definitions.
 */

/*
 * 16550 Serial UART device definitions.
 */
#define MEGASOC_SERIAL_BASE (0x1fe001e0)
#define MEGASOC_SERIAL_DATA (MEGASOC_SERIAL_BASE + 0x0)
#define MEGASOC_SERIAL_LSR (MEGASOC_SERIAL_BASE + 0x5)
#define MEGASOC_SERIAL_DATA_READY 0x1
#define MEGASOC_SERIAL_THR_EMPTY 0x20

/*
 * MEGASOC reboot on QEMU.
 */
#define MEGASOC_FPGA_HALT_ADD (0x1fe78030)
#define MEGASOC_FPGA_HALT_VAL (42)

#endif

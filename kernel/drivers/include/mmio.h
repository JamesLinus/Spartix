/*----------------------------------------------------------------------
 * Copyright (C) 2016 Pedro Falcato
 *
 * This file is part of Spartix, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * You can redistribute it and/or modify it under the terms of the GNU
 * General Public License version 2 as published by the Free Software
 * Foundation.
 *----------------------------------------------------------------------*/
#ifndef _DRIVERS_MMIO_UTILS_H
#define _DRIVERS_MMIO_UTILS_H

#include <stdint.h>

uint8_t mmio_readb(uint64_t address);
uint16_t mmio_readw(uint64_t address);
uint32_t mmio_readl(uint64_t address);
uint64_t mmio_readq(uint64_t address);

void mmio_writeb(uint64_t address, uint8_t val);
void mmio_writew(uint64_t address, uint16_t val);
void mmio_writel(uint64_t address, uint32_t val);
void mmio_writeq(uint64_t address, uint64_t val);



#endif
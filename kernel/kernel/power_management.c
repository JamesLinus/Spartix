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
#include <stdint.h>
#include <string.h>

#include <acpi.h>

#include <kernel/portio.h>
#include <kernel/acpi.h>

void pm_reboot()
{
	if(ACPI_FAILURE(AcpiReset()))
		printf("ACPI reset failed, trying PS/2\n");
	outb(0x64, 0xFE);
	// If the reboot hasn't happened yet, load a zero-idt and interrupt
	asm volatile("lidt 0x0");
	asm volatile("cli; int $0x60");
	asm volatile("cli;hlt");
}
void pm_shutdown()
{
	acpi_shutdown(NULL);
}
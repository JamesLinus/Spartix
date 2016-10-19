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
#ifndef _KERNEL_UDP
#define _KERNEL_UDP

#include <stdint.h>

typedef struct udp
{
	uint16_t source_port;
	uint16_t dest_port;
	uint16_t len;
	uint16_t checksum;
	uint8_t payload[0];
} udp_header_t;
inline uint16_t udpsum(udp_header_t *hdr)
{
	uint32_t sum = 0;
	uint32_t ret = 0;
	uint16_t *ptr = (uint16_t*) hdr;
	for(int i = 0; i < (hdr->len* 2); i++)
	{
		sum += ptr[i];
	}
	ret = sum & 0xFFFF;
	uint32_t carry = sum - ret;
	while(carry)
	{
		ret += carry;
		carry = (ret - (ret & 0xFFFF)) >> 16;
		ret &= 0xFFFF;
	}
	return ~ret;
}
int send_udp_packet(char *payload, size_t payload_size, int source_port, int dest_port, uint32_t srcip, uint32_t destip);


#endif
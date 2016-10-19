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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <kernel/udp.h>
#include <kernel/dhcp.h>
#include <kernel/network.h>
#include <kernel/panic.h>
#include <kernel/arp.h>

inline void parse_ipnumber_to_char_array(uint32_t ip, unsigned char* buffer)
{
	buffer[0] = ip & 0xFF;
	buffer[1] = (ip >> 8) & 0xFF;
	buffer[2] = (ip >> 16) & 0xFF;
	buffer[3] = (ip >> 24) & 0xFF;
}
inline uint32_t parse_char_array_to_ip_number(unsigned char* buffer)
{
	uint32_t ret = 0;
	ret = buffer[3];
	ret |= (buffer[2] << 8);
	ret |= (buffer[1] << 16);
	ret |= (buffer[0] << 24);
	return ret;
}
void test_netstack()
{
	int s = socket(AF_INET, SOCK_DGRAM, 0);
	if(s == -1)
		panic("Failed to create a sock for the dhcp client\n");
	char google_ip[] = {188,37,131,1};
	if(bind(s, 80, parse_char_array_to_ip_number(&google_ip), 80))
		panic("Failed to bind a socket for the dhcp client!\n");
	send(s, "ayy google", strlen("ayy google"));
}
int dhcp_sock = -1;
int dhcp_initialize()
{
	// Create and setup a dhcp packet for BOOTP
	dhcp_packet_t *dhcp_header = malloc(sizeof(dhcp_packet_t));
	if(!dhcp_header)
		return errno = ENOMEM, 1;
	memset(dhcp_header, 0, sizeof(dhcp_packet_t));
	memcpy(dhcp_header->chaddr, &mac_address, 6);
	dhcp_header->xid = 0xFEFEFEFE;
	dhcp_header->hlen = 6;
	dhcp_header->htype = HTYPE_ETHER;
	dhcp_header->op = BOOTREQUEST;
	dhcp_header->flags = 0x8000;
	
	memcpy(&dhcp_header->options, DHCP_OPTIONS_COOKIE, 4);
	
	dhcp_header->options[4] = DHO_DHCP_MESSAGE_TYPE;
	dhcp_header->options[5] = 1;
	dhcp_header->options[6] = DHCPDISCOVER;

	dhcp_header->options[7] = DHO_DHCP_PARAMETER_REQUEST_LIST;
	dhcp_header->options[8] = 3;
	dhcp_header->options[9] = 1;
	dhcp_header->options[10] = 3;
	dhcp_header->options[11] = 6;

	memset(&dhcp_header->options[12], DHO_PAD, 3);

	dhcp_header->options[15] = DHO_END;
	// Create a socket for the dhcp client
	dhcp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(dhcp_sock == -1)
		panic("Failed to create a sock for the dhcp client\n");
	// Bind a socket with the dhcp port numbers and the broadcast IP
	if(bind(dhcp_sock, 68, 0xFFFFFFFF, 67))
		panic("Failed to bind a socket for the dhcp client!\n");
	send(dhcp_sock, (const void*) dhcp_header, sizeof(dhcp_packet_t));
	free(dhcp_header);
	dhcp_packet_t *response = NULL;

	int response_len = recv(dhcp_sock, &response);
	
	/*uint32_t *convert = &response->options[19];
	convert = &response->options[25];
	*convert = LITTLE_TO_BIG32(*convert);
	*convert = LITTLE_TO_BIG32(*convert);
	convert = &response->options[31];
	*convert = LITTLE_TO_BIG32(*convert);
	*/
	uint32_t subnet_mask = 0;
	memcpy(&subnet_mask, &response->options[15], 4);
	uint32_t router_ip = 0;
	memcpy(&router_ip, &response->options[21], 4);
	uint32_t dns_server = 0;
	memcpy(&dns_server, &response->options[28], 4);
	uint32_t own_ip = response->yiaddr;
	char router_ip_b[4] = {0};
	parse_ipnumber_to_char_array(router_ip, &router_ip_b);
	printf("router_ip: %u.%u.%u.%u\n", router_ip_b[0], router_ip_b[1], router_ip_b[2], router_ip_b[3]);
	arp_request_t *returned_arp = send_arp_request_ipv4((char*)&router_ip_b);
	if(!returned_arp)
		return 1;
	char router_mac_dhcp[6] = {0};
	memcpy(&router_mac_dhcp, &returned_arp->sender_hw_address, 6);
	eth_set_router_mac(&router_mac_dhcp);
	test_netstack();
	return 0;
}


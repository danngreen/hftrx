/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Simon Goldschmidt
 *
 */
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include "hardware.h"
#include "formats.h"
#include <stdint.h>
#include <inttypes.h>

/* Prevent having to link sys_arch.c (we don't test the API layers in unit tests) */
#define NO_SYS                          1
#define MEM_ALIGNMENT                   4
#define LWIP_RAW                        1
#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     0
#define LWIP_DHCP                       0
#define LWIP_ICMP                       1
#define LWIP_UDP                        1
#define LWIP_TCP                        1
#define ETH_PAD_SIZE                    0
//#define LWIP_HAVE_SLIPIF				1
#define LWIP_IP_ACCEPT_UDP_PORT(p)      ((p) == PP_NTOHS(67))

#define MEM_SIZE                        10000
#define TCP_MSS                         (1500 /*mtu*/ - 20 /*iphdr*/ - 20 /*tcphhr*/)
#define TCP_SND_BUF                     (2 * TCP_MSS)

#define ETHARP_SUPPORT_STATIC_ENTRIES   1

#define LWIP_HTTPD_CGI                  1
#define LWIP_HTTPD_SSI                  1
#define LWIP_HTTPD_SSI_INCLUDE_TAG      0

#define LWIP_HTTPD_STRNSTR_PRIVATE		0


#define X32_F "08X"
#define S32_F "ld"
#define U32_F "lu"

#define X16_F "04X"
#define S16_F "d"
#define U16_F "u"

#define LWIP_DEBUG             1
#define LWIP_DBG_MIN_LEVEL     LWIP_DBG_LEVEL_ALL
#define LWIP_DBG_TYPES_ON      (LWIP_DBG_TRACE | LWIP_DBG_STATE | LWIP_DBG_FRESH | LWIP_DBG_HALT)
#define ETHARP_DEBUG           LWIP_DBG_ON
#define NETIF_DEBUG            LWIP_DBG_ON
//#define PBUF_DEBUG             LWIP_DBG_ON
#define API_LIB_DEBUG          LWIP_DBG_ON
#define API_MSG_DEBUG          LWIP_DBG_ON
#define SOCKETS_DEBUG          LWIP_DBG_ON
#define ICMP_DEBUG             LWIP_DBG_ON
#define IGMP_DEBUG             LWIP_DBG_ON
#define INET_DEBUG             LWIP_DBG_ON
#define IP_DEBUG               LWIP_DBG_ON
#define IP_REASS_DEBUG         LWIP_DBG_ON
#define RAW_DEBUG              LWIP_DBG_ON
#define MEM_DEBUG              LWIP_DBG_ON
#define MEMP_DEBUG             LWIP_DBG_ON
#define SYS_DEBUG              LWIP_DBG_ON
#define TIMERS_DEBUG           LWIP_DBG_ON
#define TCP_DEBUG              LWIP_DBG_ON
#define TCP_INPUT_DEBUG        LWIP_DBG_ON
#define TCP_FR_DEBUG           LWIP_DBG_ON
#define TCP_RTO_DEBUG          LWIP_DBG_ON
#define TCP_CWND_DEBUG         LWIP_DBG_ON
#define TCP_WND_DEBUG          LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG       LWIP_DBG_ON
#define TCP_RST_DEBUG          LWIP_DBG_ON
#define TCP_QLEN_DEBUG         LWIP_DBG_ON
#define UDP_DEBUG              LWIP_DBG_ON
#define TCPIP_DEBUG            LWIP_DBG_ON
#define SLIP_DEBUG             LWIP_DBG_ON
#define DHCP_DEBUG             LWIP_DBG_ON
#define AUTOIP_DEBUG           LWIP_DBG_ON
#define DNS_DEBUG              LWIP_DBG_ON
#define IP6_DEBUG              LWIP_DBG_ON
#define LWIP_PLATFORM_DIAG(mmsg) do { PRINTF mmsg; } while (0)

#endif /* __LWIPOPTS_H__ */
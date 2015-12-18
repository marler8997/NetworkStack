#ifndef _LWIPOPTS_H_
#define _LWIPOPTS_H_

#define LWIP_DEBUG 1

#define IP_DEBUG LWIP_DBG_ON
#define TCP_DEBUG LWIP_DBG_ON
#define API_LIB_DEBUG LWIP_DBG_ON
#define API_MSG_DEBUG                   LWIP_DBG_ON
#define SOCKETS_DEBUG                   LWIP_DBG_ON
#define INET_DEBUG                      LWIP_DBG_ON
#define IP_REASS_DEBUG                  LWIP_DBG_ON
#define RAW_DEBUG                       LWIP_DBG_ON
#define MEM_DEBUG                       LWIP_DBG_ON
#define MEMP_DEBUG                      LWIP_DBG_ON
#define SYS_DEBUG                       LWIP_DBG_ON
#define TIMERS_DEBUG                    LWIP_DBG_ON
#define TCP_INPUT_DEBUG                 LWIP_DBG_ON
#define TCP_FR_DEBUG                    LWIP_DBG_ON
#define TCP_RTO_DEBUG                   LWIP_DBG_ON
#define TCP_WND_DEBUG                   LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG                LWIP_DBG_ON
#define TCP_RST_DEBUG                   LWIP_DBG_ON
#define TCP_QLEN_DEBUG                  LWIP_DBG_ON
#define UDP_DEBUG LWIP_DBG_ON
#define TCPIP_DEBUG                     LWIP_DBG_ON

//#define LWIP_DBG_TYPES_ON (LWIP_DBG_TRACE | LWIP_DBG_STATE | LWIP_DBG_FRESH | LWIP_DBG_HALT)

#endif

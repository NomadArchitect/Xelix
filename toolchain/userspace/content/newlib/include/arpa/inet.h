/* Copyright © 2015 Lukas Martini
 *
 * This file is part of Xelix.
 *
 * Xelix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xelix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xelix. If not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef _ARPA_INET_H
#define _ARPA_INET_H

#include <stdint.h>
#include <netinet/in.h>
#include <byteswap.h>

#if BYTE_ORDER == BIG_ENDIAN
	#define htonl(x) (x)
	#define ntohl(x) (x)
	#define htons(x) (x)
	#define ntohs(x) (x)
#elif BYTE_ORDER == LITTLE_ENDIAN
	#define htonl(x) __bswap_32(x)
	#define ntohl(x) __bswap_32(x)
	#define htons(x) __bswap_16(x)
	#define ntohs(x) __bswap_16(x)
#endif

in_addr_t  inet_addr(const char *cp);
in_addr_t inet_lnaof(struct in_addr in);
struct in_addr inet_makeaddr(in_addr_t net, in_addr_t lna);
in_addr_t inet_netof(struct in_addr in);
in_addr_t inet_network(const char *cp);
char* inet_ntoa(struct in_addr in);

#endif /* _ARPA_INET_H */
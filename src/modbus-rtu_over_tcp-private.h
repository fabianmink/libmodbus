/*
 * Copyright © 2001-2011 Stéphane Raimbault <stephane.raimbault@gmail.com>
 * Copyright © 2017 Fabian Mink <fabian.mink@mink-ing.de>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#ifndef MODBUS_RTU_OVER_TCP_PRIVATE_H
#define MODBUS_RTU_OVER_TCP_PRIVATE_H

#define _MODBUS_RTU_HEADER_LENGTH      1
#define _MODBUS_RTU_PRESET_REQ_LENGTH  6
#define _MODBUS_RTU_PRESET_RSP_LENGTH  2

#define _MODBUS_RTU_CHECKSUM_LENGTH    2


typedef struct _modbus_rtu_over_tcp {
    /* TCP port */
    int port;
    /* IP address */
    char ip[16];
    /* To handle many slaves on the same link */
    int confirmation_to_ignore;
} modbus_rtu_over_tcp_t;

#endif /* MODBUS_RTU_OVER_TCP_PRIVATE_H */

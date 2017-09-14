/*
 * Copyright © 2001-2011 Stéphane Raimbault <stephane.raimbault@gmail.com>
 * Copyright © 2017 Fabian Mink <fabian.mink@mink-ing.de>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#ifndef MODBUS_RTU_OVER_TCP_H
#define MODBUS_RTU_OVER_TCP_H

#include "modbus.h"

MODBUS_BEGIN_DECLS

/* Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5
 * RS232 / RS485 ADU = 253 bytes + slave (1 byte) + CRC (2 bytes) = 256 bytes
 */
#define MODBUS_RTU_MAX_ADU_LENGTH  256

MODBUS_API modbus_t* modbus_new_rtu_over_tcp(const char *ip_address, int port);

MODBUS_END_DECLS

#endif /* MODBUS_RTU_OVER_TCP_H */

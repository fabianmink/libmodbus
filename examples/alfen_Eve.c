/*
  Copyright (c) 2021 Fabian Mink <fabian.mink@mink-ing.de>
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//gcc alfen_Eve.c -lmodbus

//Example for reading registers from Alfe Eve Charging station

#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <modbus/modbus.h>
#include <modbus/modbus-tcp.h>

#define REG_Voltage_L1  306
#define REG_Voltage_L2  308
#define REG_Voltage_L3  310


#define REG_Frequency  336

#define REG_Actual_Applied_Max_Current     1206
#define REG_Modbus_Slave_Max_Current       1210


static float reg2floatval(uint16_t* reg){
	uint32_t val = reg[1] | (reg[0] <<16);
	float valf = *((float*)&val);

	return(valf);
}

static int reg2val(uint16_t* reg, int length){
	int val = 0;
	int i;
	for (i = 0; i< length; i++){
		val = val*0x10000 + reg[i];
		//printf("%d\n",tab_reg[i]);
	}
	return(val);
}


int main (int argc, char **argv)
{	
	float u1,u2,u3;
	float f;

	int ret;
	int firstRun = 1;

	
	int mb_connected = 0;
	modbus_t *mb;
	
	printf("Hello, Alfen\n");

	mb = modbus_new_tcp("192.168.3.168", 502); //port acc. to manual
	//modbus_set_debug(mb, TRUE);
	
	while(1){
		
		if(!mb_connected){
			ret = modbus_connect(mb);
			printf("ret connect: %d\n",ret);
			if (ret == -1) {
				fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
			}
			else {
				mb_connected = 1;
				modbus_set_slave(mb, 1); //ID acc. to manual
				printf("Modbus connected\n");
			}
		}
		
		
		while(mb_connected){
			sleep(1);

			uint16_t tab_reg[1000];
			ret = modbus_read_registers(mb, REG_Voltage_L1, REG_Frequency-REG_Voltage_L1+2, tab_reg);
			//printf("ret read 1: %d\n",ret);
			if(ret == -1) {
				mb_connected = 0;
				modbus_close(mb);
				break;
			}

			u1       = reg2floatval(  &(  tab_reg[REG_Voltage_L1-REG_Voltage_L1])  );
			u2       = reg2floatval(  &(  tab_reg[REG_Voltage_L2-REG_Voltage_L1])  );
			u3       = reg2floatval(  &(  tab_reg[REG_Voltage_L3-REG_Voltage_L1])  );
			f        = reg2floatval(  &(  tab_reg[REG_Frequency-REG_Voltage_L1])  );

			printf("\n");

			printf("u1 = %05.1f V, ",u1);
			printf("u2 = %05.1f V, ",u2);
			printf("u3 = %05.1f V, ",u3);
			printf("f = %05.3f Hz\n ",f);

		} //while(mb_connected && mqtt_connected)
		sleep(10);
	}

	modbus_free(mb);
}





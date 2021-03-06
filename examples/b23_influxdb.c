/*
  Copyright (c) 2017 Fabian Mink <fabian.mink@mink-ing.de>
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

//gcc b23_influxdb.c -lmodbus -lcurl -o b23_influxdb
//(modbus-rtu_over_tcp Module must be present)

//Example for reading registers from ABB B23 Energy meter and store the values to influxdb via http.

#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <curl/curl.h>
#include <modbus/modbus.h>
#include <modbus/modbus-rtu_over_tcp.h>

#define REG_IMPORT_L1  0x5460
#define REG_IMPORT_L2  0x5464
#define REG_IMPORT_L3  0x5468

#define REG_VOLTAGE_L1  0x5B00
#define REG_VOLTAGE_L2  0x5B02
#define REG_VOLTAGE_L3  0x5B04

#define REG_VOLTAGE_L12  0x5B06
#define REG_VOLTAGE_L23  0x5B08
#define REG_VOLTAGE_L31  0x5B0A

#define REG_CURRENT_L1  0x5B0C
#define REG_CURRENT_L2  0x5B0E
#define REG_CURRENT_L3  0x5B10

#define REG_POWER_TOTAL  0x5B14
#define REG_POWER_L1  0x5B16
#define REG_POWER_L2  0x5B18
#define REG_POWER_L3  0x5B1A

#define REG_REACTIVEPOWER_TOTAL  0x5B1C
#define REG_REACTIVEPOWER_L1  0x5B1E
#define REG_REACTIVEPOWER_L2  0x5B20
#define REG_REACTIVEPOWER_L3  0x5B22

#define REG_APPARENTPOWER_TOTAL  0x5B24
#define REG_APPARENTPOWER_L1  0x5B26
#define REG_APPARENTPOWER_L2  0x5B28
#define REG_APPARENTPOWER_L3  0x5B2A

#define REG_FREQUENCY 0x5B2C


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
	CURL *curl;
 	CURLcode res;
	
	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();
	if(!curl) exit(-1);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8086/write?db=homales_db");

	printf("Hello, B23 Energy Meter\n");
   
 	modbus_t *mb;
	int ret;

	while(1){

	mb = modbus_new_rtu_over_tcp("192.168.39.73", 1234);
	modbus_connect(mb);
	modbus_set_slave(mb, 1);

	//printf("after connect\n");

	while(1){
		int voltage_L1, voltage_L2, voltage_L3;
		int voltage_L12, voltage_L23, voltage_L31;
		int current_L1, current_L2, current_L3;
		int power_total, power_L1, power_L2, power_L3;
		int reactive_power, apparent_power;
		int energy_L1, energy_L2, energy_L3;
		int frequency;

		uint16_t tab_reg[1000];
		//ret = modbus_read_registers(mb, REG_VOLTAGE_L1, REG_POWER_L3-REG_VOLTAGE_L1+2, tab_reg);
		ret = modbus_read_registers(mb, REG_VOLTAGE_L1, REG_FREQUENCY-REG_VOLTAGE_L1+2, tab_reg);
		//printf("after read\n");

		//printf("ret: %d\n",ret);

		if(ret == -1) break;

		
			voltage_L1 = reg2val(&(tab_reg[REG_VOLTAGE_L1-REG_VOLTAGE_L1]),2);
			voltage_L2 = reg2val(&(tab_reg[REG_VOLTAGE_L2-REG_VOLTAGE_L1]),2);
			voltage_L3 = reg2val(&(tab_reg[REG_VOLTAGE_L3-REG_VOLTAGE_L1]),2);

			voltage_L12 = reg2val(&(tab_reg[REG_VOLTAGE_L12-REG_VOLTAGE_L1]),2);
			voltage_L23 = reg2val(&(tab_reg[REG_VOLTAGE_L23-REG_VOLTAGE_L1]),2);
			voltage_L31 = reg2val(&(tab_reg[REG_VOLTAGE_L31-REG_VOLTAGE_L1]),2);

			current_L1 = reg2val(&(tab_reg[REG_CURRENT_L1-REG_VOLTAGE_L1]),2);
			current_L2 = reg2val(&(tab_reg[REG_CURRENT_L2-REG_VOLTAGE_L1]),2);
			current_L3 = reg2val(&(tab_reg[REG_CURRENT_L3-REG_VOLTAGE_L1]),2);

			power_total = reg2val(&(tab_reg[REG_POWER_TOTAL-REG_VOLTAGE_L1]),2);
			power_L1 = reg2val(&(tab_reg[REG_POWER_L1-REG_VOLTAGE_L1]),2);
			power_L2 = reg2val(&(tab_reg[REG_POWER_L2-REG_VOLTAGE_L1]),2);
			power_L3 = reg2val(&(tab_reg[REG_POWER_L3-REG_VOLTAGE_L1]),2);

			reactive_power = reg2val(&(tab_reg[REG_REACTIVEPOWER_TOTAL-REG_VOLTAGE_L1]),2);
			apparent_power = reg2val(&(tab_reg[REG_APPARENTPOWER_TOTAL-REG_VOLTAGE_L1]),2);
		
			frequency = reg2val(&(tab_reg[REG_FREQUENCY-REG_VOLTAGE_L1]),1);

			ret = modbus_read_registers(mb, REG_IMPORT_L1, REG_IMPORT_L3-REG_IMPORT_L1+4, tab_reg);

		if(ret == -1) break;

				energy_L1 = reg2val(&(tab_reg[REG_IMPORT_L1-REG_IMPORT_L1]),4);
				energy_L2 = reg2val(&(tab_reg[REG_IMPORT_L2-REG_IMPORT_L1]),4);
				energy_L3 = reg2val(&(tab_reg[REG_IMPORT_L3-REG_IMPORT_L1]),4);

				printf("U1 = %f V\n",voltage_L1*0.1);
				printf("U2 = %f V\n",voltage_L2*0.1);
				printf("U3 = %f V\n",voltage_L3*0.1);

				printf("U12 = %f V\n",voltage_L12*0.1);
				printf("U23 = %f V\n",voltage_L23*0.1);
				printf("U31 = %f V\n",voltage_L31*0.1);

				printf("I1 = %f A\n",current_L1*0.01);
				printf("I2 = %f A\n",current_L2*0.01);
				printf("I3 = %f A\n",current_L3*0.01);

				printf("Ps = %f W\n",power_total*0.01);
				printf("P1 = %f W\n",power_L1*0.01);
				printf("P2 = %f W\n",power_L2*0.01);
				printf("P3 = %f W\n",power_L3*0.01);

				printf("Q = %f var\n",reactive_power*0.01);
				printf("S = %f VA\n",apparent_power*0.01);

				printf("f = %f Hz\n",frequency*0.01);
		
				printf("E1 = %f kWh\n",energy_L1*0.01);
				printf("E2 = %f kWh\n",energy_L2*0.01);
				printf("E3 = %f kWh\n",energy_L3*0.01);
			
		
			char myline[10000];
			char mycurrent[1000];
			char myvoltage[1000];
			char mypower[1000];
			char myapparentpower[1000];
			char myfrequency[1000];
			snprintf(myline, 1000, "b23_energy_meter ");
			snprintf(mycurrent, 1000, "i1=%f,i2=%f,i3=%f,", current_L1*0.01, current_L2*0.01, current_L3*0.01);
			snprintf(myvoltage, 1000, "u1=%f,u2=%f,u3=%f,", voltage_L1*0.1, voltage_L2*0.1, voltage_L3*0.1);
			snprintf(mypower, 1000, "p1=%f,p2=%f,p3=%f,ps=%f,", power_L1*0.01, power_L2*0.01, power_L3*0.01, power_total*0.01);
			snprintf(myapparentpower, 1000, "qs=%f,ss=%f,", reactive_power*0.01, apparent_power*0.01);
			snprintf(myfrequency, 1000, "f=%f", frequency*0.01);
			
			strncat ( myline, mycurrent, 1000 );
			strncat ( myline, myvoltage, 1000 );
			strncat ( myline, mypower, 1000 );
			strncat ( myline, myapparentpower, 1000 );
			strncat ( myline, myfrequency, 1000 );

			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, myline);
			res = curl_easy_perform(curl);
		

		sleep(1);

	}

	modbus_close(mb);
	modbus_free(mb);

	}


}





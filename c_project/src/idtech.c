/*
 ============================================================================
 Name        : idtech.c
 Author      : Greg Turner
 Version     :
 Description : Looped NFC tag reading
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "IDTDef.h"
#include "libIDT_KioskIII.h"


void print_firmware_version() {
	int r=0;
	char str1[256] = {0};
	memset( str1, 0 ,256);

	fflush(stdout);
	r = device_getFirmwareVersion(str1);
	if ( r != IDG_P2_STATUS_CODE_DO_SUCCESS ) {
        char strErr[200] = {0};
        memset(strErr, 0, 200);
        device_getIDGStatusCodeString(r, strErr);
        printf(">>>>>>>>>>>>FAIL<<<<<<<<<<<<< \n   ----- Get Firmware Version Failed! ErrorCode:0x%02x, Info: %s ----- \n", r, strErr);
	}
	else{
		char str2[200] = {0};
		sprintf(str2, "Firmware version: %s\n", str1);
		printf (str2);
	}
	fflush(stdout);
}

void print_serial_number() {
	int r=0;
	char str1[256] = {0};
	memset( str1, 0 ,256);

	fflush(stdout);
	r = config_getSerialNumber(str1);
	if ( r != IDG_P2_STATUS_CODE_DO_SUCCESS ) {
        char strErr[200] = {0};
        memset(strErr, 0, 200);
        device_getIDGStatusCodeString(r, strErr);
        printf(">>>>>>>>>>>>FAIL<<<<<<<<<<<<< \n   ----- Get Serial Number Failed! ErrorCode:0x%02x, Info: %s ----- \n", r, strErr);
	}
	else{
		char str2[200] = {0};
		sprintf(str2, "Serial Number: %s\n", str1);
		printf (str2);
	}
	fflush(stdout);
}

void print_bytes(BYTE *bytes, int len) {
	for (int i = 0; i < len; i++)
	{
		if (i > 0) printf(":");
		printf("%02X", bytes[i]);
	}

}

int main(void) {
	int r = 0;
    printf("Initializing SDK...\n");
    r = device_init();
    if ( r != RETURN_CODE_DO_SUCCESS ) {
        printf(">>>>>>>>>>>>FAIL<<<<<<<<<<<<< \n   ----- Failed to Init! ErrorCode:0x%02x.\n", r);
		return 0;
    }

    device_setCurrentDevice(IDT_DEVICE_KIOSK_III);

    // preliminaries
    print_firmware_version();
    print_serial_number();

    //light up the LEDs and make 2 beeps
#ifdef __DEBUG
    BYTE data[] = {0xFF, 0x00, 0XFF, 0x01}; // no beep, all lights
#else
    BYTE data[] = {0xFF, 0x02, 0XFF, 0x01}; // 2 beeps, all lights
#endif
    device_controlUserInterface(data);
    mssleep(500);

    device_enablePassThrough(1);

    while (1) {
    	// code from idtech support
    	BYTE data[] = {0x1A, 0x00};
    	BYTE value[512] = {0};
    	int len = 512;
		device_SendDataCommandNEO(0x2C,0x02,data,2,value,&len);

		if (value[0] > 0) {
			print_bytes(value, len);
			printf("\n");
			fflush(stdout);
		};
    }

	device_close();

    return 0;
}

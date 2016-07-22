// ArduinoSerialCom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include "tserial.h"
#include "device_control.h"

Device device;

int main()
{
	device.startDevice("COM3", 9600);
	
	//printf("Retreiving device name...");

	device.device_name();
	//device.device_name();

	printf("Sending chirp...");
	device.playChirp("qpk93solit6k530de9");
	//device.playChirp("qpk93solit6k530de9");
	Sleep(5000);
	device.stopDevice();
    return 0;
}


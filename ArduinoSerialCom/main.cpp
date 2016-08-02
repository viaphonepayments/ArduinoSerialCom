// ArduinoSerialCom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include "tserial.h"
//#include "SerialClass.h"
#include "device_control.h"
#include "com_discovery.h"



Device device;


int main()
{
	
	char port[8];
	if (ComPortDiscovery::FindArduinoDevice(port)) {
		printf("Found: <%s>\n", port);
	
	}
	else {
		printf("Not found!");
		return 0;
	}	

	/*Serial* sp = new Serial(port);
	if (sp->IsConnected()) {
		printf("Connected!\n");
	}
	else {
		return 0;
	}

	char incomingData[256] = "";
	sp->WriteData("\2ua;", 4);
	Sleep(200);
	sp->ReadData(incomingData, 256);
	printf("%s\n", incomingData);


	Sleep(5000);
	*/

	if (device.startDevice(port)) {

		//printf("Retreiving device name...");

		char buf[255] = "";
		device.device_name(buf, 255);
		printf("user-agent: %s\n", buf);
		//device.device_name();
		

		//printf("Sending chirp...");
		device.playChirp("qpk93solit6k530de9");
		//device.playChirp("qpk93solit6k530de9");
		Sleep(5000);
		device.stopDevice();
	} 

    return 0;
}







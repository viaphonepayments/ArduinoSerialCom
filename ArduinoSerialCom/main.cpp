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

	

	Tserial::CPortsArray ports;
	Tserial::CNamesArray names;
	
	/*Tserial::listActiveComPorts(ports);
	
	for (int i = 0; i < ports.size(); i++) {
		_tprintf(_T("COM%u\n"), ports[i]);
		
    }
	*/
	
	Tserial::UsingSetupAPI1(ports, names);

	for (int i = 0; i < ports.size(); i++) {
		_tprintf(_T("COM%u <%s>\n"), ports[i], names[i].c_str());
	}
	
	Sleep(5000);

/*	device.startDevice("COM3", 9600);
	
	//printf("Retreiving device name...");

	device.device_name();
	//device.device_name();

	printf("Sending chirp...");
	device.playChirp("qpk93solit6k530de9");
	//device.playChirp("qpk93solit6k530de9");
	Sleep(5000);
	device.stopDevice(); */

    return 0;
}







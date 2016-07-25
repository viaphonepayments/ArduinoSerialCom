#pragma once

#include "tserial.h"

#define CHIRP_LEN 18

class Device {

private:
	
	Tserial *com;

public:

	Device() {
	}

	bool startDevice(char *port, int speed)	{
		com = new Tserial();
		if (com != 0) {
			if (com->connect(port, speed, spNONE)) {
				printf("Not Connected...\n");
			} 
			else {
				printf("Connected..\n");
			}
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	void stopDevice() {
		com->disconnect();
		// ------------------
		delete com;
		com = 0;
	}

	void send_data(unsigned char data) {
		com->sendChar(data);
		printf("%c", data);
	}

	void device_name() {
		char *cmd = "ua;";
		com->sendArray(cmd, strlen(cmd));

		char* buffer = new char[1024];
		com->getArray(buffer, 1024);

//		printf("%c", buffer);

		delete buffer;
	}

	int playChirp(const char* chirp) {
		int len = strlen(chirp);
		if (len != CHIRP_LEN) {
			return -1;
		}
		char cmd[CHIRP_LEN + 4];
		cmd[0] = '\0';
		strcat(cmd, "ch");
		strcat(cmd, chirp);
		strcat(cmd, ";");
		com->sendArray(cmd, strlen(cmd));
		return 0;
	}

};
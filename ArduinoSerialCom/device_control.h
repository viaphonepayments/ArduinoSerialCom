#pragma once

#include "tserial.h"

#define CHIRP_LEN 18

class Device {

private:
	
	Tserial *com;

public:

	Device() {
	}

	bool startDevice(char *port, int speed = 9600)	{
		com = new Tserial();
		if (com != 0) {
			if (com->connect(port, speed, spNONE)) {
			//if (com->connectWithDefualtOpts(port)) {
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


	void device_name(char* buf, int len) {
		char *cmd = "\2ua;";
		com->sendArray(cmd, strlen(cmd));
		Sleep(100);		
		com->readData(buf, len);
	}


	int playChirp(const char* chirp) {
		int len = strlen(chirp);
		if (len != CHIRP_LEN) {
			return -1;
		}
		char cmd[CHIRP_LEN + 5];
		sprintf(cmd, "\2ch%s;", chirp);
		com->sendArray(cmd, strlen(cmd));

		Sleep(200);

		char buf[255] = "";
		if (com->readData(buf, 255)) {
			printf("%s\n", buf);
		}

		memset(&buf[0], 0, sizeof(buf));
		
		Sleep(3000);
		if (com->readData(buf, 255)) {
			printf("%s\n", buf);
		}

		return 0;
	}

};
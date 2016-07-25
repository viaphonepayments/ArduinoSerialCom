#pragma once


#ifndef TSERIAL_H
#define TSERIAL_H

#include <stdio.h>
//#include <windows.h>
//#include <vector>


enum serial_parity { spNONE, spODD, spEVEN };


class Tserial {
	
protected:
	char              port[10];                      // port name "com1",...
	int               rate;                          // baudrate
	serial_parity     parityMode;
	HANDLE            serial_handle;               
													 
public:
	typedef std::vector<UINT> CPortsArray;
	typedef std::vector<std::string> CNamesArray;

	Tserial();
	
	~Tserial();

	int connect(char *port_arg, int rate_arg, serial_parity parity_arg);
	
	void sendChar(char c);
	
	void sendArray(char *buffer, int len);
	
	char getChar(void);
	
	int getArray(char *buffer, int len);
	
	int getNbrOfBytes(void);
	
	void disconnect(void);

	
	static BOOL IsNumeric(LPCSTR pszString, BOOL bIgnoreColon);

	static BOOL RegQueryValueString(ATL::CRegKey& key, LPCTSTR lpValueName, LPTSTR& pszValue);

	static BOOL QueryDeviceDescription(HDEVINFO hDevInfoSet, SP_DEVINFO_DATA& devInfo, ATL::CHeapPtr<BYTE>& byFriendlyName);

	static BOOL QueryRegistryPortName(ATL::CRegKey& deviceKey, int& nPort);

	static BOOL QueryUsingSetupAPI(const GUID& guid, WORD dwFlags, CPortsArray& ports, CNamesArray& friendlyNames);
	
	static BOOL UsingSetupAPI1(CPortsArray& ports, CNamesArray& friendlyNames);

};

#endif TSERIAL_H




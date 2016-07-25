/* ------------------------------------------------------------------------ --
--                                                                          --
--                        PC serial port connection object                  --
--                           for non-event-driven programs                  --
--                                                                          --
--                                                                          --
--                                                                          --
--  Copyright @ 2001          Thierry Schneider                             --
--                            thierry@tetraedre.com                         --
--                                                                          --
--                                                                          --
--                                                                          --
-- ------------------------------------------------------------------------ --
--                                                                          --
--  Filename : tserial.cpp                                                  --
--  Author   : Thierry Schneider                                            --
--  Created  : April 4th 2000                                               --
--  Modified : April 8th 2001                                               --
--  Plateform: Windows 95, 98, NT, 2000 (Win32)                             --
-- ------------------------------------------------------------------------ --
--                                                                          --
--  This software is given without any warranty. It can be distributed      --
--  free of charge as long as this header remains, unchanged.               --
--                                                                          --
-- ------------------------------------------------------------------------ */




/* ---------------------------------------------------------------------- */
#define STRICT

#include "stdafx.h"
#include "tserial.h"


#include <setupapi.h>
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "advapi32.lib")


Tserial::Tserial() {
	parityMode = spNONE;
	port[0] = 0;
	rate = 0;
	serial_handle = INVALID_HANDLE_VALUE;
}


Tserial::~Tserial() {
	if (serial_handle != INVALID_HANDLE_VALUE)
		CloseHandle(serial_handle);
	serial_handle = INVALID_HANDLE_VALUE;
}


void Tserial::disconnect(void) {
	if (serial_handle != INVALID_HANDLE_VALUE) {
		CloseHandle(serial_handle);
	}
	serial_handle = INVALID_HANDLE_VALUE;
}


int  Tserial::connect(char *port_arg, int rate_arg, serial_parity parity_arg) {
	int erreur;
	DCB  dcb;
	COMMTIMEOUTS cto = { 0, 0, 0, 0, 0 };

	
	if (serial_handle != INVALID_HANDLE_VALUE) {
		CloseHandle(serial_handle);
	}
	serial_handle = INVALID_HANDLE_VALUE;

	erreur = 0;

	if (port_arg != 0) {
		strncpy(port, port_arg, 10);
		rate = rate_arg;
		parityMode = parity_arg;
		memset(&dcb, 0, sizeof(dcb));		
		dcb.DCBlength = sizeof(dcb);
		dcb.BaudRate = rate;

		switch (parityMode)	{
		case spNONE:
			dcb.Parity = NOPARITY;
			dcb.fParity = 0;
			break;
		case spEVEN:
			dcb.Parity = EVENPARITY;
			dcb.fParity = 1;
			break;
		case spODD:
			dcb.Parity = ODDPARITY;
			dcb.fParity = 1;
			break;
		}

		dcb.StopBits = ONESTOPBIT;
		dcb.ByteSize = 8;

		dcb.fOutxCtsFlow = 0;
		dcb.fOutxDsrFlow = 0;
		dcb.fDtrControl = DTR_CONTROL_DISABLE;
		dcb.fDsrSensitivity = 0;
		dcb.fRtsControl = RTS_CONTROL_DISABLE;
		dcb.fOutX = 0;
		dcb.fInX = 0;

		// -- misc params ---- 
		dcb.fErrorChar = 0;
		dcb.fBinary = 1;   //binary mode is enabled, must be true
		dcb.fNull = 0;
		dcb.fAbortOnError = 0;
		dcb.wReserved = 0;
		dcb.XonLim = 2;
		dcb.XoffLim = 4;
		dcb.XonChar = 0x13;
		dcb.XoffChar = 0x19;
		dcb.EvtChar = 0;
		// -------------------

		serial_handle = CreateFile(port, GENERIC_READ | GENERIC_WRITE,
			0, NULL, OPEN_EXISTING, NULL, NULL);
		

		if (serial_handle != INVALID_HANDLE_VALUE) {
			if (!SetCommMask(serial_handle, 0)) {
				erreur = 1;
			}
		
			if (!SetCommTimeouts(serial_handle, &cto)) {
				erreur = 2;
			}
			
			if (!SetCommState(serial_handle, &dcb)) {
				erreur = 4;
			}
		}
		else {
			erreur = 8;
		}
	}
	else {
		erreur = 16;
	}

	if (erreur != 0) {
		CloseHandle(serial_handle);
		serial_handle = INVALID_HANDLE_VALUE;
	}
	return(erreur);
}



void Tserial::sendChar(char data) {
	sendArray(&data, 1);
}


void Tserial::sendArray(char *buffer, int len) {
	unsigned long result;

	if (serial_handle != INVALID_HANDLE_VALUE) {
		WriteFile(serial_handle, buffer, len, &result, NULL);
	}
}


char Tserial::getChar(void) {
	char c;
	getArray(&c, 1);
	return(c);
}


int Tserial::getArray(char *buffer, int len) {
	unsigned long read_nbr;

	read_nbr = 0;
	if (serial_handle != INVALID_HANDLE_VALUE) {
		ReadFile(serial_handle, buffer, len, &read_nbr, NULL);
	}
	return((int)read_nbr);
}


int Tserial::getNbrOfBytes(void) {
	struct _COMSTAT status;
	int             n;
	unsigned long   etat;

	n = 0;

	if (serial_handle != INVALID_HANDLE_VALUE) {
		ClearCommError(serial_handle, &etat, &status);
		n = status.cbInQue;
	}

	return(n);
}


int checkComPort(char* port) {
	DWORD dwSize = 0;
	LPCOMMCONFIG lpCC = (LPCOMMCONFIG) new BYTE[1];
	BOOL ret = GetDefaultCommConfig(port, lpCC, &dwSize);
	delete[] lpCC;

	lpCC = (LPCOMMCONFIG) new BYTE[dwSize];
	ret = GetDefaultCommConfig(port, lpCC, &dwSize);
	delete[] lpCC;
	return ret;
}



BOOL Tserial::IsNumeric(LPCSTR pszString, BOOL bIgnoreColon)
{
	size_t nLen = strlen(pszString);
	if (nLen == 0)
		return FALSE;

	//What will be the return value from this function (assume the best)
	BOOL bNumeric = TRUE;

	for (size_t i = 0; i<nLen && bNumeric; i++)
	{
		bNumeric = (isdigit(static_cast<int>(pszString[i])) != 0);
		if (bIgnoreColon && (pszString[i] == ':'))
			bNumeric = TRUE;
	}

	return bNumeric;
}



BOOL Tserial::RegQueryValueString(ATL::CRegKey& key, LPCTSTR lpValueName, LPTSTR& pszValue)
{
	//Initialize the output parameter
	pszValue = NULL;

	//First query for the size of the registry value 
	ULONG nChars = 0;
	LSTATUS nStatus = key.QueryStringValue(lpValueName, NULL, &nChars);
	if (nStatus != ERROR_SUCCESS)
	{
		SetLastError(nStatus);
		return FALSE;
	}

	//Allocate enough bytes for the return value
	DWORD dwAllocatedSize = ((nChars + 1) * sizeof(TCHAR)); //+1 is to allow us to NULL terminate the data if required
	pszValue = reinterpret_cast<LPTSTR>(LocalAlloc(LMEM_FIXED, dwAllocatedSize));
	if (pszValue == NULL)
		return FALSE;

	//We will use RegQueryValueEx directly here because ATL::CRegKey::QueryStringValue does not handle non-Null terminated data 
	DWORD dwType = 0;
	ULONG nBytes = dwAllocatedSize;
	pszValue[0] = _T('\0');
	nStatus = RegQueryValueEx(key, lpValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pszValue), &nBytes);
	if (nStatus != ERROR_SUCCESS)
	{
		LocalFree(pszValue);
		pszValue = NULL;
		SetLastError(nStatus);
		return FALSE;
	}
	if ((dwType != REG_SZ) && (dwType != REG_EXPAND_SZ))
	{
		LocalFree(pszValue);
		pszValue = NULL;
		SetLastError(ERROR_INVALID_DATA);
		return FALSE;
	}
	if ((nBytes % sizeof(TCHAR)) != 0)
	{
		LocalFree(pszValue);
		pszValue = NULL;
		SetLastError(ERROR_INVALID_DATA);
		return FALSE;
	}
	if (pszValue[(nBytes / sizeof(TCHAR)) - 1] != _T('\0'))
	{
		//Forcibly NULL terminate the data ourselves
		pszValue[(nBytes / sizeof(TCHAR))] = _T('\0');
	}

	return TRUE;
}



 BOOL Tserial::QueryRegistryPortName(ATL::CRegKey& deviceKey, int& nPort)
{
	//What will be the return value from the method (assume the worst)
	BOOL bAdded = FALSE;

	//Read in the name of the port
	LPTSTR pszPortName = NULL;
	if (RegQueryValueString(deviceKey, _T("PortName"), pszPortName))
	{
		//If it looks like "COMX" then
		//add it to the array which will be returned
		size_t nLen = _tcslen(pszPortName);
		if (nLen > 3)
		{
			if ((_tcsnicmp(pszPortName, _T("COM"), 3) == 0) && IsNumeric((pszPortName + 3), FALSE))
			{
				//Work out the port number
				nPort = _ttoi(pszPortName + 3);

				bAdded = TRUE;
			}
		}
		LocalFree(pszPortName);
	}

	return bAdded;
}


 BOOL Tserial::QueryDeviceDescription(HDEVINFO hDevInfoSet, SP_DEVINFO_DATA& devInfo, ATL::CHeapPtr<BYTE>& byFriendlyName)
 {
	 DWORD dwType = 0;
	 DWORD dwSize = 0;
	 //Query initially to get the buffer size required
	 if (!SetupDiGetDeviceRegistryProperty(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, NULL, 0, &dwSize))
	 {
		 if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			 return FALSE;
	 }

#pragma warning(suppress: 6102)
	 if (!byFriendlyName.Allocate(dwSize))
	 {
		 SetLastError(ERROR_OUTOFMEMORY);
		 return FALSE;
	 }

	 return SetupDiGetDeviceRegistryProperty(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, byFriendlyName.m_pData, dwSize, &dwSize) && (dwType == REG_SZ);
 }

BOOL Tserial::QueryUsingSetupAPI(const GUID& guid, WORD dwFlags, CPortsArray& ports, CNamesArray& friendlyNames)
{
	//Set our output parameters to sane defaults
	ports.clear();
	friendlyNames.clear();

	//Create a "device information set" for the specified GUID
	HDEVINFO hDevInfoSet = SetupDiGetClassDevs(&guid, NULL, NULL, dwFlags);
	if (hDevInfoSet == INVALID_HANDLE_VALUE)
		return FALSE;

	//Finally do the enumeration
	BOOL bMoreItems = TRUE;
	int nIndex = 0;
	SP_DEVINFO_DATA devInfo;
	while (bMoreItems)
	{
		//Enumerate the current device
		devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
		bMoreItems = SetupDiEnumDeviceInfo(hDevInfoSet, nIndex, &devInfo);
		if (bMoreItems)
		{
			//Did we find a serial port for this device
			BOOL bAdded = FALSE;

			//Get the registry key which stores the ports settings
			ATL::CRegKey deviceKey;
			deviceKey.Attach(SetupDiOpenDevRegKey(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE));
			if (deviceKey != INVALID_HANDLE_VALUE)
			{
				int nPort = 0;
				if (QueryRegistryPortName(deviceKey, nPort))
				{
					ports.push_back(nPort);
					bAdded = TRUE;
				}
			}

			//If the port was a serial port, then also try to get its friendly name
			if (bAdded)
			{
				ATL::CHeapPtr<BYTE> byFriendlyName;
				if (QueryDeviceDescription(hDevInfoSet, devInfo, byFriendlyName))
				{
					friendlyNames.push_back(reinterpret_cast<LPCTSTR>(byFriendlyName.m_pData));
				}
				else
				{
					friendlyNames.push_back(_T(""));
				}
			}
		}

		++nIndex;
	}

	//Free up the "device information set" now that we are finished with it
	SetupDiDestroyDeviceInfoList(hDevInfoSet);

	//Return the success indicator
	return TRUE;
}


BOOL Tserial::UsingSetupAPI1(CPortsArray& ports, CNamesArray& friendlyNames)
{
	//Delegate the main work of this method to the helper method
	return QueryUsingSetupAPI(GUID_DEVINTERFACE_COMPORT, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE, ports, friendlyNames);
}


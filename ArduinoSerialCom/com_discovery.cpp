
#define STRICT

#include "stdafx.h"
#include "com_discovery.h"


#include <setupapi.h>
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "advapi32.lib")

BOOL ComPortDiscovery::IsNumeric(LPCSTR pszString, BOOL bIgnoreColon)
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



BOOL ComPortDiscovery::RegQueryValueString(ATL::CRegKey& key, LPCTSTR lpValueName, LPTSTR& pszValue)
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



BOOL ComPortDiscovery::QueryRegistryPortName(ATL::CRegKey& deviceKey, int& nPort)
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


BOOL ComPortDiscovery::QueryDeviceDescription(HDEVINFO hDevInfoSet, SP_DEVINFO_DATA& devInfo, ATL::CHeapPtr<BYTE>& byFriendlyName)
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

BOOL ComPortDiscovery::QueryUsingSetupAPI(const GUID& guid, WORD dwFlags, CPortsArray& ports, CNamesArray& friendlyNames)
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


BOOL ComPortDiscovery::UsingSetupAPI1(CPortsArray& ports, CNamesArray& friendlyNames)
{
	//Delegate the main work of this method to the helper method
	return QueryUsingSetupAPI(GUID_DEVINTERFACE_COMPORT, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE, ports, friendlyNames);
}



BOOL ComPortDiscovery::FindArduinoDevice(char* result) {
	ComPortDiscovery::CPortsArray ports;
	ComPortDiscovery::CNamesArray names;
	
	ComPortDiscovery::UsingSetupAPI1(ports, names);

	std::string prefix("USB-SERIAL CH340"); 
	for (int i = 0; i < ports.size(); i++) {
		if (!names[i].compare(0, prefix.size(), prefix)) {			
			sprintf(result, "COM%u", ports[i]);
			return TRUE;
		}
		//_tprintf(_T("COM%u <%s>\n"), ports[i], names[i].c_str());
	}

	return FALSE;
}


#pragma once

class ComPortDiscovery {

public:

	typedef std::vector<UINT> CPortsArray;

	typedef std::vector<std::string> CNamesArray;

	static BOOL IsNumeric(LPCSTR pszString, BOOL bIgnoreColon);

	static BOOL RegQueryValueString(ATL::CRegKey& key, LPCTSTR lpValueName, LPTSTR& pszValue);

	static BOOL QueryDeviceDescription(HDEVINFO hDevInfoSet, SP_DEVINFO_DATA& devInfo, ATL::CHeapPtr<BYTE>& byFriendlyName);

	static BOOL QueryRegistryPortName(ATL::CRegKey& deviceKey, int& nPort);

	static BOOL QueryUsingSetupAPI(const GUID& guid, WORD dwFlags, CPortsArray& ports, CNamesArray& friendlyNames);

	static BOOL UsingSetupAPI1(CPortsArray& ports, CNamesArray& friendlyNames);

	static BOOL FindArduinoDevice(char* name);

};
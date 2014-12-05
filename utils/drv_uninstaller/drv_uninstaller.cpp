// drv_installer.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>

#ifdef UNICODE
#define PRINTF		wprintf
#define STDSTRING	std::wstring
#else
#define PRINTF		printf
#define STDSTRING	std::string
#endif

/*#ifdef WIN32
#define REGKEYSAMDESIRED	KEY_WOW64_32KEY
#else WIN64
#define REGKEYSAMDESIRED	KEY_WOW64_64KEY	// not tested
#endif*/

int _tmain(int argc, _TCHAR* argv[])
{
	TCHAR *machine_name = NULL;		// the name of target computer - local
	TCHAR *database_name = NULL;	// the database name (by default)	

	if (argc < 2) {
		PRINTF(_TEXT("Invalid arguments. Use: drv_uninstaller <DRIVER_NAME> \n"));
		return 1;
	}

	TCHAR *drv = argv[1];
	PRINTF(_TEXT("Uninstalling driver %s ...\n"), drv);

	SC_HANDLE h_cmdb = OpenSCManager(machine_name, database_name, SC_MANAGER_ALL_ACCESS);
	if (h_cmdb == NULL) {
		DWORD err = GetLastError();
		TCHAR *err_str = _TEXT("UNKNOWN");
		switch (err) {
		case ERROR_ACCESS_DENIED:
			err_str = _TEXT("ERROR_ACCESS_DENIED");
			break;
		case ERROR_DATABASE_DOES_NOT_EXIST:
			err_str = _TEXT("ERROR_DATABASE_DOES_NOT_EXIST");
			break;
		}
		PRINTF(_TEXT("OpenSCManager err: %s(%ld) \n", err_str, err));
		return 1;
	}

	bool err = false;
	SC_HANDLE h_serv = OpenService(h_cmdb,
		drv,
		SC_MANAGER_ALL_ACCESS
		);
	if (h_serv == NULL) {
		PRINTF(_TEXT("OpenService err: %ld \n", GetLastError()));		
		err = true;
	} else {
		if (!DeleteService(h_serv)) {
			PRINTF(_TEXT("DeleteService err: %ld \n", GetLastError()));			
			err = true;
		}
	}

	// Win7 x86 testing:
	// For undefined reason the registry path HKLM\SYSTEM\services with name SERVICENAME will not be deleted

	/*STDSTRING path = TEXT("SYSTEM\\services\\");
	path += SERVICENAME;
	try {
		if (RegDeleteTree(HKEY_LOCAL_MACHINE,
			path.c_str()
			) != ERROR_SUCCESS) {
				PRINTF(_TEXT("RegDeleteTree for %s err: %d \n", path.c_str(), GetLastError()));
				return 1;
		}
	} catch (...) {
				PRINTF(_TEXT("RegDeleteKeyEx for %s exception\n", path.c_str()));
	}*/

	CloseServiceHandle(h_serv);
	CloseServiceHandle(h_cmdb);

	if (!err) {
		PRINTF(TEXT("Success\n"));
	}

	return 0;
}


// drv_installer.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

#ifdef UNICODE
#define PRINTF wprintf
#else
#define PRINTF printf
#endif

int _tmain(int argc, _TCHAR* argv[])
{
	TCHAR *machine_name = NULL;		// the name of target computer - local
	TCHAR *database_name = NULL;	// the database name (by default)
	DWORD access = SC_MANAGER_ALL_ACCESS;

	if (argc < 2) {
		PRINTF(_TEXT("Invalid arguments. Use: drv_starter <DRIVER_NAME> \n"));
		return 1;
	}

	TCHAR *drv = argv[1];
	PRINTF(_TEXT("Starting driver %s ...\n"), drv);

	SC_HANDLE h_cmdb = OpenSCManager(machine_name, database_name, access);
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
		CloseServiceHandle(h_cmdb);
		return 1;
	}
	PRINTF(_TEXT("OpenService - OK\n"));

	BOOL start_res = StartService(h_serv, NULL, NULL); 
	if (!start_res) {
		PRINTF(_TEXT("StartService err: %ld \n", GetLastError()));					
		CloseServiceHandle(h_serv);	
		CloseServiceHandle(h_cmdb);
		return 1;
	}

	CloseServiceHandle(h_serv);	
	CloseServiceHandle(h_cmdb);

	PRINTF(TEXT("Success\n"));

	return 0;
}


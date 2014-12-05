// drv_installer.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>

#ifdef UNICODE
#define PRINTF wprintf
#define STDSTRING	std::wstring
#else
#define PRINTF printf
#define STDSTRING	std::string
#endif

int _tmain(int argc, _TCHAR* argv[])
{
	TCHAR *machine_name = NULL;		// the name of target computer - local
	TCHAR *database_name = NULL;	// the database name (by default)
	DWORD access = SC_MANAGER_ALL_ACCESS;

	if (argc < 2) {
			PRINTF(_TEXT("Invalid arguments. Use: drv_installer <FULL_DIRECT_sys_file_path> <Driver_NAME-optional> \n"));
			return 1;
	}

	//TCHAR *drv = argv[1];
	STDSTRING drvFullPath = argv[1];
	unsigned found = drvFullPath.find_last_of(TEXT("\\"));
	if (found == STDSTRING::npos) {
		PRINTF(TEXT("Invalid path format: %s\n"), drvFullPath.c_str());
		return 1;
	}
	STDSTRING regName = drvFullPath.substr(found+1);

	if (argc == 3) {
		regName = argv[2];
	}
	
	PRINTF(_TEXT("Installing driver %s ...\n"), regName.c_str());

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

	SC_HANDLE h_serv = CreateService( h_cmdb, 
									  regName.c_str(),
									  regName.c_str(),
									  SC_MANAGER_ALL_ACCESS,
									  SERVICE_KERNEL_DRIVER,
									  SERVICE_AUTO_START,
									  SERVICE_ERROR_CRITICAL,
									  //TEXT("d:\\Documents\\work\\drv\\test_drv5\\test_drv5\\objchk_win7_x86\\i386\\LeontovichVocordTest.sys"),
									  drvFullPath.c_str(),
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL
									  );
	if (h_serv == NULL) {
		PRINTF(_TEXT("CreateService err: %ld \n", GetLastError()));
		return 1;
	}

	CloseServiceHandle(h_serv);
	CloseServiceHandle(h_cmdb);

	PRINTF(TEXT("Success\n"));

	return 0;
}


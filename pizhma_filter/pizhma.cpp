#include "pizhma.h"
#include "FileSystemHook.h"


NTSTATUS FLTAPI PortConnectionNotifyRoutine(
      __in PFLT_PORT ClientPort,
      __in_opt PVOID ServerPortCookie,
      __in_bcount_opt(SizeOfContext) PVOID ConnectionContext,
      __in ULONG SizeOfContext,
      __deref_out_opt PVOID *ConnectionPortCookie
      )
{
	DBGPRINTEX_FOO();

	return STATUS_SUCCESS;
};

VOID FLTAPI PortDisconnectionNotifyRoutine (
      __in_opt PVOID ConnectionCookie
      )
{
	DBGPRINTEX_FOO();
};

NTSTATUS FLTAPI PortMessageNotifyRoutine (
    __in_opt PVOID PortCookie,
    __in_bcount_opt(InputBufferLength) PVOID InputBuffer,
    __in ULONG InputBufferLength,
    __out_bcount_part_opt(OutputBufferLength,*ReturnOutputBufferLength) PVOID OutputBuffer,
    __in ULONG OutputBufferLength,
    __out PULONG ReturnOutputBufferLength
    )
{
	DBGPRINTEX_FOO();

	return STATUS_SUCCESS;
};

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	DbgPrint("DriverEntry\n");
	DBGPRINTEX_INFO("%s\n", "DriverEntry");

	DriverObject->DriverUnload = UnloadRoutine; 

	/*__asm {
		int 3
	};*/

	g_fileSystemHook.DriverObject = DriverObject;	
	g_fileSystemHook.FilterPort = NULL;
	g_fileSystemHook.Filter = NULL;
	
	NTSTATUS status = FltRegisterFilter(DriverObject, &::g_FltRegistration, &g_fileSystemHook.Filter);

	if (status != STATUS_SUCCESS) {		
		switch(status) {
			case (STATUS_INSUFFICIENT_RESOURCES):
				DBGPRINTEX_ERR("FltRegisterFilter error status: %s(%ld)\n", "STATUS_INSUFFICIENT_RESOURCES", status);
				break;
			case (STATUS_INVALID_PARAMETER):
				DBGPRINTEX_ERR("FltRegisterFilter error status: %s(%ld)\n", "STATUS_INVALID_PARAMETER", status);
				break;
			case (STATUS_FLT_NOT_INITIALIZED):
				DBGPRINTEX_ERR("FltRegisterFilter error status: %s(%ld)\n", "STATUS_FLT_NOT_INITIALIZED", status);
				break;
			case (STATUS_OBJECT_NAME_NOT_FOUND):
				DBGPRINTEX_ERR("FltRegisterFilter error status: %s(%ld)\n", "STATUS_OBJECT_NAME_NOT_FOUND", status);
				break;
			default:
				DBGPRINTEX_ERR("FltRegisterFilter unspecified error: %ld\n", status);
		}
		return status;
	} else {
		DBGPRINTEX_INFO("FltRegisterFilter success");
	}

	ASSERT(g_fileSystemHook.Filter != NULL);

	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING objectName;

	RtlInitUnicodeString(&objectName, PORTNAME);	// need to deallocate ?

	InitializeObjectAttributes(&objectAttributes, &objectName, OBJ_KERNEL_HANDLE, NULL, NULL/*TODO: set to impersonation possibility*/);

	FltCreateCommunicationPort(g_fileSystemHook.Filter, &g_fileSystemHook.FilterPort, &objectAttributes, NULL /*not full if several applications may connect to us*/, 
		PortConnectionNotifyRoutine, PortDisconnectionNotifyRoutine, PortMessageNotifyRoutine, 1);

	if (status != STATUS_SUCCESS) {
		switch(status) {
			case (STATUS_FLT_DELETING_OBJECT):
				DBGPRINTEX_ERR("FltStartFiltering error status: %s(%ld)\n", "STATUS_FLT_DELETING_OBJECT", status);
				break;
			case (STATUS_INSUFFICIENT_RESOURCES):
				DBGPRINTEX_ERR("FltStartFiltering error status: %s(%ld)\n", "STATUS_INSUFFICIENT_RESOURCES", status);
				break;
			case (STATUS_OBJECT_NAME_COLLISION):
				DBGPRINTEX_ERR("FltStartFiltering error status: %s(%ld)\n", "STATUS_OBJECT_NAME_COLLISION", status);
				break;
			default:
				DBGPRINTEX_ERR("FltStartFiltering unspecified error: %ld\n", status);
		}
		//return status;
	} else {
		DBGPRINTEX_INFO("FltStartFiltering success");
	};

	if (!NT_SUCCESS(status)) {
		if (g_fileSystemHook.FilterPort != NULL) {
			FltCloseCommunicationPort(g_fileSystemHook.FilterPort);
		}
		if (g_fileSystemHook.Filter != NULL) {
			FltUnregisterFilter(g_fileSystemHook.Filter);
		}
		return status;
	}

	status = FltStartFiltering(g_fileSystemHook.Filter);
	if (status != STATUS_SUCCESS) {
		switch(status) {
			case (STATUS_INVALID_PARAMETER):
				DBGPRINTEX_ERR("FltStartFiltering error status: %s(%ld)\n", "STATUS_INVALID_PARAMETER", status);
				break;
			default:
				DBGPRINTEX_ERR("FltStartFiltering unspecified error: %ld\n", status);
		}
		return status;
	} else {
		DBGPRINTEX_INFO("FltStartFiltering success");
	};
	
	return STATUS_SUCCESS;
}

extern "C" VOID UnloadRoutine(IN PDRIVER_OBJECT DriverObject)
{	
	DBGPRINTEX_INFO("%s\n", PRGNAME);

	if (g_fileSystemHook.FilterPort != NULL) {
		FltCloseCommunicationPort(g_fileSystemHook.FilterPort);
	}

	if (g_fileSystemHook.Filter != NULL) {
		FltUnregisterFilter(g_fileSystemHook.Filter);
	}
}






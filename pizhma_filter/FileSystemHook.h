#ifndef _FILE_SYSTEM_HOOK__
#define _FILE_SYSTEM_HOOK__

#include "pizhma.h"
#include <wdm.h>
#include <ntstrsafe.h>

typedef struct _FILE_SYSTEM_HOOK
{
	PDRIVER_OBJECT	DriverObject;
	PFLT_FILTER		Filter;
	PFLT_PORT		FilterPort;

} FileSystemHook, *PFileSystemHook;


FileSystemHook g_fileSystemHook;

const FLT_CONTEXT_REGISTRATION g_ContextRegistration[] = {
	{ FLT_CONTEXT_END }
};

#define FLAG_CHECK(Flags, ToCheck)	\
	if (FlagOn(Flags, ToCheck)) DBGPRINTEX_INFO("Flag: %s", #ToCheck)

FLT_PREOP_CALLBACK_STATUS FLTAPI PreFileOperationCallback(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    )
{	
	DBGPRINTEX_FOO();

	if (FLT_IS_FS_FILTER_OPERATION(Data)) {
		DBGPRINTEX_INFO("FS_FILTER_OPERATION\n");
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}

	if (FLT_IS_FASTIO_OPERATION(Data)) {
		DBGPRINTEX_INFO("FS_IS_FASTIO_OPERATION\n");
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}

#ifdef RESEARCH_MODE
	{
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_REISSUE_MASK);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_IRP_OPERATION);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_FAST_IO_OPERATION);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_FS_FILTER_OPERATION);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_SYSTEM_BUFFER);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_GENERATED_IO);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_REISSUED_IO);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_DRAINING_IO);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_POST_OPERATION);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_NEW_SYSTEM_BUFFER);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_DIRTY);	
	}

	// Data->Thread is intresting too

	//_KTHREAD *thread = (_KTHREAD *)Data->Thread;
	//ULONG scall = thread->SystemCallNumber;

#endif

	UNICODE_STRING fileName;
	fileName.Length = 0;
	fileName.MaximumLength = NTSTRSAFE_UNICODE_STRING_MAX_CCH * sizeof(WCHAR);
	fileName.Buffer = (PWCH)ExAllocatePoolWithTag(NonPagedPool, fileName.MaximumLength, (ULONG)PIZHMA_POOL_TAG);

	if(fileName.Buffer == NULL) {
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}

	if(pFileNameInformation->Name.Length > 0) {
		RtlUnicodeStringCopy(&filePath, &pFileNameInformation->Name);
		//RtlStringCbCopyUnicodeString(pFileEvent->filePath, 1024, &pFileNameInformation->Name);
		pathNameFound = TRUE;
	}

	KIRQL irql = KeGetCurrentIrql();

	// Discovering what file is the operation consists to
	if (irql <= APC_LEVEL) {

		PFLT_FILE_NAME_INFORMATION fileNameInfo;
		NTSTATUS status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP, &fileNameInfo);

		if (status != STATUS_SUCCESS) {		
			switch(status) {
				case (STATUS_FLT_INVALID_NAME_REQUEST):
					DBGPRINTEX_ERR("FltGetFileNameInformation error status: %s(%ld)\n", "STATUS_FLT_INVALID_NAME_REQUEST", status);
					break;
				case (STATUS_INSUFFICIENT_RESOURCES):
					DBGPRINTEX_ERR("FltGetFileNameInformation error status: %s(%ld)\n", "STATUS_INSUFFICIENT_RESOURCES", status);
					break;
				case (STATUS_INVALID_PARAMETER):
					DBGPRINTEX_ERR("FltGetFileNameInformation error status: %s(%ld)\n", "STATUS_INVALID_PARAMETER", status);
					break;
				case (STATUS_FLT_NAME_CACHE_MISS):
					DBGPRINTEX_ERR("FltGetFileNameInformation error status: %s(%ld)\n", "STATUS_FLT_NAME_CACHE_MISS", status);
					break;
				case (STATUS_ACCESS_DENIED):
					DBGPRINTEX_ERR("FltGetFileNameInformation error status: %s(%ld)\n", "STATUS_ACCESS_DENIED", status);
					break;
				default:
					DBGPRINTEX_ERR("FltGetFileNameInformation unspecified error: %ld\n", status);
			}
			return FLT_PREOP_SUCCESS_NO_CALLBACK;
		}

		DBGPRINTEX_INFO("File: %s %s %s\n", fileNameInfo->Name, fileNameInfo->Volume, fileNameInfo->Extension);
	} else {
		DBGPRINTEX_WARN("irql level is unpropriate: %d\n", irql);
	}

	// Discovering which process context we are executed under
	if (irql <= DISPATCH_LEVEL) {

		ULONG pid = FltGetRequestorProcessId(Data);
		DBGPRINTEX_INFO("PID: %d\n", pid);

	} else {
		DBGPRINTEX_WARN("irql level is unpropriate: %d\n", irql);
	}
	
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
};


FLT_POSTOP_CALLBACK_STATUS FLTAPI PostFileOperationCallback (
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in_opt PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags
    )
{
	
	DBGPRINTEX_FOO();

#ifdef RESEARCH_MODE
	{
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_REISSUE_MASK);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_IRP_OPERATION);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_FAST_IO_OPERATION);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_FS_FILTER_OPERATION);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_SYSTEM_BUFFER);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_GENERATED_IO);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_REISSUED_IO);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_DRAINING_IO);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_POST_OPERATION);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_NEW_SYSTEM_BUFFER);
	FLAG_CHECK(Data->Flags, FLTFL_CALLBACK_DATA_DIRTY);	
	}

	// Data->Thread is intresting too

#endif

	return FLT_POSTOP_FINISHED_PROCESSING;	
};

NTSTATUS FLTAPI FilterUnloadCallback (
    FLT_FILTER_UNLOAD_FLAGS Flags
    )
{	
	DBGPRINTEX_FOO();

	return STATUS_SUCCESS;
};

const FLT_OPERATION_REGISTRATION g_OperationRegistration[] = {
	
	{ IRP_MJ_CREATE,
	  // flags
	  // FLTFL_OPERATION_REGISTRATION_SKIP_PAGING_IO | FLTFL_OPERATION_REGISTRATION_SKIP_CACHED_IO | FLTFL_OPERATION_REGISTRATION_SKIP_NON_DASD_IO\	   
	  0,
	  //
	  PreFileOperationCallback,
	  PostFileOperationCallback },	

		///////.... 

	{ IRP_MJ_OPERATION_END	}
		
};

NTSTATUS FLTAPI InstanceSetupCallback (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_SETUP_FLAGS Flags,
    __in DEVICE_TYPE VolumeDeviceType,
    __in FLT_FILESYSTEM_TYPE VolumeFilesystemType
    )
{
	DBGPRINTEX_FOO();

	switch (VolumeFilesystemType) {
		case (FLT_FSTYPE_UNKNOWN): 
			DBGPRINTEX_INFO("VolumeFilesystemType: FLT_FSTYPE_UNKNOWN\n");
			break;
		case (FLT_FSTYPE_RAW): 
			DBGPRINTEX_INFO("VolumeFilesystemType: FLT_FSTYPE_RAW\n");
			break;
		case (FLT_FSTYPE_NTFS): 
			DBGPRINTEX_INFO("VolumeFilesystemType: FLT_FSTYPE_NTFS\n");
			break;
		default:
			DBGPRINTEX_INFO("VolumeFilesystemType: %d\n", VolumeFilesystemType);			
	};

	switch (VolumeDeviceType) {
		case (FILE_DEVICE_CD_ROM):
			DBGPRINTEX_INFO("VolumeDeviceType: FILE_DEVICE_CD_ROM");
			break;
		case (FILE_DEVICE_CD_ROM_FILE_SYSTEM):
			DBGPRINTEX_INFO("VolumeDeviceType: FILE_DEVICE_CD_ROM_FILE_SYSTEM");
			break;
		case (FILE_DEVICE_DISK):
			DBGPRINTEX_INFO("VolumeDeviceType: FILE_DEVICE_DISK");
			break;
		case (FILE_DEVICE_NETWORK_FILE_SYSTEM):
			DBGPRINTEX_INFO("VolumeDeviceType: FILE_DEVICE_NETWORK_FILE_SYSTEM");
			break;
		default:
			DBGPRINTEX_INFO("VolumeDeviceType: %ld\n", VolumeDeviceType);
	}

#ifdef RESEARCH_MODE
	{

	if (Flags & FLTFL_INSTANCE_SETUP_AUTOMATIC_ATTACHMENT) {
		DBGPRINTEX_INFO("Flags: FLTFL_INSTANCE_SETUP_AUTOMATIC_ATTACHMENT");
	}

	if (Flags & FLTFL_INSTANCE_SETUP_MANUAL_ATTACHMENT) {
		DBGPRINTEX_INFO("Flags: FLTFL_INSTANCE_SETUP_MANUAL_ATTACHMENT");
	}

	if (Flags & FLTFL_INSTANCE_SETUP_NEWLY_MOUNTED_VOLUME) {
		DBGPRINTEX_INFO("Flags: FLTFL_INSTANCE_SETUP_NEWLY_MOUNTED_VOLUME");
	}

	if (Flags & FLTFL_INSTANCE_SETUP_DETACHED_VOLUME) {
		DBGPRINTEX_INFO("Flags: FLTFL_INSTANCE_SETUP_DETACHED_VOLUME");
	}

	}
#endif
	
	return STATUS_SUCCESS;
};

NTSTATUS FLTAPI InstanceQueryTeardownCallback (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
    )
{	
	DBGPRINTEX_FOO();
	return STATUS_SUCCESS;
};

const ::FLT_REGISTRATION g_FltRegistration = {
	sizeof(::FLT_REGISTRATION),
	FLT_REGISTRATION_VERSION,
	// flags
	// FLTFL_REGISTRATION_DO_NOT_SUPPORT_SERVICE_STOP
	0,
	//
	g_ContextRegistration,
	g_OperationRegistration,
	FilterUnloadCallback,
	InstanceSetupCallback,
	NULL, //InstanceQueryTeardownCallback,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

#endif
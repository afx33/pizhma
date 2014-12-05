#ifndef _PIZHMA_H__
#define _PIZHMA_H__

#include <ntifs.h>
#include <wdm.h>
#include "fltKernel.h"

// Determine Platform
#define _X86
#define _X86_

// Specified Checked or Free solution
#define PIZHMA_CHECKED	DBG


// Tha common routines

DRIVER_UNLOAD Unload;

extern "C" VOID UnloadRoutine(IN PDRIVER_OBJECT DriverObject);
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);

//#ifdef ALLOC_PRAGMA

// Specify that DriverEntry will be used one time 
#pragma alloc_text(INIT, DriverEntry)

// Specify that UnloadRoutine will locate in paged memory
#pragma alloc_text(PAGE, UnloadRoutine)

//#endif

// Debug helping routines

#define PRGNAME					"pizhma"
#define PRGLOGLBL				PRGNAME		// The label for debug viewing - may be whatever

// Determine research execution mode - the external tracing for research purposes 
#define RESEARCH_MODE

#define PIZHMA_POOL_TAG	'ppt'

// Level must set bits from 0 to 31
// MSDN:DbgPrintEx routine
// http://msdn.microsoft.com/en-us/library/windows/hardware/ff543634(v=vs.85).aspx
#define DBGPRINT(x) DbgPrint(x)

#define DBGLEVERR	0x0000000F
#define DBGLEVWARN	0x000000FF
#define DBGLEVINFO	0x00000FFF

#ifdef PIZHMA_CHECKED

#define DBGPRINTEX_ERR(fmt, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DBGLEVERR, "%s\t%s:%d\tERR:" fmt "\n", PRGLOGLBL, __FILE__, __LINE__, __VA_ARGS__)
#define DBGPRINTEX_WARN(fmt, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DBGLEVWARN, "%s\t%s:%d\tWARN:" fmt "\n", PRGLOGLBL, __FILE__, __LINE__, __VA_ARGS__)
#define DBGPRINTEX_INFO(fmt, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DBGLEVINFO, "%s\t%s:%d\tINFO:" fmt "\n", PRGLOGLBL, __FILE__, __LINE__, __VA_ARGS__)
#define DBGPRINTEX_FOO()	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DBGLEVINFO, "%s\t%s: %s\n", PRGLOGLBL, __FILE__, __FUNCTION__)
#define DBGROUTERR(routine, ret_status_num) DBGPRINTEX_ERR("%s returned %x", __FILE__, __LINE__, routine, ret_status_num)

#else

#define DBGPRINTEX_ERR(fmt, ...)
#define DBGPRINTEX_WARN(fmt, ...)
#define DBGPRINTEX_INFO(fmt, ...)
#define DBGROUTERR(routine, ret_status_num)

#endif

#define PORTNAME				L"\\PizhmaPort"

#endif



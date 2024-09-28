#include "../include/hide_imports.h"

void ThreadExample()
{
    CALL("DbgPrint", 1, "Hi From Thread\n");

	PETHREAD currentThread = (PETHREAD)CALL("PsGetCurrentThread", 0);
	CALL("DbgPrint", 2, "Current Thread Address: %p\n", currentThread);
}

NTSTATUS GsDriverEntry(PDRIVER_OBJECT obj, PUNICODE_STRING path, ULONG64 MmGetSystemRoutineAddress)
{
	UNREFERENCED_PARAMETER(obj);
	UNREFERENCED_PARAMETER(path);
	UNREFERENCED_PARAMETER(MmGetSystemRoutineAddress);

#if MANUAL_MODE
	// from driver mapping params (example)
	MmGetSystemRoutineAddressAddr = MmGetSystemRoutineAddress;
#endif

	CALL("DbgPrint", 1, "DbgPrint Test\n");

	CALL("DbgPrintEx", 3, DPFLTR_IHVDRIVER_ID, 0, "DbgPrintEx test\n");

	HANDLE threadHandle;
	CALL("PsCreateSystemThread",
		7,
		&threadHandle,
		THREAD_ALL_ACCESS,
		NULL,
		NULL,
		NULL,
		ThreadExample,
		NULL);
	
	return STATUS_SUCCESS;
}

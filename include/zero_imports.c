#include "hide_imports.h"

typedef struct _KLDR_DATA_TABLE_ENTRY
{
	struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
	VOID* ExceptionTable;                                                   //0x10
	ULONG ExceptionTableSize;                                               //0x18
	VOID* GpValue;                                                          //0x20
	struct _NON_PAGED_DEBUG_INFO* NonPagedDebugInfo;                        //0x28
	VOID* DllBase;                                                          //0x30
	VOID* EntryPoint;                                                       //0x38
	ULONG SizeOfImage;                                                      //0x40
	struct _UNICODE_STRING FullDllName;                                     //0x48
	struct _UNICODE_STRING BaseDllName;                                     //0x58
} _KLDR_DATA_TABLE_ENTRY, * PKLDR_DATA_TABLE_ENTRY;

typedef struct _IMPORT 
{
	LPCWSTR name;
	uintptr_t address;
} IMPORT, * PIMPORT;

static IMPORT imports_list[MAX_IMPORTS];
static uintptr_t kernel_base;

typedef uintptr_t(*fn_t)(uintptr_t a1, ...);

size_t _wcslen(const wchar_t* s) 
{
	const wchar_t* ptr = s;
	while (*ptr) {
		ptr++;
	}
	return ptr - s;
}

inline static uintptr_t get_kernel_export(const PUNICODE_STRING name)
{
#if MANUAL_MODE
	if (!MmGetSystemRoutineAddressAddr) 
	{
#if DEBUG
		LOG("Can't find the MmGetSystemRoutineAddressAddr!\n");
#endif
		return 0;
	}
#if DEBUG
	LOG("MmGetSystemRoutineAddress: %p\n", (PVOID)MmGetSystemRoutineAddressAddr);
#endif
	return (uintptr_t)((PVOID(*)(PUNICODE_STRING))MmGetSystemRoutineAddressAddr)(name);
#else
	if (!MmGetSystemRoutineAddress)
	{
#if DEBUG
		LOG("Can't find the MmGetSystemRoutineAddress!\n");
#endif
		return 0;
	}
#if DEBUG
	LOG("MmGetSystemRoutineAddress: %p\n", (PVOID)MmGetSystemRoutineAddress);
#endif
	return (uintptr_t)MmGetSystemRoutineAddress(name);
#endif
}

static bool get_kernel_base()
{
	if (kernel_base)
	{
		return true;
	}

	LPCWSTR name = L"PsLoadedModuleList";

	UNICODE_STRING funcName;
	funcName.Buffer = (PWSTR)name;
	funcName.Length = (USHORT)(_wcslen(name) * sizeof(WCHAR));
	funcName.MaximumLength = funcName.Length + sizeof(WCHAR);

	PLIST_ENTRY PsLoadedModuleList;
	PsLoadedModuleList = (PLIST_ENTRY)get_kernel_export(&funcName);
	if (!PsLoadedModuleList)
	{
#if DEBUG
		LOG("Can't find the PsLoadedModuleList!\n");
#endif
		return false;
	}
#if DEBUG
	LOG("PsLoadedModuleList: %p\n", PsLoadedModuleList);
#endif

	/* First entry is always ntoskrnl.exe :) */
	PKLDR_DATA_TABLE_ENTRY ldr_entry = (PKLDR_DATA_TABLE_ENTRY)PsLoadedModuleList->Flink;

#if DEBUG
	LOG("ldr_entry->BaseDllName: %wZ\n", &ldr_entry->BaseDllName);
	LOG("ldr_entry->DllBase: %p\n", &ldr_entry->DllBase);
#endif

	kernel_base = (uintptr_t)&ldr_entry->DllBase;

	return true;
}


static uintptr_t get_function_address(const LPCWSTR name)
{
	if (!get_kernel_base())
	{
		return 0;
	}

	for (int i = 0; i < MAX_IMPORTS; i++)
	{
		if (imports_list[i].name == name)
		{
#if DEBUG
			LOG("Function: %ls Address: %p [previously imported]\n", name, (PVOID)imports_list[i].address);
#endif
			return imports_list[i].address;
		}
	}

	UNICODE_STRING funcName;
	funcName.Buffer = (PWSTR)name;
	funcName.Length = (USHORT)(_wcslen(name) * sizeof(WCHAR));
	funcName.MaximumLength = funcName.Length + sizeof(WCHAR);

	uintptr_t funcAddress = 0;
	funcAddress = get_kernel_export(&funcName);

	IMPORT import;
	import.name = name;
	import.address = funcAddress;

	for (int i = 0; i < MAX_IMPORTS; i++) 
	{
		if (imports_list[i].address == 0) 
		{
			imports_list[i] = import;
		}
	}
#if DEBUG
	LOG("Function: %ls Address: %p [imported successfully]\n", name, (PVOID)funcAddress);
#endif
	return funcAddress;
}

uintptr_t __fastcall call(const LPCWSTR name, const int args_count, ...)
{
	uintptr_t address = (uintptr_t)get_function_address(name);
	if (address == 0)
	{
		return 0;
	}

	fn_t func = (fn_t)address;

	va_list args_list;
	va_start(args_list, args_count);

	uintptr_t a[MAX_ARGS] = { 0 };

	// Populate the argument array
	for (int i = 0; i < args_count; i++)
	{
		a[i] = va_arg(args_list, uintptr_t);
	}

	va_end(args_list);

	uintptr_t result = 0;
	switch (args_count)
	{
	case 0:
		result = func(0);
		break;
#if MAX_ARGS >= 1
	case 1:
		result = func(a[0]);
		break;
#endif
#if MAX_ARGS >= 2
	case 2:
		result = func(a[0], a[1]);
		break;
#endif
#if MAX_ARGS >= 3
	case 3:
		result = func(a[0], a[1], a[2]);
		break;
#endif
#if MAX_ARGS >= 4
	case 4:
		result = func(a[0], a[1], a[2], a[3]);
		break;
#endif
#if MAX_ARGS >= 5
	case 5:
		result = func(a[0], a[1], a[2], a[3], a[4]);
		break;
#endif
#if MAX_ARGS >= 6
	case 6:
		result = func(a[0], a[1], a[2], a[3], a[4], a[5]);
		break;
#endif
#if MAX_ARGS >= 7
	case 7:
		result = func(a[0], a[1], a[2], a[3], a[4], a[5], a[6]);
		break;
#endif
#if MAX_ARGS >= 8
	case 8:
		result = func(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
		break;
#endif
#if MAX_ARGS >= 9
	case 9:
		result = func(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]);
		break;
#endif
#if MAX_ARGS >= 10
	case 10:
		result = func(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9]);
		break;
#endif
#if MAX_ARGS >= 11
	case 11:
		result = func(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10]);
		break;
#endif
#if MAX_ARGS >= 12
	case 12:
		result = func(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11]);
		break;
#endif
#if MAX_ARGS >= 13
	case 13:
		result = func(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12]);
		break;
#endif
#if MAX_ARGS >= 14
	case 14:
		result = func(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12], a[13]);
		break;
#endif
#if MAX_ARGS >= 15
	case 15:
		result = func(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12], a[13], a[14]);
		break;
#endif
	default:
		return 0;
	}

	return result;
}
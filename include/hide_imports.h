#ifndef HIDE_IMPORTS
#define HIDE_IMPORTS

#ifndef _NTIFS_
#include <ntifs.h>
#endif

#ifndef _INC_STDARG
#include <stdarg.h>
#endif

#define MAX_IMPORTS 50
#define MAX_ARGS	10
#define DEBUG		0
#define MANUAL_MODE 0

#define bool  int
#define true  1
#define false 0

#ifndef _UINTPTR_T_DEFINED
#ifdef _WIN64
typedef unsigned __int64  uintptr_t;
#else
typedef unsigned int uintptr_t;
#endif
#endif

#if MANUAL_MODE
uintptr_t MmGetSystemRoutineAddressAddr;
#endif

#define WIDETEXT(x) L##x

#define LOG(str, ...)		   DbgPrint(str, __VA_ARGS__)
#define CALL(name, args, ...)  call(WIDETEXT(name), args, __VA_ARGS__)

/* Function that calls km functions */
uintptr_t __fastcall call(const LPCWSTR name, const int args_count, ...);

//       |\      _,,,---,,_
// Zzz   /,`.-'`'    -.  ;-;;,_
//      |,4-  ) )-,_. ,\ (  `'-'
//     '---''(_/--'  `-\_)

#endif

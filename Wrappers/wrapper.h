#pragma once

namespace Wrapper
{
	bool ValidProcAddress(FARPROC ProcAddress);
	void ShimProc(volatile FARPROC &var, FARPROC in, volatile FARPROC &out);
	const char *GetWrapperName(const char *WrapperMode);
	bool CheckWrapperName(const char *WrapperMode);
	HMODULE CreateWrapper(const char *ProxyDll, const char *WrapperMode, const char *MyDllName);
}

struct DXWAPPERSETTINGS
{
	bool Dd7to9 = false;
	bool Dinputto8 = false;
};

typedef void(WINAPI* DxWrapperSettingsProc)(DXWAPPERSETTINGS* DxSettings);
typedef void(WINAPI* DxWrapperLoggingProc)(const char* LogMessage);

// Shared procs
#include "shared.h"

#define VISIT_DLLS_BASIC(visit) \
	visit(d3d9) \
	visit(ddraw) \
	visit(dinput) \
	visit(dinput8)

#define VISIT_DLLS(visit) \
	VISIT_DLLS_BASIC(visit)

// Wrappers
#include "d3d9.h"
#include "ddraw.h"
#include "dinput.h"
#include "dinput8.h"

#define EXPORT_OUT_WRAPPED_PROC(procName, unused) \
	extern volatile FARPROC procName ## _out;

#define INITIALIZE_OUT_WRAPPED_PROC(procName, unused) \
	volatile FARPROC procName ## _out = nullptr;

#define DEFINE_STATIC_PROC_ADDRESS(VarType, VarName, VarAddress) \
	static VarType VarName = (VarType)0xFFFFFFFF; \
	if ((DWORD)VarName == 0xFFFFFFFF) \
	{ \
		VarName = Wrapper::ValidProcAddress(VarAddress) ? reinterpret_cast<VarType>(VarAddress) : nullptr; \
	}

#define DECLARE_PROC_VARIABLES(procName, unused) \
	volatile extern FARPROC procName ## _funct; \
	volatile extern FARPROC procName ## _var;

#define	DECLARE_PROC_VARIABLES_SHARED(procName, procName_shared, unused) \
	volatile extern FARPROC procName ## _funct; \
	volatile extern FARPROC procName ## _var;

namespace ddraw
{
	VISIT_PROCS_DDRAW(DECLARE_PROC_VARIABLES);
	VISIT_SHARED_DDRAW_PROCS(DECLARE_PROC_VARIABLES_SHARED);
	HMODULE Load(const char *ProxyDll, const char *MyDllName);
}
namespace dinput
{
	VISIT_PROCS_DINPUT(DECLARE_PROC_VARIABLES);
	VISIT_SHARED_DINPUT_PROCS(DECLARE_PROC_VARIABLES_SHARED);
	HMODULE Load(const char *ProxyDll, const char *MyDllName);
}
namespace dinput8
{
	VISIT_PROCS_DINPUT8(DECLARE_PROC_VARIABLES);
	VISIT_SHARED_DINPUT8_PROCS(DECLARE_PROC_VARIABLES_SHARED);
	HMODULE Load(const char *ProxyDll, const char *MyDllName);
}
namespace d3d9
{
	VISIT_PROCS_D3D9(DECLARE_PROC_VARIABLES);
	HMODULE Load(const char *ProxyDll, const char *MyDllName);
}
namespace ShardProcs
{
	VISIT_PROCS_SHAREDPROCS(DECLARE_PROC_VARIABLES);
	void Load(HMODULE dll);
}

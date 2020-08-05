#pragma once
#include <Windows.h>
#include "Offsets.h"
#include "ESP.h"
#include "Maths.h"
class Memory
{

public:

	Memory(const wchar_t*);

	template <typename T>
	static T RPM(size_t address)
	{
		T result;
		ReadProcessMemory(handle, (LPCVOID)(address), &result, sizeof(T),NULL);
		return result;
	}

	static Vec3 GetBonePosition(DWORD EntityPtr, int BoneIndex);
    DWORD GetModuleAdress(size_t PID, std::wstring ModuleName);
	~Memory();

private:
	/// <summary>
	/// private members
	/// </summary>
	static HANDLE handle;
	DWORD moduleAddress;
};


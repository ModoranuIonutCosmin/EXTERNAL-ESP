#include "pch.h"
#include "Memory.h"
#include <Windows.h>
#include <TlHelp32.h>
#include "Memory.h"

HANDLE Memory::handle;

Memory::Memory(const wchar_t* processName)
{
    DWORD PID;
    HWND _targetWindow = FindWindow(NULL, processName);
    GetWindowThreadProcessId(_targetWindow, &PID);
    this->handle = OpenProcess(PROCESS_VM_READ, NULL, PID);
    if (handle == INVALID_HANDLE_VALUE)
        throw(std::runtime_error("Process was not found!"));
    this->moduleAddress = GetModuleAdress(PID, L"client.dll");

}
Vec3 Memory::GetBonePosition(DWORD EntityPtr, int BoneIndex)
{
    DWORD BoneMatrixPtr = RPM<DWORD>(EntityPtr + m_dwBoneMatrix);
    Matrix3x4F BoneMatrix = RPM<Matrix3x4F>(BoneMatrixPtr + 0x30 * BoneIndex);
    return Vec3(BoneMatrix.m[0][3], BoneMatrix.m[1][3], BoneMatrix.m[2][3]);
}
DWORD Memory::GetModuleAdress(size_t PID, std::wstring ModuleName)
{
    DWORD_PTR baseAddy = 0;
    HANDLE moduleAccess = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32 | TH32CS_SNAPMODULE, PID);
    if (moduleAccess != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modInfo;
        bool succeded = 1;
        modInfo.dwSize = sizeof(MODULEENTRY32);
        succeded = Module32FirstW(moduleAccess, &modInfo);
        if (succeded)
        {
            if (std::wstring(modInfo.szModule) == ModuleName)
                return reinterpret_cast<DWORD_PTR>(modInfo.modBaseAddr);
            while (Module32Next(moduleAccess, &modInfo))
            {
                if (std::wstring(modInfo.szModule) == ModuleName)
                    return reinterpret_cast<DWORD>(modInfo.modBaseAddr);
            }
        }
    }
    return baseAddy;
}

Memory::~Memory()
{
    CloseHandle(this->handle);
}

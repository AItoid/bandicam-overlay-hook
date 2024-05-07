#include "Includes.h"

bool ApplyHooks()
{
	std::uintptr_t BandicamModule = (std::uintptr_t)GetModuleHandleA("bdcam64.dll");
	if (!BandicamModule)
	{
		BandicamModule = (std::uintptr_t)GetModuleHandleA("bdcam32.dll");
	}

	if (!BandicamModule)
		return false;

#ifdef _WIN64
	std::uintptr_t* BandicamPresent = (std::uintptr_t*)Memory::FindPattern("bdcam64.dll", "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC ? 41 8B F0");
	std::uintptr_t* BandicamResizeBuffers = (std::uintptr_t*)Memory::FindPattern("bdcam64.dll", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 41 8B F9");
#else
	std::uintptr_t* BandicamPresent = (std::uintptr_t*)Memory::FindPattern("bdcam64.dll", "55 8B EC 6A ? 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC ? 56 57 A1 ? ? ? ? 33 C5 50 8D 45 ? 64 A3 ? ? ? ? A1");
	std::uintptr_t* BandicamResizeBuffers = (std::uintptr_t*)Memory::FindPattern("bdcam64.dll", "55 8B EC 6A ? 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC ? 56 A1 ? ? ? ? 33 C5 50 8D 45 ? 64 A3 ? ? ? ? 8B 0D");
#endif

	// Bad function signatures.
	if (!BandicamPresent || !BandicamResizeBuffers)
		return false;

	// Store originals.
	Hooks::oPresent = (Hooks::Present)BandicamPresent;
	Hooks::oResizeBuffers = (Hooks::ResizeBuffers)BandicamResizeBuffers;

	// Setup VEH.
	veh::Setup();

	// Hook.
	veh::Hook(BandicamPresent, Hooks::PresentHk);
	veh::Hook(BandicamResizeBuffers, Hooks::ResizeBuffersHk);

	return true;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD AttachReason, LPVOID Reserved)
{
	if (AttachReason == DLL_PROCESS_ATTACH)
	{
		if (!ApplyHooks())
		{
			MessageBoxA(nullptr, "Failed while setting up hook.", "Error", MB_OK | MB_ICONERROR);
			return FALSE;
		}
	}

	return TRUE;
}
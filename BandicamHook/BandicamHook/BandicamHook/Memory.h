#pragma once
#include "Includes.h"

namespace Memory
{
	inline std::uint8_t* FindPattern(const char* Module, const char* Signature)
	{
		const auto ModuleBase = GetModuleHandleA(Module);

		if (!ModuleBase)
			return nullptr;

		static auto PatternToByte = [](const char* pattern)
		{
			auto bytes = std::vector<int>{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + std::strlen(pattern);

			for (auto current = start; current < end; ++current)
			{
				if (*current == '?')
				{
					++current;

					if (*current == '?')
						++current;

					bytes.push_back(-1);
				}
				else
				{
					bytes.push_back(std::strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

		auto Dos = reinterpret_cast<PIMAGE_DOS_HEADER>(ModuleBase);
		auto Nt = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(ModuleBase) + Dos->e_lfanew);

		auto ImageSize = Nt->OptionalHeader.SizeOfImage;
		auto PatternBytes = PatternToByte(Signature);
		auto ScanBytes = reinterpret_cast<std::uint8_t*>(ModuleBase);

		auto s = PatternBytes.size();
		auto d = PatternBytes.data();

		for (auto i = 0ul; i < ImageSize - s; ++i)
		{
			bool found = true;

			for (auto j = 0ul; j < s; ++j)
			{
				if (ScanBytes[i + j] != d[j] && d[j] != -1)
				{
					found = false;
					break;
				}
			}
			if (found)
				return &ScanBytes[i];
		}

		return nullptr;
	}
}
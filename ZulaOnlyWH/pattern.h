#pragma once

char EntityListArray[] = "\x3C\x00\x00\x00\x90\x01\x00\x00\x48\x01\x00\x00\xF8\x00\x00\x00\xE8";//DC 8D 60 00 00 00 DD 9D 70 00 00 00 E8 08 00 00 00

char EntityList_Mask[] = "xxxxxxxxxxxxxxxxx";

char* EntityListAddr;

char* ScanBasic(char* pattern, char* mask, char* begin, intptr_t size)
{
	intptr_t patternLen = strlen(mask);

	for (size_t i = 0; i < size; i++)
	{
		bool found = true;
		for (int j = 0; j < patternLen; j++)
		{
			if (mask[j] != '?' && pattern[j] != *(char*)((intptr_t)begin + i + j))
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return (begin + i);
		}
	}
	return nullptr;
}

char* ScanInternal(char* pattern, char* mask, char* begin, intptr_t size)
{
	char* match{ nullptr };
	MEMORY_BASIC_INFORMATION mbi{};

	for (char* curr = begin; curr < begin + size; curr += mbi.RegionSize)
	{
		if (!VirtualQuery(curr, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT || mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) continue;

		match = ScanBasic(pattern, mask, curr, mbi.RegionSize);

		if (match != nullptr)
		{
			break;
		}
	}
	return match;
}



char* ScanProc(char* pattern, char* mask)
{
	unsigned long long int kernelMemory = (sizeof(char*) == 4) ? 0x80000000 : 0x800000000000;

	return ScanInternal(pattern, mask, (char*)0x30000000, (uintptr_t)kernelMemory);
}
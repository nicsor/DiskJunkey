#include <guiddef.h>
#include <sal.h>
#include <stdint.h>

#include "DataFile.h"

#pragma once
class Driver
{
private:
	HANDLE m_microphone = INVALID_HANDLE_VALUE;
	HANDLE m_speaker    = INVALID_HANDLE_VALUE;

public:
	Driver(_In_ LPGUID InterfaceGuid);
	~Driver();

	bool send_data(char* buffer, DWORD length);
	DWORD retrieve_data(char* buffer, DWORD length);

	DWORD retrieve_speaker_data(char* buffer, DWORD length);
	DWORD retrieve_speaker_format(WAVEFORMATEX* format);

private:
	// uninspired name
	DWORD retrieve_something(DWORD ioctl, void* buffer, DWORD length);
};


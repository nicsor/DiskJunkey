#include "stdafx.h"
#include "Driver.h"



#include <DriverSpecs.h>
_Analysis_mode_(_Analysis_code_type_user_code_)

#define INITGUID

#include <windows.h>



#pragma warning(disable:4201)  // nameless struct/union
#include <winioctl.h>
#pragma warning(default:4201)

#include <strsafe.h>
#include <cfgmgr32.h>
#include <stdio.h>
#include <strsafe.h>
#include <stdlib.h>
#include <string>


// =======================================================================
// =======================================================================

#define FILEIO_TYPE 40001
//
// The IOCTL function codes from 0x800 to 0xFFF are for customer use.
//
#define IOCTL_NONPNP_METHOD_IN_DIRECT \
    CTL_CODE( FILEIO_TYPE, 0x900, METHOD_IN_DIRECT, FILE_ANY_ACCESS  )

#define IOCTL_NONPNP_METHOD_OUT_DIRECT \
    CTL_CODE( FILEIO_TYPE, 0x901, METHOD_OUT_DIRECT , FILE_ANY_ACCESS  )


// =======================================================================
// =======================================================================



Driver::Driver(_In_ LPGUID InterfaceGuid)
{
    CONFIGRET cr = CR_SUCCESS;
    PSTR deviceInterfaceList = NULL;
    ULONG deviceInterfaceListLength = 0;
    PSTR nextInterface;

    cr = CM_Get_Device_Interface_List_Size(
        &deviceInterfaceListLength,
        InterfaceGuid,
        NULL,
        CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
    if (cr != CR_SUCCESS) {
        throw std::exception("Oh nooooo ... error when retrieving device interface list");
    }

    if (deviceInterfaceListLength <= 1) {
        throw std::exception("Oh nooooo ... no active devices found. Make sure driver is loaded");
    }

    deviceInterfaceList = (PSTR)malloc(deviceInterfaceListLength * sizeof(CHAR));
    if (deviceInterfaceList == NULL) {
        throw std::exception("Oh nooooo ... not enough memory for device interface list.");
    }
    ZeroMemory(deviceInterfaceList, deviceInterfaceListLength * sizeof(CHAR));

    cr = CM_Get_Device_Interface_List(
        InterfaceGuid,
        NULL,
        deviceInterfaceList,
        deviceInterfaceListLength,
        CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

    if (cr != CR_SUCCESS) {
        throw std::exception("Oh nooooo ... error retrieving device interface list");
    }

    nextInterface = deviceInterfaceList;

    do {
        if (strlen(nextInterface) <= 1) {
            // TODO: quite stupid ... recheck
            break;
        }

        if (m_microphone == INVALID_HANDLE_VALUE && strstr(nextInterface, "WaveSpeaker") != NULL)
        {
            m_microphone = CreateFile(nextInterface,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                0,
                NULL);
        }

        if (m_speaker == INVALID_HANDLE_VALUE && strstr(nextInterface, "WaveMicArray1") != NULL)
        {
            m_speaker = CreateFile(nextInterface,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                0,
                NULL);
        }

        nextInterface = nextInterface + strlen(nextInterface) + 1;

    } while (nextInterface != NULL);

    if ((m_microphone == INVALID_HANDLE_VALUE) || (m_speaker == INVALID_HANDLE_VALUE))
    {
        throw std::exception("Oh nooooo ... unknown interfaces");
    }
}

Driver::~Driver()
{
    CloseHandle(m_speaker);
    CloseHandle(m_microphone);
}

bool Driver::send_data(char* buffer, DWORD length)
{
    char outBuffer[1] = { 0 };
    bool status;
    ULONG bytesReturned;

    status = DeviceIoControl(m_microphone,
        (DWORD)IOCTL_NONPNP_METHOD_IN_DIRECT,
        buffer,
        (DWORD)length + 1,
        outBuffer,
        sizeof(outBuffer),
        &bytesReturned,
        NULL
    );

	return status;
}

DWORD Driver::retrieve_data(char* buffer, DWORD length)
{
    char inBuffer[1] = {0};
    bool status;
    ULONG bytesReturned;

    status = DeviceIoControl(m_speaker,
        (DWORD)IOCTL_NONPNP_METHOD_OUT_DIRECT,
        inBuffer,
        sizeof(inBuffer),
        buffer,
        length,
        &bytesReturned,
        NULL
    );

    if (!status)
    {
        return (DWORD)-1;
    }

	return bytesReturned;
}
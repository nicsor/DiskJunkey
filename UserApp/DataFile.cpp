#include "stdafx.h"
#include "DataFile.h"

//=============================================================================
// Defines
//=============================================================================
#define RIFF_TAG                    0x46464952;
#define WAVE_TAG                    0x45564157;
#define FMT__TAG                    0x20746D66;
#define DATA_TAG                    0x61746164;

char* superbuffer[1024 * 1024] = {0};
int superbufferoffset = 0;

DataFile::DataFile(const char* filename, PWAVEFORMATEX waveFormat)
:  m_file(filename, std::ios::out | std::ios::binary)
{
    m_FileHeader.dwRiff           = RIFF_TAG;
    m_FileHeader.dwFileSize       = 0;
    m_FileHeader.dwWave           = WAVE_TAG;
    m_FileHeader.dwFormat         = FMT__TAG;
    m_FileHeader.dwFormatLength   = sizeof(WAVEFORMATEX);

    m_DataHeader.dwData           = DATA_TAG;
    m_DataHeader.dwDataLength     = 0;

    // Write the file header
    {
        m_FileHeader.dwFormatLength = (waveFormat->wFormatTag == WAVE_FORMAT_PCM) ?
            sizeof(PCMWAVEFORMAT) :
            sizeof(WAVEFORMATEX) + waveFormat->cbSize;

        m_file.write((char*)&m_FileHeader, sizeof(m_FileHeader));
        m_file.write((char*)waveFormat, m_FileHeader.dwFormatLength);
        m_dataOffset = m_file.tellp();
        m_file.write((char*)&m_DataHeader, sizeof(m_DataHeader));
    }

}

//=============================================================================
DataFile::~DataFile()
{
    auto fileSize = m_file.tellp();
    m_FileHeader.dwFileSize = (DWORD)fileSize - 2 * sizeof(DWORD);
    m_DataHeader.dwDataLength = (DWORD)fileSize -
                                    sizeof(m_FileHeader)        -
                                    m_FileHeader.dwFormatLength -
                                    sizeof(m_DataHeader);

    // update file header
    m_file.seekp(0);
    m_file.write((char*)&m_FileHeader, sizeof(m_FileHeader));
    //m_file.write((char*)m_waveFormat, m_FileHeader.dwFormatLength);
    m_file.seekp(m_dataOffset);    
    m_file.write((char*)&m_DataHeader, sizeof(m_DataHeader));
    m_file.seekp(fileSize);
    
    m_file.close();
}

bool
DataFile::write_data
(
    const char * pData,
    int    ulDataSize
)
{
    m_file.write(pData, ulDataSize);

    return true;
}
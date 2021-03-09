#ifndef _BASEAUDIODRIVER_SAVEDATA_H
#define _BASEAUDIODRIVER_SAVEDATA_H

#include "stdafx.h"
#include<fstream>


#include <pshpack1.h>
typedef struct tWAVEFORMATEX
{
    WORD    wFormatTag;        /* format type */
    WORD    nChannels;         /* number of channels (i.e. mono, stereo...) */
    DWORD   nSamplesPerSec;    /* sample rate */
    DWORD   nAvgBytesPerSec;   /* for buffer estimation */
    WORD    nBlockAlign;       /* block size of data */
    WORD    wBitsPerSample;    /* Number of bits per sample of mono data */
    WORD    cbSize;            /* The count in bytes of the size of
                                    extra information (after cbSize) */
} WAVEFORMATEX;

typedef WAVEFORMATEX* PWAVEFORMATEX;

/* general waveform format structure (information common to all formats) */
typedef struct waveformat_tag {
    WORD    wFormatTag;        /* format type */
    WORD    nChannels;         /* number of channels (i.e. mono, stereo...) */
    DWORD   nSamplesPerSec;    /* sample rate */
    DWORD   nAvgBytesPerSec;   /* for buffer estimation */
    WORD    nBlockAlign;       /* block size of data */
} WAVEFORMAT;

/* specific waveform format structure for PCM data */
typedef struct pcmwaveformat_tag {
    WAVEFORMAT  wf;
    WORD        wBitsPerSample;
} PCMWAVEFORMAT;
#include <poppack.h>


#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM         1
#endif

// wave file header.
#include <pshpack1.h>
typedef struct _OUTPUT_FILE_HEADER
{
    DWORD dwRiff;
    DWORD dwFileSize;
    DWORD dwWave;
    DWORD dwFormat;
    DWORD dwFormatLength;
} OUTPUT_FILE_HEADER;
typedef OUTPUT_FILE_HEADER *POUTPUT_FILE_HEADER;

typedef struct _OUTPUT_DATA_HEADER
{
    DWORD dwData;
    DWORD dwDataLength;
} OUTPUT_DATA_HEADER;
typedef OUTPUT_DATA_HEADER *POUTPUT_DATA_HEADER;

#include <poppack.h>

class DataFile
{
protected:
    OUTPUT_FILE_HEADER          m_FileHeader;
    PWAVEFORMATEX               m_waveFormat;
    OUTPUT_DATA_HEADER          m_DataHeader;
    std::ofstream               m_file;

public:
    DataFile(const char* filename, PWAVEFORMATEX waveFormat);
    ~DataFile();

    bool DataFile::write_data(
        const char* pData,
        int    ulDataSize
    );

    int getOffset();
};

#endif

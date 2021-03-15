#include "DataBuffer.h"

#include <wdm.h>

DataBuffer::DataBuffer(unsigned int bufferSize) :
    m_buffer_size(bufferSize)
{
    m_buffer = (unsigned char *) ExAllocatePool2(POOL_FLAG_NON_PAGED, bufferSize, 'DBPL');

    if (!m_buffer)
    {
        // DPF(D_TERSE, ("Could not allocate memory for saving Data"));
        m_buffer_size = 0;
    }

    reset();
}

DataBuffer::~DataBuffer()
{
    if (m_buffer)
    {
        ExFreePoolWithTag(m_buffer, 'DBPL');
    }
}

void DataBuffer::reset()
{
    m_offset_read = 0;
    m_offset_write = 0;
    m_available_data = 0;
}


#pragma code_seg("PAGE")
unsigned int DataBuffer::push(const char* data, unsigned int length)
{
    // unsigned int requested = length;

    // Drop what is over, don't overwrite data
    length = min(length, m_buffer_size - m_available_data);

    // DPF(D_TERSE, ("+ 0x%x 0x%x %s, %d %d", length, requested, (requested >= length) ? "-" : ">", m_offset_write, (m_offset_write + length) % m_buffer_size))

    unsigned int writeSize = length;

    if (m_offset_write + length >= m_buffer_size)
    {
        writeSize = m_buffer_size - m_offset_write;
    }

    RtlCopyMemory(m_buffer + m_offset_write, data, writeSize);

    // wrap around
    if (writeSize != length) {
        unsigned int remaining = length - writeSize;
        RtlCopyMemory(m_buffer, data + writeSize, remaining);
        m_offset_write = remaining;
    }
    else {
        m_offset_write += writeSize;
    }

    m_available_data += length;
    m_offset_write %= m_buffer_size;

    return length;
}

#pragma code_seg("PAGE")
unsigned int DataBuffer::pop(char* data, unsigned int length)
{
    static unsigned int empty_buffers = 0;

    unsigned int copy = min(length, m_available_data);
    unsigned int offset = m_offset_read;

    if (copy == 0) {
        ++empty_buffers;
    }
    else {
        // DPF(D_TERSE, ("- 0x%x 0x%x %s [%d] %d %d", copy, length, (copy < length) ? "<" : "-", empty_buffers, m_offset_read, (m_offset_read + copy) % m_buffer_size))
        empty_buffers = 0;
    }

    if (copy != 0)
    {
        unsigned int copySize = copy;

        if (offset + copy >= m_buffer_size)
        {
            copySize = m_buffer_size - offset;
        }

        RtlCopyMemory(data, m_buffer + offset, copySize);

        if (copySize != copy)
        {
            unsigned int remaining = copy - copySize;
            RtlCopyMemory(data + copySize, m_buffer, remaining);
            offset = remaining;
        }
        else {
            offset = copySize;
        }

        m_offset_read += copy;
        m_offset_read %= m_buffer_size;
    }
    else {
        offset = 0;
    }

    // Zero unavailable data.
    if (copy != length)
    {
        unsigned int zeros = length - copy;
        unsigned int zerosSize = zeros;

        if (offset + zeros >= m_buffer_size)
        {
            zerosSize = m_buffer_size - offset;
        }

        RtlZeroMemory(data + offset, zerosSize);

        if (zerosSize != zeros)
        {
            unsigned int remaining = zeros - zerosSize;
            RtlZeroMemory(data + offset + zerosSize, remaining);
        }
    }

    m_available_data -= copy;

    return copy;
}
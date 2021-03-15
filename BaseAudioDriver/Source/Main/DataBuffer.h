#pragma once

class DataBuffer
{
private:
	unsigned int m_buffer_size;
	unsigned char* m_buffer;

	unsigned int m_offset_write;
	unsigned int m_offset_read;
	unsigned int m_available_data; // std::atomic<ULONG> ?

public:
	DataBuffer(unsigned int size);
	~DataBuffer();

	void reset();

	unsigned int push(const char* data, unsigned int length);
	unsigned int pop(char* data, unsigned int length);
};


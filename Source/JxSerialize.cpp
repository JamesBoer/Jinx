/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx
{

	inline_t void BinaryReader::Read(String * val)
	{
		uint32_t length;
		m_buffer->Read(&m_pos, &length, sizeof(length));
		if (length < (1024 - 1))
		{
			char buffer[1024];
			m_buffer->Read(&m_pos, buffer, length + 1);
			*val = buffer;
		}
		else
		{
			char * buffer = (char *)JinxAlloc(length + 2);
			m_buffer->Read(&m_pos, buffer, length + 1);
			*val = buffer;
			JinxFree(buffer);
		}
	}

	inline_t void BinaryReader::Read(BufferPtr & val)
	{
		uint32_t size;
		m_buffer->Read(&m_pos, &size, sizeof(size));
		val->Reserve(size);
		m_buffer->Read(&m_pos, val, size);
	}


	inline_t void BinaryWriter::Write(const String & val)
	{
		uint32_t size = (uint32_t)val.size();
		m_buffer->Write(&m_pos, &size, sizeof(size));
		m_buffer->Write(&m_pos, val.c_str(), size + 1);
	}

	inline_t void BinaryWriter::Write(const BufferPtr & val)
	{
		uint32_t size = (uint32_t)val->Size();
		m_buffer->Write(&m_pos, &size, sizeof(uint32_t));
		m_buffer->Write(&m_pos, val->Ptr(), val->Size());
	}

	inline_t void BinaryWriter::Write(BinaryReader & reader, size_t bytes)
	{
		assert((reader.m_pos + bytes) <= reader.m_buffer->Size());
		m_buffer->Write(&m_pos, reader.m_buffer->Ptr() + reader.m_pos, bytes);
		reader.m_pos += bytes;
	}

} // namespace Jinx
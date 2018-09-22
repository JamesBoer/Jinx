/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx
{
	inline_t Buffer::Buffer()
		: m_data(nullptr), m_size(0), m_capacity(0)
	{
	}

	inline_t Buffer::~Buffer()
	{
		JinxFree(m_data);
	}

	inline_t size_t Buffer::Capacity() const
	{
		return m_capacity;
	}

	inline_t void Buffer::Clear()
	{
		m_size = 0;
	}

	inline_t void Buffer::Read(size_t * pos, void * data, size_t bytes)
	{
		assert(*pos < m_size);
		assert(bytes <= (m_size - *pos));
		size_t bytesToCopy = std::min(bytes, m_size - *pos);
		assert(bytesToCopy);
		assert(bytesToCopy == bytes);
		memcpy(data, m_data + *pos, bytesToCopy);
		*pos += bytes;
	}

	inline_t void Buffer::Read(size_t * pos, BufferPtr & buffer, size_t bytes)
	{
		assert(*pos < m_size);
		assert(bytes <= (m_size - *pos));
		size_t bytesToCopy = std::min(bytes, m_size - *pos);
		assert(bytesToCopy);
		assert(bytesToCopy == bytes);
		buffer->Reserve(bytesToCopy);
		memcpy(buffer->m_data, m_data + *pos, bytesToCopy);
		*pos += bytes;
		buffer->m_size = bytesToCopy;
	}

	inline_t void Buffer::Reserve(size_t size)
	{
		if (m_data)
		{
			if (size <= m_capacity)
				return;
			uint8_t * newData = (uint8_t *)JinxAlloc(size);
			memcpy(newData, m_data, m_size);
			JinxFree(m_data);
			m_data = newData;
			m_capacity = size;
		}
		else
		{
			m_data = (uint8_t *)JinxRealloc(m_data, size);
			m_capacity = size;
		}
	}

	inline_t void Buffer::Write(const void * data, size_t bytes)
	{
		assert(data && bytes);
		if (m_capacity < bytes)
			Reserve((m_capacity + bytes) + (m_capacity / 2));
		memcpy(m_data, data, bytes);
		m_size = bytes;
	}

	inline_t void Buffer::Write(size_t * pos, const void * data, size_t bytes)
	{
		assert(*pos <= m_size);
		assert(data && bytes);
		if (m_capacity < (*pos + bytes))
			Reserve((m_capacity + bytes) + (m_capacity / 2));
		memcpy(m_data + *pos, data, bytes);
		*pos += bytes;
		if (m_size < *pos)
			m_size = *pos;
	}

	inline_t BufferPtr CreateBuffer()
	{
		return std::allocate_shared<Buffer>(Allocator<Buffer>());
	}

} // namespace Jinx

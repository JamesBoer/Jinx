/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_BUFFER_H__
#define JX_BUFFER_H__

/*! \file */

/*! \namespace */
namespace Jinx
{
	class Buffer;
	using BufferPtr = std::shared_ptr<Buffer>;

	class Buffer 
	{
	public:
		Buffer();
		~Buffer();
		size_t Capacity() const;
		void Clear();
		void Read(size_t * pos, void * data, size_t bytes);
		void Read(size_t * pos, BufferPtr & buffer, size_t bytes);
		void Reserve(size_t size);
		inline uint8_t * Ptr() const { return m_data; }
		inline size_t Size() const { return m_size; }
		void Write(const void * data, size_t bytes);
		void Write(size_t * pos, const void * data, size_t bytes);

	private:
		uint8_t * m_data;
		size_t m_size;
		size_t m_capacity;
	};


	BufferPtr CreateBuffer();
}

#endif // JX_BUFFER_H__

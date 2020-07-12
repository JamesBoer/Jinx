/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_SERIALIZE_H__
#define JX_SERIALIZE_H__


namespace Jinx
{

	class BinaryReader
	{
	public:
		explicit BinaryReader(BufferPtr buffer) :
			m_buffer(buffer),
			m_pos(0)
		{}
		BinaryReader(const BinaryReader & other) :
			m_buffer(other.m_buffer),
			m_pos(other.m_pos)
		{}

		inline void Read(bool * val) { uint8_t b = false; m_buffer->Read(&m_pos, &b, sizeof(uint8_t)); *val = b ? true : false; }
		inline void Read(char * val) { m_buffer->Read(&m_pos, val, sizeof(char)); }
		inline void Read(int8_t * val) { m_buffer->Read(&m_pos, val, sizeof(int8_t)); }
		inline void Read(int16_t * val) { m_buffer->Read(&m_pos, val, sizeof(int16_t)); }
		inline void Read(int32_t * val) { m_buffer->Read(&m_pos, val, sizeof(int32_t)); }
		inline void Read(int64_t * val) { m_buffer->Read(&m_pos, val, sizeof(int64_t)); }
		inline void Read(uint8_t * val) { m_buffer->Read(&m_pos, val, sizeof(uint8_t)); }
		inline void Read(uint16_t * val) { m_buffer->Read(&m_pos, val, sizeof(uint16_t)); }
		inline void Read(uint32_t * val) { m_buffer->Read(&m_pos, val, sizeof(uint32_t)); }
		inline void Read(uint64_t * val) { m_buffer->Read(&m_pos, val, sizeof(uint64_t)); }
		inline void Read(float * val) { m_buffer->Read(&m_pos, val, sizeof(float)); }
		inline void Read(double * val) { m_buffer->Read(&m_pos, val, sizeof(double)); }

		void Read(String * val);
		void Read(BufferPtr & val);

		void Read(void * val, size_t bytes) { m_buffer->Read(&m_pos, val, bytes); }

		// Useful for reading generic data (like enums) whose type is different than the raw serialized data
		template<typename T0, typename T1>
		void Read(T0 * val)
		{
			T1 v;
			Read(&v);
			*val = static_cast<T0>(v);
		}

		size_t Tell() const { return m_pos; }
		void Seek(size_t pos) { m_pos = pos; }
		size_t Size() const { return m_buffer->Size(); }

	private:
		friend class BinaryWriter;
		BufferPtr m_buffer;
		size_t m_pos;
	};

	class BinaryWriter
	{
	public:
		BinaryWriter(BufferPtr buffer) :
			m_buffer(buffer),
			m_pos(0)
		{}

		inline void Write(bool val) { uint8_t b = val ? 1 : 0; m_buffer->Write(&m_pos, &b, sizeof(uint8_t)); }
		inline void Write(char val) { m_buffer->Write(&m_pos, &val, sizeof(char)); }
		inline void Write(int8_t val) { m_buffer->Write(&m_pos, &val, sizeof(int8_t)); }
		inline void Write(int16_t val) { m_buffer->Write(&m_pos, &val, sizeof(int16_t)); }
		inline void Write(int32_t val) { m_buffer->Write(&m_pos, &val, sizeof(int32_t)); }
		inline void Write(int64_t val) { m_buffer->Write(&m_pos, &val, sizeof(int64_t)); }
		inline void Write(uint8_t val) { m_buffer->Write(&m_pos, &val, sizeof(uint8_t)); }
		inline void Write(uint16_t val) { m_buffer->Write(&m_pos, &val, sizeof(uint16_t)); }
		inline void Write(uint32_t val) { m_buffer->Write(&m_pos, &val, sizeof(uint32_t)); }
		inline void Write(uint64_t val) { m_buffer->Write(&m_pos, &val, sizeof(uint64_t)); }
		inline void Write(float val) { m_buffer->Write(&m_pos, &val, sizeof(float)); }
		inline void Write(double val) { m_buffer->Write(&m_pos, &val, sizeof(double)); }

		void Write(const String & val);
		void Write(const BufferPtr & val);
		void Write(BinaryReader & reader, size_t bytes);
		void Write(const void * val, size_t bytes) { m_buffer->Write(&m_pos, val, bytes); }

		// Useful for writing generic data (like enums) whose type is different than the raw serialized data
		template<typename T0, typename T1>
		void Write(T0 val)
		{
			T1 v = static_cast<T1>(val);
			Write(v);
		}

		size_t Tell() const { return m_pos; }
		void Seek(size_t pos) { m_pos = pos; }
		size_t Size() const { return m_buffer->Size(); }

	private:

		BufferPtr m_buffer;
		size_t m_pos;
	};

} // namespace Jinx

#endif // JX_SERIALIZE_H__

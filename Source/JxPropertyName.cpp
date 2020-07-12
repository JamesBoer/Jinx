/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx::Impl
{

	inline_t PropertyName::PropertyName()
	{
	}

	inline_t PropertyName::PropertyName(VisibilityType visibility, bool readOnly, const String & moduleName, const String & propertyName, const Variant & defaultValue) :
		m_visibility(visibility),
		m_readOnly(readOnly),
		m_name(propertyName),
		m_defaultValue(defaultValue)
	{
		String combinedName = moduleName + " " + propertyName;
		m_id = GetHash(combinedName.c_str(), combinedName.size());
		m_partCount = GetNamePartCount(propertyName);
	}

	inline_t void PropertyName::Read(BinaryReader & reader)
	{
		reader.Read(&m_id);
		reader.Read(&m_readOnly);
		reader.Read(&m_name);
		reader.Read<VisibilityType, uint8_t>(&m_visibility);
		reader.Read<size_t, uint16_t>(&m_partCount);
		m_defaultValue.Read(reader);
	}

	inline_t void PropertyName::Write(BinaryWriter & writer) const
	{
		writer.Write(m_id);
		writer.Write(m_readOnly);
		writer.Write(m_name);
		writer.Write<VisibilityType, uint8_t>(m_visibility);
		writer.Write<size_t, uint16_t>(m_partCount);
		m_defaultValue.Write(writer);
	}

} // namespace Jinx::Impl


/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

using namespace Jinx;

PropertyName::PropertyName() :
	m_readOnly(false),
	m_visibility(VisibilityType::Local),
	m_id(0)
{
}

PropertyName::PropertyName(bool readOnly, VisibilityType visibility, const String & moduleName, const String & propertyName, Variant defaultValue) :
	m_readOnly(readOnly),
	m_visibility(visibility),
	m_name(propertyName),
	m_defaultValue(defaultValue)
{
	String combinedName = moduleName + " " + propertyName;
	m_id = GetHash(reinterpret_cast<const uint8_t *>(combinedName.c_str()), static_cast<uint32_t>(combinedName.size()));
}

void PropertyName::Read(BinaryReader & reader)
{
	reader.Read(&m_id);
	reader.Read(&m_readOnly);
	reader.Read(&m_name);
	reader.Read<VisibilityType, uint8_t>(&m_visibility);
	m_defaultValue.Read(reader);
}

void PropertyName::Write(BinaryWriter & writer) const
{
	writer.Write(m_id);
	writer.Write(m_readOnly);
	writer.Write(m_name);
	writer.Write<VisibilityType, uint8_t>(m_visibility);
	m_defaultValue.Write(writer);
}

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
	m_scope(ScopeType::Local),
	m_id(0)
{
}

PropertyName::PropertyName(bool readOnly, ScopeType scope, const String & moduleName, const String & propertyName, Variant defaultValue) :
	m_readOnly(readOnly),
	m_scope(scope),
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
	reader.Read<ScopeType, uint8_t>(&m_scope);
	m_defaultValue.Read(reader);
}

void PropertyName::Write(BinaryWriter & writer) const
{
	writer.Write(m_id);
	writer.Write(m_readOnly);
	writer.Write(m_name);
	writer.Write<ScopeType, uint8_t>(m_scope);
	m_defaultValue.Write(writer);
}

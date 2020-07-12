/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_PROPERTY_H__
#define JX_PROPERTY_H__


namespace Jinx::Impl
{

	class PropertyName
	{
	public:
		PropertyName();
		PropertyName(VisibilityType visibility, bool readOnly, const String & moduleName, const String & propertyName, const Variant & defaultValue = nullptr);

		bool IsValid() const { return m_visibility != VisibilityType::Local; }
		bool IsReadOnly() const { return m_readOnly; }
		RuntimeID GetId() const { return m_id; }
		const String & GetName() const { return m_name; }
		VisibilityType GetVisibility() const { return m_visibility; }
		size_t GetPartCount() const { return m_partCount; }
		const Variant & GetDefaultValue() const { return m_defaultValue; }

		// Serialization
		void Read(BinaryReader & reader);
		void Write(BinaryWriter & writer) const;

	private:

		RuntimeID m_id = 0;
		VisibilityType m_visibility = VisibilityType::Local;
		bool m_readOnly = false;
		String m_name;
		Variant m_defaultValue;
		size_t m_partCount = 0;
	};

} // namespace Jinx::Impl

#endif // JX_PROPERTY_H__


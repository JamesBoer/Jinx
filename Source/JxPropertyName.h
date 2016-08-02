/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_PROPERTY_H__
#define JX_PROPERTY_H__


namespace Jinx
{

	class PropertyName
	{
	public:
		PropertyName();
		PropertyName(bool readOnly, VisibilityType visibility, const String & moduleName, const String & propertyName, Variant defaultValue = nullptr);

		bool IsValid() const { return m_visibility != VisibilityType::Local; }
		bool IsReadOnly() const { return m_readOnly; }
		RuntimeID GetId() const { return m_id; }
		const String & GetName() const { return m_name; }
		VisibilityType GetVisibility() const { return m_visibility; }
		const Variant & GetDefaultValue() const { return m_defaultValue; }

		// Serialization
		void Read(BinaryReader & reader);
		void Write(BinaryWriter & writer) const;

	private:

		RuntimeID m_id;
		bool m_readOnly;
		String m_name;
		VisibilityType m_visibility;
		Variant m_defaultValue;
	};


};

#endif // JX_PROPERTY_H__


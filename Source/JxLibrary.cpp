/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx::Impl
{

	inline_t Library::Library(RuntimeWPtr runtime, const String & name) :
		m_name(name),
		m_maxPropertyParts(0),
		m_runtime(runtime)
	{
	}

	inline_t FunctionSignature Library::CreateFunctionSignature(Visibility visibility, const String & name) const
	{
		Lexer lexer(m_runtime.lock()->GetSymbolTypeMap(), name.c_str(), name.c_str() + name.size(), name);
		if (!lexer.Execute())
			return FunctionSignature();
		Parser parser(m_runtime.lock(), lexer.GetSymbolList(), name);
		return parser.ParseFunctionSignature(visibility == Visibility::Public ? VisibilityType::Public : VisibilityType::Private, m_name);
	}

	inline_t FunctionSignature Library::FindFunctionSignature(Visibility visibility, const String & name) const
	{
		auto signature = CreateFunctionSignature(visibility, name);
		std::lock_guard<std::mutex> lock(m_functionMutex);
		auto itr = std::find(m_functionList.begin(), m_functionList.end(), signature);
		if (itr == m_functionList.end())
			return FunctionSignature();
		return signature;
	}

	inline_t const FunctionPtrList Library::Functions() const
	{
		std::lock_guard<std::mutex> lock(m_functionMutex);
		FunctionPtrList fnList;
		fnList.reserve(m_functionList.size());
		for (const auto & fn : m_functionList)
			fnList.push_back(&fn);
		return fnList;
	}

	inline_t bool Library::FunctionSignatureExists(const FunctionSignature & signature) const
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex);
		auto itr = std::find(m_functionList.begin(), m_functionList.end(), signature);
		return itr == m_functionList.end() ? false : true;
	}

	inline_t PropertyName Library::GetPropertyName(const String & name)
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex);
		auto itr = m_propertyNameTable.find(name);
		if (itr == m_propertyNameTable.end())
			return PropertyName();
		return itr->second;
	}

	inline_t Variant Library::GetProperty(const String & name) const
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex);
		auto itr = m_propertyNameTable.find(name);
		if (itr == m_propertyNameTable.end())
			return Variant();
		auto runtime = m_runtime.lock();
		if (!runtime)
			return Variant();
		return runtime->GetProperty(itr->second.GetId());
	}

	inline_t bool Library::PropertyNameExists(const String & name) const
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex);
		return m_propertyNameTable.find(name) == m_propertyNameTable.end() ? false : true;
	}

	inline_t bool Library::RegisterFunction(Visibility visibility, const String & name, FunctionCallback function)
	{
		if (name.empty())
		{
			LogWriteLine(LogLevel::Error, "Registered function requires a valid name");
			return false;
		}
		if (!function)
		{
			LogWriteLine(LogLevel::Error, "Registered function requires a valid callback");
			return false;
		}

		// Calculate the function signature
		auto signature = CreateFunctionSignature(visibility, name);
		if (!signature.IsValid())
			return false;

		// Register function in library table.  This allows the the parser to find
		// and use this function.
		RegisterFunctionSignature(signature);

		// Register the function definition with the runtime system for 
		// runtime lookups.
		auto runtime = m_runtime.lock();
		if (!runtime)
			return false;
		runtime->RegisterFunction(signature, function);

		// Return success
		return true;
	}

	inline_t void Library::RegisterFunctionSignature(const FunctionSignature & signature)
	{
		std::lock_guard<std::mutex> lock(m_functionMutex);
		m_functionList.push_back(signature);
	}

	inline_t bool Library::RegisterProperty(Visibility visibility, Access access, const String & name, const Variant & value)
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex);

		// Register the property name with the library
		PropertyName prop(visibility == Visibility::Public ? VisibilityType::Public : VisibilityType::Private, access == Access::ReadOnly ? true : false, GetName(), name);
		if (!RegisterPropertyNameInternal(prop, false))
			return false;

		// Set the property with the runtime value
		auto runtime = m_runtime.lock();
		if (!runtime)
			return false;
		runtime->SetProperty(prop.GetId(), value);

		// Return success
		return true;
	}

	inline_t bool Library::RegisterPropertyName(const PropertyName & propertyName, bool checkForDuplicates)
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex);
		return RegisterPropertyNameInternal(propertyName, checkForDuplicates);
	}

	inline_t bool Library::RegisterPropertyNameInternal(const PropertyName & propertyName, bool checkForDuplicates)
	{
		if (checkForDuplicates && (m_propertyNameTable.find(propertyName.GetName()) != m_propertyNameTable.end()))
			return false;
		m_propertyNameTable.insert(std::make_pair(propertyName.GetName(), propertyName));
		if (propertyName.GetPartCount() > m_maxPropertyParts)
			m_maxPropertyParts = propertyName.GetPartCount();
		return true;
	}

	inline_t void Library::SetProperty(const String & name, const Variant & value)
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex);
		auto itr = m_propertyNameTable.find(name);
		if (itr == m_propertyNameTable.end())
			return;
		auto runtime = m_runtime.lock();
		if (!runtime)
			return;
		runtime->SetProperty(itr->second.GetId(), value);
	}

} // namespace Jinx::Impl


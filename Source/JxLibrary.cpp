/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

using namespace Jinx;


Library::Library(RuntimeWPtr runtime, const String & name) :
	m_name(name),
	m_maxPropertyParts(0),
	m_runtime(runtime)
{
}

FunctionSignature Library::CreateFunctionSignature(bool publicScope, std::initializer_list<String> name) const
{
	// Build function signature parts from parameters
	FunctionSignatureParts parts;
	for (const auto & n : name)
	{
		// Validate name
		if (n.empty())
		{
			LogWriteLine("Registered function requires a valid name");
			return FunctionSignature();
		}

		FunctionSignaturePart part;
		if (n.empty())
		{
			LogWriteLine("Empty identifier in function signature");
			return FunctionSignature();
		}
		else if (n[0] == '{')
		{
			// Validate parameter part and parse optional type
			String np;
			np.reserve(32);
			const char * p = n.c_str();
			const char * e = p + n.size();
			bool term = false;
			++p;
			while (p != e)
			{
				if (*p == '}')
				{
					++p;
					term = true;
					break;
				}
				else if (*p != ' ')
				{
					np += (char)(*p);
					part.names.push_back(np);
				}
				++p;
			}
			if (!term)
			{
				LogWriteLine("Argument not properly terminated in function signature");
				return FunctionSignature();
			}
			if (!np.empty())
			{	
				if (!StringToValueType(np, &part.valueType))
				{
					LogWriteLine("Unknown parameter value in function signature");
					return FunctionSignature();
				}
			}
			while (p != e)
			{
				if (*p != ' ')
				{
					LogWriteLine("Invalid symbols after closing argument brace");
					return FunctionSignature();
				}
				++p;
			}
			part.partType = FunctionSignaturePartType::Parameter;
		}
		else
		{
			// Break names into component parts
			String np;
			np.reserve(32);
			const char * p = n.c_str();
			const char * e = p + n.size();
			bool optional = *p == '(';
			if (optional)
			{
				if (n.size() < 3 || n[n.size() - 1] != ')')
				{
					LogWriteLine("Error when parsing optional name component");
					return FunctionSignature();
				}
				++p;
				--e;
			}
			while (p != e)
			{
				if (*p == '/')
				{
					part.names.push_back(np);
					np.clear();
				}
				else if (*p == '{' || *p == '}')
				{
					LogWriteLine("Illegal characters { or } found in name.  Missing comma?");
					return FunctionSignature();
				}
				else
				{
					np += (char)(*p);
				}
				++p;
			}
			part.partType = FunctionSignaturePartType::Name;
			part.optional = optional;
			part.names.push_back(np);
		}
		parts.push_back(part);
	}

	// Create functions signature
	FunctionSignature functionSignature(publicScope ? VisibilityType::Public : VisibilityType::Private, GetName(), parts);
	return functionSignature;
}

bool Library::FunctionSignatureExists(const FunctionSignature & signature) const
{
	for (const auto & function : m_functionList)
	{
		if (signature == function)
			return true;
	}
	return false;
}

PropertyName Library::GetPropertyName(const String & name)
{
	std::lock_guard<Mutex> lock(m_propertyMutex);
	auto itr = m_propertyNameTable.find(name);
	if (itr == m_propertyNameTable.end())
		return PropertyName();
	return itr->second;
}

Variant Library::GetProperty(const String & name) const
{
	std::lock_guard<Mutex> lock(m_propertyMutex);
	auto itr = m_propertyNameTable.find(name);
	if (itr == m_propertyNameTable.end())
		return Variant();
	auto runtime = m_runtime.lock();
	if (!runtime)
		return Variant();
	return runtime->GetProperty(itr->second.GetId());
}

bool Library::PropertyNameExists(const String & name) const
{
	std::lock_guard<Mutex> lock(m_propertyMutex);
	return m_propertyNameTable.find(name) == m_propertyNameTable.end() ? false : true;
}

bool Library::RegisterFunction(Visibility visibility, std::initializer_list<String> name, FunctionCallback function)
{
	if (name.size() < 1)
	{
		LogWriteLine("Registered function requires a valid name");
		return false;
	}
	if (!function)
	{
		LogWriteLine("Registered function requires a valid callback");
		return false;
	}

	// Calculate the function signature
	FunctionSignature functionSignature = CreateFunctionSignature(visibility == Visibility::Public ? true : false, name);
	if (!functionSignature.IsValid())
		return false;

	// Register function in library table.  This allows the the parser to find
	// and use this function.
	m_functionList.push_back(functionSignature);

	// Register the function definition with the runtime system for 
	// runtime lookups.
	auto runtime = m_runtime.lock();
	if (!runtime)
		return false;
	runtime->RegisterFunction(functionSignature, function);

	// Return success
	return true;
}

void Library::RegisterFunctionSignature(const FunctionSignature & signature)
{
	// TODO: make thread-safe
	m_functionList.push_back(signature);
}

bool Library::RegisterProperty(Visibility visibility, Access access, const String & name, const Variant & value)
{
	std::lock_guard<Mutex> lock(m_propertyMutex);

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

bool Library::RegisterPropertyName(const PropertyName & propertyName, bool checkForDuplicates)
{
	std::lock_guard<Mutex> lock(m_propertyMutex);
	return RegisterPropertyNameInternal(propertyName, checkForDuplicates);
}

bool Library::RegisterPropertyNameInternal(const PropertyName & propertyName, bool checkForDuplicates)
{
	if (checkForDuplicates && (m_propertyNameTable.find(propertyName.GetName()) != m_propertyNameTable.end()))
		return false;
	m_propertyNameTable.insert(std::make_pair(propertyName.GetName(), propertyName));
	if (propertyName.GetPartCount() > m_maxPropertyParts)
		m_maxPropertyParts = propertyName.GetPartCount();
	return true;
}

void Library::SetProperty(const String & name, const Variant & value)
{
	std::lock_guard<Mutex> lock(m_propertyMutex);
	auto itr = m_propertyNameTable.find(name);
	if (itr == m_propertyNameTable.end())
		return;
	auto runtime = m_runtime.lock();
	if (!runtime)
		return;
	runtime->SetProperty(itr->second.GetId(), value);
}


/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_LIBRARY_H__
#define JX_LIBRARY_H__


namespace Jinx
{
	class Library : public ILibrary
	{
	public:
		Library(RuntimeWPtr runtime, const String & name);

		// ILibrary interface
		bool RegisterFunction(Visibility visibility, std::initializer_list<String> name, FunctionCallback function) override;
		bool RegisterProperty(Visibility visibility, Access access, const String & name, const Variant & value) override;
		Variant GetProperty(const String & name) const override;
		void SetProperty(const String & name, const Variant & value) override;

		// Internal functions
		FunctionList & Functions() { return m_functionList; }
		const String & GetName() const { return m_name; }
		
		bool RegisterPropertyName(const PropertyName & propertyName, bool checkForDuplicates);
		bool PropertyNameExists(const String & name) const;
		PropertyName GetPropertyName(const String & name);
		size_t GetMaxPropertyParts() const { return m_maxPropertyParts; }

		void RegisterFunctionSignature(const FunctionSignature & signature);
		bool FunctionSignatureExists(const FunctionSignature & signature) const;

	private:
		
		// Private internal functions
		FunctionSignature CreateFunctionSignature(bool publicScope, std::initializer_list<String> name) const;
		bool RegisterPropertyNameInternal(const PropertyName & propertyName, bool checkForDuplicates);

		typedef std::map <String, PropertyName, std::less<String>, Allocator<std::pair<const String, PropertyName>>> PropertyNameTable;

		// Library name
		String m_name;

		// Track function definitions
		FunctionList m_functionList;

		// Properties
		mutable Mutex m_propertyMutex;
		PropertyNameTable m_propertyNameTable;
		size_t m_maxPropertyParts;

		// Weak ptr to runtime system
		RuntimeWPtr m_runtime;
	};

	typedef std::shared_ptr<Library> LibraryIPtr;
};

#endif // JX_LIBRARY_H__

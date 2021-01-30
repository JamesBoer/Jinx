/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_LIBRARY_H__
#define JX_LIBRARY_H__


namespace Jinx::Impl
{

	class Library : public ILibrary
	{
	public:
		Library(RuntimeWPtr runtime, const String & name);

		// ILibrary interface
		bool RegisterFunction(Visibility visibility, const String & name, FunctionCallback function) override;
		bool RegisterProperty(Visibility visibility, Access access, const String & name, const Variant & value) override;
		Variant GetProperty(const String & name) const override;
		void SetProperty(const String & name, const Variant & value) override;

		// Internal name getter
		const String & GetName() const { return m_name; }

		// Internal property functions
		bool RegisterPropertyName(const PropertyName & propertyName, bool checkForDuplicates);
		bool PropertyNameExists(const String & name) const;
		PropertyName GetPropertyName(const String & name);
		size_t GetMaxPropertyParts() const { return m_maxPropertyParts; }

		// Internal function signature functions
		void RegisterFunctionSignature(const FunctionSignature & signature);
		bool FunctionSignatureExists(const FunctionSignature & signature) const;
		FunctionSignature FindFunctionSignature(Visibility visibility, const String & name) const;
		FunctionSignature FindFunctionSignature(const FunctionSignature & signature) const;
		const FunctionPtrList Functions() const;

	private:

		// Create a function signature from a string
		FunctionSignature CreateFunctionSignature(Visibility visibility, const String & name) const;

		// Private internal functions
		bool RegisterPropertyNameInternal(const PropertyName & propertyName, bool checkForDuplicates);

		// Static memory pool for fast allocations
		static const size_t ArenaSize = 4096;
		StaticArena<ArenaSize> m_staticArena;

		// Library name
		String m_name;

		// Track function definitions
		mutable std::mutex m_functionMutex;
		std::vector<FunctionSignature, StaticAllocator<FunctionSignature, ArenaSize>> m_functionList{ m_staticArena };

		// Properties
		mutable std::mutex m_propertyMutex;
		std::map<String, PropertyName, std::less<String>, StaticAllocator<std::pair<const String, PropertyName>, ArenaSize>> m_propertyNameTable{ m_staticArena };
		size_t m_maxPropertyParts;

		// Weak ptr to runtime system
		RuntimeWPtr m_runtime;
	};

	using LibraryIPtr = std::shared_ptr<Library>;

} // namespace Jinx::Impl

#endif // JX_LIBRARY_H__

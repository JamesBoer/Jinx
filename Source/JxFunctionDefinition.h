/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_FUNCTION_DEFINITION_H__
#define JX_FUNCTION_DEFINITION_H__


namespace Jinx
{

	class FunctionDefinition
	{
	public:
		FunctionDefinition(const FunctionSignature & signature, BufferPtr bytecode, size_t offset) :
			m_id(signature.GetId()),
			m_parameterCount(signature.GetParameterCount()),
			m_bytecode(bytecode),
			m_offset(offset)
		{}
		FunctionDefinition(const FunctionSignature & signature, FunctionCallback callback) :
			m_id(signature.GetId()),
			m_parameterCount(signature.GetParameterCount()),
			m_callback(callback)
		{}
		size_t GetParameterCount() const { return m_parameterCount; }
		RuntimeID GetId() const { return m_id; }
		const BufferPtr & GetBytecode() const { return m_bytecode; }
		size_t GetOffset() const { return m_offset; }
		FunctionCallback GetCallback() const { return m_callback; }
		friend class FunctionTable;

	private:
		RuntimeID m_id;
		size_t m_parameterCount;
		BufferPtr m_bytecode;
		size_t m_offset;
		FunctionCallback m_callback;
	};
	
	typedef std::shared_ptr<FunctionDefinition> FunctionDefinitionPtr;

};

#endif // JX_FUNCTION_DEFINITION_H__


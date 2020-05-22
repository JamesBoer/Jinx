/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_SCRIPT_H__
#define JX_SCRIPT_H__


namespace Jinx::Impl
{

	class Script : public IScript, public std::enable_shared_from_this<Script>
	{
	public:
		Script(RuntimeIPtr runtime, BufferPtr bytecode, Any userContext);
		virtual ~Script();

		RuntimeID FindFunction(LibraryPtr library, const String & name) override;
		Variant CallFunction(RuntimeID id, Parameters params) override;

		bool Execute() override;
		bool IsFinished() const override;

		Variant GetVariable(const String & name) const override;
		void SetVariable(const String & name, const Variant & value) override;

		const String & GetName() const override { return m_name; }
		Any GetUserContext() const override { return m_userContext; }
		LibraryPtr GetLibrary() const override { return m_library; }

		std::vector<String, Allocator<String>> GetCallStack() const;

	private:
		void Error(const char * message);

		Variant GetVariable(RuntimeID id) const;
		Variant Pop();
		void Push(const Variant & value);
		void SetVariableAtIndex(RuntimeID id, size_t index);
		void SetVariable(RuntimeID id, const Variant & value);

		std::pair<CollectionPtr, Variant> WalkSubscripts(uint32_t subscripts, CollectionPtr collection);

		Variant CallFunction(RuntimeID id);

	private:
		using IdIndexMap = std::map<RuntimeID, size_t, std::less<RuntimeID>, Allocator<std::pair<const RuntimeID, size_t>>>;
		using ScopeStack = std::vector<size_t, Allocator<size_t>>;
		using FunctionMap = std::map<RuntimeID, FunctionDefinitionPtr, std::less<RuntimeID>, Allocator<std::pair<const RuntimeID, FunctionDefinitionPtr>>>;

		// Pointer to runtime object
		RuntimeIPtr m_runtime;

		// Execution frame allows jumping to remote code (function calls) and returning
		struct ExecutionFrame
		{
			ExecutionFrame(BufferPtr b, const char * n) : bytecode(b), reader(b), name(n)
			{
				scopeStack.reserve(32);
			}
			explicit ExecutionFrame(FunctionDefinitionPtr fn) : ExecutionFrame(fn->GetBytecode(), fn->GetName()) {}

			// Buffer containing script bytecode
			BufferPtr bytecode;

			// Binary reader - sequentially extracts data from bytecode buffer.  The reader's
			// current internal position acts as the current frame's instruction pointer.
			BinaryReader reader;

			// Function definition name.  Note that storing a raw string pointer is reasonably safe, 
			// because shared pointers to other objects are referenced in this struct, and there is 
			// no other way to modify the containing string.  I don't want to incur the expense of a 
			// safer string copy, which would cause an allocation cost for each function call.
			const char * name;

			// Variable id lookup map
			IdIndexMap ids;

			// Track top of stack for each level of scope
			ScopeStack scopeStack;

			// Top of the stack to clear to when this frame is popped
			size_t stackTop = 0;

			// Stop execution at the end of this frame
			bool waitOnReturn = false;
		};

		// Execution frame stack
		std::vector<ExecutionFrame, Allocator<ExecutionFrame>> m_execution;

		// Runtime stack
		std::vector<Variant, Allocator<Variant>> m_stack;

		// Current library
		LibraryIPtr m_library;

		// User context pointer
		Any m_userContext;

		// Initial position of bytecode for this script
		size_t m_bytecodeStart;

		// Is finished executing
		bool m_finished;

		// Runtime error
		bool m_error;

		// Script name
		String m_name;
	};

	using ScriptIPtr = std::shared_ptr<Script>;

} // namespace Jinx::Impl

#endif // JX_SCRIPT_H__

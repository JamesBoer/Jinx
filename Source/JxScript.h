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
	class Script;
	using ScriptIPtr = std::shared_ptr<Script>;

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

		enum class OnReturn
		{
			Continue,
			Wait,
			Finish,
		};

		std::shared_ptr<Runtime> GetRuntime() const { return std::static_pointer_cast<Runtime>(m_runtime); }
		void CallBytecodeFunction(const FunctionDefinitionPtr & fnDef, OnReturn onReturn);
		void Push(const Variant & value);
		Variant Pop();

		void Error(const char * message);

	private:

		Variant GetVariable(RuntimeID id) const;
		void Push(Variant && value);
		void SetVariableAtIndex(RuntimeID id, size_t index);
		void SetVariable(RuntimeID id, const Variant & value);
		void SetVariable(RuntimeID id, Variant && value);

		std::pair<CollectionPtr, Variant> WalkSubscripts(uint32_t subscripts, CollectionPtr collection);

		Variant CallFunction(RuntimeID id);
		Variant CallNativeFunction(const FunctionDefinitionPtr & fnDef);

	private:

		// Pointer to runtime object
		RuntimeIPtr m_runtime;

		// Execution frame allows jumping to remote code (function calls) and returning
		struct ExecutionFrame
		{
			ExecutionFrame(BufferPtr b, const char * n) : bytecode(b), reader(b), name(n) {}
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

			// Top of the stack to clear to when this frame is popped
			size_t stackTop = 0;

			// Continue or pause execution at the end of this frame
			OnReturn onReturn = OnReturn::Continue;
		};

		// Static memory pool for fast allocations
		static const size_t ArenaSize = 4096;
		StaticArena<ArenaSize> m_staticArena;

		// Execution frame stack
		std::vector<ExecutionFrame, StaticAllocator<ExecutionFrame, ArenaSize>> m_execution{ m_staticArena };

		// Runtime stack
		std::vector<Variant, StaticAllocator<Variant, ArenaSize>> m_stack{ m_staticArena };

		// Track top of stack for each level of scope
		std::vector<size_t, StaticAllocator<size_t, ArenaSize>> m_scopeStack{ m_staticArena };

		// Collection of ID-index associations
		struct IdIndexData
		{
			IdIndexData(RuntimeID i, size_t idx, size_t f) : id(i), index(idx), frameIndex(f) {}
			RuntimeID id;
			size_t index;
			size_t frameIndex;
		};
		std::vector<IdIndexData, StaticAllocator<IdIndexData, ArenaSize>> m_idIndexData{ m_staticArena };

		// Current library
		LibraryIPtr m_library;

		// User context pointer
		Any m_userContext;

		// Initial position of bytecode for this script
		size_t m_bytecodeStart = 0;

		// Is finished executing
		bool m_finished = false;

		// Runtime error
		bool m_error = false;

		// Script name
		String m_name;
	};


} // namespace Jinx::Impl

#endif // JX_SCRIPT_H__

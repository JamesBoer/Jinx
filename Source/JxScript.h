/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_SCRIPT_H__
#define JX_SCRIPT_H__


namespace Jinx
{

	class Script : public IScript, public std::enable_shared_from_this<Script>
	{
	public:
		Script(RuntimeIPtr runtime, BufferPtr bytecode);
		virtual ~Script();

		bool Execute() override;
		bool IsFinished() const override;

		Variant GetVariable(const String & name) const override;
		void SetVariable(const String & name, const Variant & value) override;

		LibraryPtr GetLibrary() const override { return m_library; }

	private:
		void Error(const char * message);

		Variant GetVariableInternal(const String & name) const;
		Variant Pop();
		void Push(const Variant & value);
		void SetVariableAtIndex(const String & name, size_t index);
		void SetVariableInternal(const String & name, const Variant & value);

	private:
		typedef std::map<String, size_t, std::less<String>, Allocator<std::pair<String, size_t>>> NameIndexMap;

		// Pointer to runtime object
		RuntimeIPtr m_runtime;

		// Name table frame for local names
		struct ScopeFrame
		{
			NameIndexMap nameMap;
			size_t stackTop;
		};

		// Execution frame allows jumping to remote code (function calls) and returning
		struct ExecutionFrame
		{
			ExecutionFrame(BufferPtr bc) : bytecode(bc), reader(bc)
			{
				ScopeFrame frame;
				frame.stackTop = 0;
				names.reserve(8);
				names.push_back(frame);
			}

			// Buffer containing script bytecode
			BufferPtr bytecode;

			// Binary reader - sequentially extracts data from bytecode buffer.  The reader's
			// current internal position acts as the current frame's instruction pointer.
			BinaryReader reader;

			// Name lookup map
			std::vector<ScopeFrame, Allocator<ScopeFrame>> names;

			// Top of the stack to clear to when this frame is popped
			size_t stackTop;
		};

		// Execution frame stack
		std::vector<ExecutionFrame, Allocator<ExecutionFrame>> m_execution;

		// Runtime stack
		std::vector<Variant, Allocator<Variant>> m_stack;

		// Current library
		LibraryIPtr m_library;

		// Is finished executing
		bool m_finished;

		// Runtime error
		bool m_error;
	};
};

#endif // JX_SCRIPT_H__
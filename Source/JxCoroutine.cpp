/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"


namespace Jinx::Impl
{

	class Coroutine final : public ICoroutine
	{
	public:
		Coroutine(std::shared_ptr<Script> script, RuntimeID functionID, const Parameters & params);

		bool IsFinished() override;
		Variant GetReturnValue() const override;

	private:

		ScriptIPtr m_script;
		Variant m_returnValue;
	};

	inline_t Coroutine::Coroutine(std::shared_ptr<Script> script, RuntimeID functionID, const Parameters & params)
	{
		auto runtime = script->GetRuntime();
		FunctionDefinitionPtr functionDef = runtime->FindFunction(functionID);
		if (!functionDef)
		{
			script->Error("Could not find function definition for coroutine");
			return;
		}
		if (!functionDef->GetBytecode())
		{
			script->Error("Native function can't be called as asynchronously as a coroutine");
			return;
		}

		// Create a script to act as a coroutine.
		m_script = std::static_pointer_cast<Script>(runtime->CreateScript(functionDef->GetBytecode(), script->GetUserContext()));

		// Push all parameters in script stack
		for (const auto & param : params)
			m_script->Push(param);

		// Call the bytecode function, indicating the script should finish execution on return
		m_script->CallBytecodeFunction(functionDef, Script::OnReturn::Finish);

		// Initial script execution
		m_script->Execute();
		if (m_script->IsFinished())
			m_returnValue = m_script->Pop();
	}

	inline_t bool Coroutine::IsFinished()
	{
		if (!m_script)
			return true;
		// Check to see if the script is finished executing.
		bool finished = m_script->IsFinished();
		if (!finished)
		{
			// If not, execute for one cycle
			m_script->Execute();
			finished = m_script->IsFinished();

			// If the script is finished, pop off the return value
			if (finished)
				m_returnValue = m_script->Pop();
		}
		return finished;
	}

	inline_t Variant Coroutine::GetReturnValue() const
	{
		return m_returnValue;
	}

} // namespace Jinx::Impl

namespace Jinx
{

	inline_t CoroutinePtr CreateCoroutine(std::shared_ptr<Impl::Script> script, RuntimeID functionID, const Parameters & params)
	{
		return std::allocate_shared<Impl::Coroutine>(Allocator<Impl::Coroutine>(), script, functionID, params);
	}

}// namespace Jinx


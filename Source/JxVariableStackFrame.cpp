/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx::Impl
{

	inline_t VariableStackFrame::VariableStackFrame()
	{
		m_frames.push_back(FrameData());
		m_frames.back().stack.push_back(VariableSet());
	}

	inline_t void VariableStackFrame::CalculateMaxVariableParts()
	{
		size_t maxVarParts = 0;

		// Retrieve the current frame data
		FrameData & frame = m_frames.back();

		// Check the entire variable stack
		for (auto itr = frame.stack.begin(); itr != frame.stack.end(); ++itr)
		{
			// Check each variable within each set
			for (auto itr2 = itr->begin(); itr2 != itr->end(); ++itr2)
			{
				size_t varParts = GetNamePartCount(*itr2);
				if (varParts > maxVarParts)
					maxVarParts = varParts;
			}
		}

		// Set current max var parts
		SetMaxVariableParts(maxVarParts);
	}

	inline_t size_t VariableStackFrame::GetMaxVariableParts() const
	{
		// Retrieve the current frame data and return max variable parts
		const FrameData & frame = m_frames.back();
		return frame.maxVariableParts;
	}

	inline_t void VariableStackFrame::SetMaxVariableParts(size_t varParts)
	{
		// Retrieve the current frame data and set max variable parts
		FrameData & frame = m_frames.back();
		frame.maxVariableParts = varParts;
	}

	inline_t size_t VariableStackFrame::GetStackDepthFromName(const String & name) const
	{
		// Sanity check
		if (m_frames.empty())
			return 0;

		// Find the variable in the current frame
		const FrameData & frame = m_frames.back();
		size_t stackDepth = frame.stack.size();
		for (auto ritr = frame.stack.rbegin(); ritr != frame.stack.rend(); ++ritr)
		{
			auto itr = ritr->find(name);
			if (itr != ritr->end())
				return stackDepth;
			--stackDepth;
		}
		return 0;
	}

	inline_t bool VariableStackFrame::VariableAssign(const String & name)
	{
		// Sanity check
		if (m_frames.empty())
		{
			m_errorMessage = "Unexpected empty variable frame";
			return false;
		}

		// Retrieve the current frame data
		FrameData & frame = m_frames.back();
		if (frame.stack.empty())
		{
			m_errorMessage = "Attempting to assign a variable to an empty stack";
			return false;
		}

		// Work from the top of the stack down, and attempt to find an existing name
		for (auto ritr = frame.stack.rbegin(); ritr != frame.stack.rend(); ++ritr)
		{
			const auto & itr = ritr->find(name);
			if (itr != ritr->end())
				return true;
		}

		// If we don't find the name, create a new variable on the top of the stack
		auto & variableSet = frame.stack.back();
		variableSet.insert(name);

		// Adjust the max variable parts value if necessary
		size_t varParts = GetNamePartCount(name);
		if (varParts > GetMaxVariableParts())
			SetMaxVariableParts(varParts);

		return true;
	}

	inline_t bool VariableStackFrame::VariableExists(const String & name) const
	{
		// Sanity check
		if (m_frames.empty())
			return false;

		// Find the variable in the current frame
		const FrameData & frame = m_frames.back();
		for (auto ritr = frame.stack.rbegin(); ritr != frame.stack.rend(); ++ritr)
		{
			auto itr = ritr->find(name);
			if (itr != ritr->end())
				return true;
		}
		return false;
	}

	inline_t void VariableStackFrame::FrameBegin()
	{
		m_frames.push_back(FrameData());
		m_frames.back().stack.push_back(VariableSet());
	}

	inline_t bool VariableStackFrame::FrameEnd()
	{
		if (m_frames.empty())
		{
			m_errorMessage = "Attempted to pop empty variable frame";
			return false;
		}
		m_frames.pop_back();
		return true;
	}

	inline_t bool VariableStackFrame::ScopeBegin()
	{
		if (m_frames.empty())
		{
			m_errorMessage = "Unexpected empty variable frame";
			return false;
		}
		FrameData & frame = m_frames.back();
		frame.stack.push_back(VariableSet());
		return true;
	}

	inline_t bool VariableStackFrame::ScopeEnd()
	{
		if (m_frames.empty())
		{
			m_errorMessage = "Unexpected empty variable frame";
			return false;
		}
		FrameData & frame = m_frames.back();
		if (frame.stack.empty())
		{
			m_errorMessage = "Attempted to pop empty variable stack";
			return false;
		}
		frame.stack.pop_back();
		CalculateMaxVariableParts();
		return true;
	}

	inline_t bool VariableStackFrame::IsRootScope() const
	{
		return (m_frames.back().stack.size() == 1) ? true : false;
	}

	inline_t bool VariableStackFrame::IsRootFrame() const
	{
		return (m_frames.size() == 1) ? true : false;
	}

} // namespace Jinx::Impl


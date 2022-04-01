/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_COROUTINE_H__
#define JX_COROUTINE_H__

/*! \file */

/*! \namespace */
namespace Jinx
{
	class ICoroutine
	{
	public:

		/// Checks if the script is finished executing, and executes if not finished
		/**
		Determines if the coroutine is finished executing, and executes if not
		\return true if finished, false if still executing.
		*/
		virtual bool IsFinished() = 0;

		/// Retrieves the coroutine's return value
		/**
		Returns the function value when finished executing
		\return value from async function.
		*/
		virtual Variant GetReturnValue() const = 0;

	protected:
		virtual ~ICoroutine() {}
	};


	class IScript;
	
	using CoroutinePtr = std::shared_ptr<ICoroutine>;
	CoroutinePtr CreateCoroutine(std::shared_ptr<Jinx::IScript> script, RuntimeID functionID, const std::vector<Variant, Allocator<Variant>> & params);
}

#endif // JX_COROUTINE_H__


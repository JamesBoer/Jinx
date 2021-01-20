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
		virtual bool IsFinished() = 0;
		virtual Variant GetReturnValue() const = 0;

	protected:
		virtual ~ICoroutine() {}
	};

	namespace Impl
	{
		class Script;
	}
	using CoroutinePtr = std::shared_ptr<ICoroutine>;
	CoroutinePtr CreateCoroutine(std::shared_ptr<Jinx::Impl::Script> script, RuntimeID functionID, const std::vector<Variant, Allocator<Variant>> & params);
}

#endif // JX_COROUTINE_H__


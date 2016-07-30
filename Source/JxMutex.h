/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_MUTEX_H__
#define JX_MUTEX_H__

namespace Jinx
{
	const size_t MutexDataSize = 64;

	// Platform-specific locking mechanism.  We don't use std::mutex because it uses new/delete internally,
	// and we'd prefer to control all allocations.
	class Mutex
	{
	public:
		Mutex();
		~Mutex();

		void lock();
		void unlock();

	private:
		uint8_t m_data[MutexDataSize];
	};

};




#endif // JX_MUTEX_H__


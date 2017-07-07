/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

using namespace Jinx;

#if defined(JINX_WINDOWS)
#include <SDKDDKVer.h>
#include <Windows.h>
#elif defined(JINX_OSX)
#include <libkern/OSAtomic.h>
#elif defined(JINX_LINUX)
#include<pthread.h>
#else
#include<pthread.h>
#endif


#if defined(JINX_WINDOWS)
static_assert(sizeof(CRITICAL_SECTION) <= MutexDataSize, "Data size must match or exceed CRITICAL_SECTION");
#elif defined(JINX_MACOS) || defined(JINX_LINUX)
static_assert(sizeof(pthread_mutex_t) <= MutexDataSize, "Data size must match or exceed pthread_mutex_t");
#else
assert(!"Platform-specific mutex not defined");
#endif

Mutex::Mutex()
{
#if defined(JINX_WINDOWS)
	InitializeCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(&m_data));
#elif defined(JINX_MACOS) || defined(JINX_LINUX)
	pthread_mutex_init(reinterpret_cast<pthread_mutex_t *>(&m_data), NULL);
#else
	assert(!"Platform-specific mutex not defined");
#endif
}

Mutex::~Mutex()
{
#if defined(JINX_WINDOWS)
	DeleteCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(&m_data));
#elif defined(JINX_MACOS) || defined(JINX_LINUX)
	pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t *>(&m_data));
#else
	assert(!"Platform-specific mutex not defined");
#endif
}

void Mutex::lock()
{
#if defined(JINX_WINDOWS)
	EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(&m_data));
#elif defined(JINX_MACOS) || defined(JINX_LINUX)
	pthread_mutex_lock(reinterpret_cast<pthread_mutex_t *>(&m_data));
#else
	assert(!"Platform-specific mutex not defined");
#endif
}

void Mutex::unlock()
{
#if defined(JINX_WINDOWS)
	LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(&m_data));
#elif defined(JINX_MACOS) || defined(JINX_LINUX)
	pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t *>(&m_data));
#else
	assert(!"Platform-specific mutex not defined");
#endif
}


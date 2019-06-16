/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx::Impl
{

	const unsigned BufferSize = 1024 * 4;

	struct Log
	{
		static inline bool enableLogging = true;
		static inline bool logSymbols = false;
		static inline bool logBytecode = false;
		static inline LogFn logFn = [](LogLevel, const char * logText) { printf("%s", logText); };
		static inline std::mutex logMutex;
	};

	inline_t void LogWrite(LogLevel level, const char * format, ...)
	{
		std::unique_lock<std::mutex> lock(Log::logMutex);
		if (!Log::enableLogging)
			return;
		va_list argptr;
		va_start(argptr, format);
		char buffer[BufferSize];
#if defined(JINX_WINDOWS)
		_vsnprintf_s(buffer, BufferSize, _TRUNCATE, format, argptr);
#else
		vsnprintf(buffer, BufferSize, format, argptr);
#endif
		Log::logFn(level, buffer);
		va_end(argptr);
	}

	inline_t void LogWriteLine(LogLevel level, const char * format, ...)
	{
		std::unique_lock<std::mutex> lock(Log::logMutex);
		if (!Log::enableLogging)
			return;
		va_list argptr;
		va_start(argptr, format);
		char buffer[BufferSize];
#if defined(JINX_WINDOWS)
		_vsnprintf_s(buffer, BufferSize, _TRUNCATE, format, argptr);
#else
		vsnprintf(buffer, BufferSize, format, argptr);
#endif
		size_t len = strlen(buffer);
		if (len < BufferSize - 2)
		{
			buffer[len] = '\n';
			buffer[len + 1] = 0;
		}
		Log::logFn(level, buffer);
		va_end(argptr);
	}

	inline_t void InitializeLogging(const GlobalParams & params)
	{
		Log::enableLogging = params.enableLogging;
		if (Log::enableLogging)
		{
			if (params.logFn)
				Log::logFn = params.logFn;
			Log::logSymbols = params.logSymbols;
			Log::logBytecode = params.logBytecode;
		}
	}

	inline_t bool IsLogSymbolsEnabled()
	{
		return Log::logSymbols;
	}

	inline_t bool IsLogBytecodeEnabled()
	{
		return Log::logBytecode;
	}

} // namespace Jinx::Impl


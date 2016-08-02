/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

using namespace Jinx;


const unsigned BufferSize = 1024 * 4;


static bool				s_enableLogging = true;
static bool				s_logSymbols = false;
static bool				s_logBytecode = false;
static LogFn			s_logFn = [](const char * logText) { printf("%s", logText); };
static std::mutex		s_mutex;


void Jinx::LogWrite(const char * format, ...)
{
	std::unique_lock<std::mutex> lock(s_mutex);
	if (!s_enableLogging)
		return;
	va_list argptr;
	va_start(argptr, format);
	char buffer[BufferSize];
#if defined(JINX_WINDOWS)
	_vsnprintf_s(buffer, BufferSize, _TRUNCATE, format, argptr);
#else
	vsnprintf(buffer, BufferSize, format, argptr);
#endif
	s_logFn(buffer);
	va_end(argptr);
}

void Jinx::LogWriteLine(const char * format, ...)
{
	std::unique_lock<std::mutex> lock(s_mutex);
	if (!s_enableLogging)
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
	s_logFn(buffer);
	va_end(argptr);
}

void Jinx::InitializeLogging(const GlobalParams & params)
{
	s_enableLogging = params.enableLogging;
	if (s_enableLogging)
	{
		if (params.logFn)
			s_logFn = params.logFn;
		s_logSymbols = params.logSymbols;
		s_logBytecode = params.logBytecode;
	}
}

bool Jinx::IsLogSymbolsEnabled()
{
	return s_logSymbols;
}

bool Jinx::IsLogBytecodeEnabled()
{
	return s_logBytecode;
}


/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_LOGGING_H__
#define JX_LOGGING_H__


namespace Jinx::Impl
{

	void InitializeLogging(const GlobalParams & params);

	bool IsLogSymbolsEnabled();
	bool IsLogBytecodeEnabled();

	void LogWrite(LogLevel level, const char * format, ...);
	void LogWriteLine(LogLevel level, const char * format, ...);

} // namespace Jinx::Impl

#endif // JX_LOGGING_H__

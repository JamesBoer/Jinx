/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_LOGGING_H__
#define JX_LOGGING_H__


namespace Jinx
{

	void InitializeLogging(const GlobalParams & params);
	
	bool IsLogSymbolsEnabled();
	bool IsLogBytecodeEnabled();

	void LogWrite(const char * format, ...);
	void LogWriteLine(const char * format, ...);
	
};

#endif // JX_LOGGING_H__

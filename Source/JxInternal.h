/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_INTERNAL_H__
#define JX_INTERNAL_H__

#include "Jinx.h"

#ifdef JINX_WINDOWS
#pragma warning(push)
#pragma warning(disable : 4530) // Silence warnings if exceptions are disabled
#endif

#include <cassert>
#include <cstdarg>
#include <mutex>
#include <algorithm>
#include <memory>
#include <string>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <vector>
#include <random>
#include <cctype>
#include <cinttypes>
#include <string.h>
#include <cstddef>
#include <atomic>
#include <locale>
#include <codecvt>
#include <climits>
#ifdef JINX_USE_FROM_CHARS
#include <charconv>
#else
#include <sstream>
#endif

#ifdef JINX_WINDOWS
#pragma warning(pop)
#endif

#include "JxLogging.h"
#include "JxCommon.h"
#include "JxUnicode.h"
#include "JxUnicodeCaseFolding.h"
#include "JxGuid.h"
#include "JxConversion.h"
#include "JxSerialize.h"
#include "JxPropertyName.h"
#include "JxLexer.h"
#include "JxHash.h"
#include "JxFunctionSignature.h"
#include "JxFunctionDefinition.h"
#include "JxLibrary.h"
#include "JxVariableStackFrame.h"
#include "JxParser.h"
#include "JxScript.h"
#include "JxRuntime.h"
#include "JxLibCore.h"

#endif // JX_INTERNAL_H__

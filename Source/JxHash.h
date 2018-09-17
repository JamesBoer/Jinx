/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_HASH_H__
#define JX_HASH_H__


namespace Jinx::Impl
{

	uint64_t GetHash(const void * data, size_t len);

} // namespace Jinx::Impl

#endif // JX_HASH_H__



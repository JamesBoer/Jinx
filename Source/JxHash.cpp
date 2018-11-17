// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. Appleby disclaims copyright to this source code.
// https://en.wikipedia.org/wiki/MurmurHash

// All code in this source file is compatible with the MIT license.

#include "JxInternal.h"

namespace Jinx::Impl
{

	// This hash variant is supposedly platform and endian independent.
	inline_t uint32_t MurmurHashNeutral2(const void * key, int len, uint32_t seed)
	{
		const uint32_t m = 0x5bd1e995;
		const int r = 24;

		uint32_t h = seed ^ len;

		const unsigned char * data = (const unsigned char *)key;

		while (len >= 4)
		{
			uint32_t k;

			k = data[0];
			k |= data[1] << 8;
			k |= data[2] << 16;
			k |= data[3] << 24;

			k *= m;
			k ^= k >> r;
			k *= m;

			h *= m;
			h ^= k;

			data += 4;
			len -= 4;
		}

		switch (len)
		{
		case 3: h ^= data[2] << 16;
		[[fallthrough]];
		case 2: h ^= data[1] << 8;
		[[fallthrough]];
		case 1: h ^= data[0];
			h *= m;
		};

		h ^= h >> 13;
		h *= m;
		h ^= h >> 15;

		return h;
	}

	// Use two endian-neutral 32-bit hashes combined to a 64-bit hash to reduce odds of random collisions
	inline_t uint64_t GetHash(const void * data, size_t len)
	{
		uint32_t hash1 = MurmurHashNeutral2(data, static_cast<int>(len), 0xF835E195);
		uint32_t hash2 = MurmurHashNeutral2(data, static_cast<int>(len), 0x5C285D21);
		uint64_t hash = (static_cast<uint64_t>(hash2) << 32) | static_cast<uint64_t>(hash1);
		assert(hash != InvalidID);
		return hash;
	}

} // namespace Jinx::Impl



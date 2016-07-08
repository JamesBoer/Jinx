// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. Appleby disclaims copyright to this source code.
// https://en.wikipedia.org/wiki/MurmurHash

// Jenkins one at a time hash was written by Bob Jenkins, and has been
// placed in the public domain.
// https://en.wikipedia.org/wiki/Jenkins_hash_function

// All code in this source file is compatible with the MIT license.

#include "JxInternal.h"

using namespace Jinx;

// This hash variant is supposedly platform and endian independent.
uint32_t MurmurHashNeutral2(const void * key, int len, uint32_t seed)
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
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
		h *= m;
	};

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

uint32_t jenkins_one_at_a_time_hash(const uint8_t * key, size_t len)
{
	uint32_t hash, i;
	for (hash = i = 0; i < len; ++i)
	{
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

// Use two 32-bit hashes which are theoretically endian-neutral to reduce odds of collisions
uint64_t Jinx::GetHash(const uint8_t * data, uint32_t len)
{
	uint32_t hash1 = MurmurHashNeutral2(data, len, 0xF835E195);
	uint32_t hash2 = jenkins_one_at_a_time_hash(data, len);
	uint64_t hash = (static_cast<uint64_t>(hash2) << 32) | static_cast<uint64_t>(hash1);
	assert(hash != InvalidID);
	return hash;
}


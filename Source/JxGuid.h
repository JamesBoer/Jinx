/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_GUID_H__
#define JX_GUID_H__

namespace Jinx
{

	struct Guid
	{
		uint32_t data1;
		uint16_t  data2;
		uint16_t  data3;
		uint8_t  data4[8];
	};

	const Guid NullGuid = { 0, 0, 0,{ 0, 0, 0, 0, 0, 0, 0, 0 } };

	inline bool operator < (const Guid & left, const Guid & right)
	{
		return (memcmp(&left, &right, sizeof(Guid)) < 0) ? true : false;
	}

	inline bool operator == (const Guid & left, const Guid & right)
	{
		return (memcmp(&left, &right, sizeof(Guid)) == 0) ? true : false;
	}

	inline bool operator != (const Guid & left, const Guid & right)
	{
		return !(left == right);
	}

}




#endif // JX_GUID_H__

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"


namespace Jinx
{
	inline_t CollectionPtr CreateCollection()
	{
		return std::allocate_shared<Collection>(Allocator<Collection>());
	}

} // namespace Jinx


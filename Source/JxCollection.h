/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_COLLECTION_H__
#define JX_COLLECTION_H__

/*! \file */

/*! \namespace */
namespace Jinx
{
	class Variant;
	using Collection = std::map<Variant, Variant, std::less<Variant>, Allocator<std::pair<const Variant, Variant>>>;
	using CollectionPtr = std::shared_ptr<Collection>;
	using CollectionItr = Collection::iterator;
	using CollectionItrPair = std::pair<CollectionItr, CollectionPtr>;
	CollectionPtr CreateCollection();
}

#endif // JX_COLLECTION_H__


/*
 * Copyright (c) 2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <functional>
#include <utility>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/optional.hpp>

namespace Swift {


/**
 * The \ref LRUCache templaged class implements a lookup cache which removes
 * the least recently used cached item from the cache, if the cache size hits
 * the \ref MAX_SIZE limit.
 *
 * An example use is a cache for entity capabilities hash to DiscoInfo.
 */
template <typename KEY_TYPE, typename VALUE_TYPE, size_t MAX_SIZE>
class LRUCache {
public:
    using cacheMissFunction = std::function<boost::optional<VALUE_TYPE>(const KEY_TYPE& )>;

public:
    /**
     * Inserts the key/value pair in the front of the cache. If the \ref key
     * already exists in the cache, it is moved to the front instead. If
     * afterwards, the cahe size exceeds the \ref MAX_SIZE limit, the least
     * recently item is removed from the cache.
     */
    void insert(const KEY_TYPE& key, VALUE_TYPE value) {
        auto pushResult = cache.push_front(entry_t(key, value));
        if (!pushResult.second) {
            cache.relocate(cache.begin(), pushResult.first);
        }
        else if (cache.size() > MAX_SIZE) {
          cache.pop_back();
        }
    }

    /**
     * Looks up a cache entry based on the provided \ref key and moves it back
     * to the front of the cache. If there is no cache entry for the provided
     * \ref key, an uninitialized \ref boost::optional is returned.
     * If the optional \ref missFunction is provided, it is called on a cache miss.
     * If the \ref missFunction returns an initialized \ref boost::optional, the
     * value is inserted in the cache.
     */
    boost::optional<VALUE_TYPE> get(const KEY_TYPE& key, cacheMissFunction missFunction = cacheMissFunction()) {
        boost::optional<VALUE_TYPE> cachedValue;
        auto cacheItemIterator = boost::multi_index::get<1>(cache).find(key);
        if (cacheItemIterator != boost::multi_index::get<1>(cache).end()) {
            cachedValue = cacheItemIterator->second;
            cache.relocate(cache.begin(), cache.iterator_to(*cacheItemIterator));
        }
        else if (missFunction && (cachedValue = missFunction(key))) {
            insert(key, cachedValue.get());
        }
        return cachedValue;
    }

private:
    using entry_t =  std::pair<KEY_TYPE, VALUE_TYPE>;

private:
    boost::multi_index_container< entry_t, boost::multi_index::indexed_by< boost::multi_index::sequenced<>, boost::multi_index::hashed_unique<
    BOOST_MULTI_INDEX_MEMBER(entry_t, KEY_TYPE, first)> > > cache;
};

}

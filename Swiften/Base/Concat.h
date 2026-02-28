/*
 * Copyright (c) 2011-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <algorithm>
#include <type_traits>

namespace Swift {
    template<typename V1, typename V2, typename... Rest>
    std::size_t getReserveAmount(V1 const& v1, V2 const& v2, Rest const&... rest) SWIFTEN_NOEXCEPT
    {
        // Total size of the concatenated output is equal to sum of sizes of all the containers that were passed in.
        // Two first can be accessed directly:
        std::size_t sizeSum = v1.size() + v2.size();
        // While the rest needs to be calculated in parameter pack expansion:
        // http://stackoverflow.com/a/25683817/61574
        // Pack expansion can occur inside a braced-init-list - dummy array is used for that (with side effect of accumulating the size)
        int dummy[] = {0, ((sizeSum += rest.size()), void(), 0)...};

        (void)dummy; // dummy is unused - suppress compiler warning
        return sizeSum;
}

    template<typename V1, typename V2, typename... Rest>
    typename std::decay<V1>::type concat(V1&& v1, V2&& v2, Rest&&... rest) {
        // Create the output container and reserve the required amount of space
        typename std::decay<V1>::type v;
        v.reserve(getReserveAmount(v1, v2, rest...));
        // Insert the elements from input containers to output. Directly for the first two:
        v.insert(v.cend(), v1.cbegin(), v1.cend());
        v.insert(v.cend(), v2.cbegin(), v2.cend());
        // Use pack expansion for the others:
        int dummy[] = {0, ((v.insert(v.cend(), rest.cbegin(), rest.cend())), void(), 0 )...};
        (void)dummy;
        return v;
    }
}

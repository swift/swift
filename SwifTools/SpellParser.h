/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>

namespace Swift {
    enum token_ids
    {
        ID_WWW = 1,
        ID_HTTP = 2,
        ID_WORD = 3,
        ID_CHAR = 4
    };

    typedef boost::tuple<size_t, size_t> PositionPair;
    typedef std::vector<PositionPair > PositionPairList;

    class SpellParser{
        public:
            void check(const std::string& fragment, PositionPairList& wordPositions);
    };
}

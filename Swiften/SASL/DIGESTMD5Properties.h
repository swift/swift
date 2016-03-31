/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
    class SWIFTEN_API DIGESTMD5Properties {
        public:
            DIGESTMD5Properties();

            boost::optional<std::string> getValue(const std::string& key) const;

            void setValue(const std::string& key, const std::string& value);

            ByteArray serialize() const;

            static DIGESTMD5Properties parse(const ByteArray&);

        private:
            static bool isQuoted(const std::string& property);

        private:
            typedef std::multimap<std::string, ByteArray> DIGESTMD5PropertiesMap;
            DIGESTMD5PropertiesMap properties;
    };
}

/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
    class SWIFTEN_API IDNConverter {
        public:
            virtual ~IDNConverter();

            enum StringPrepProfile {
                NamePrep,
                XMPPNodePrep,
                XMPPResourcePrep,
                SASLPrep
            };

            virtual std::string getStringPrepared(const std::string& s, StringPrepProfile profile) = 0;
            virtual SafeByteArray getStringPrepared(const SafeByteArray& s, StringPrepProfile profile) = 0;

            // Thread-safe
            virtual boost::optional<std::string> getIDNAEncoded(const std::string& s) = 0;
    };
}

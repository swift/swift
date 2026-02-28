/*
 * Copyright (c) 2012-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/IDN/IDNConverter.h>

namespace Swift {
    class SWIFTEN_API ICUConverter : public IDNConverter {
        public:
            virtual std::string getStringPrepared(const std::string& s, StringPrepProfile profile) override;
            virtual SafeByteArray getStringPrepared(const SafeByteArray& s, StringPrepProfile profile) override;

            virtual boost::optional<std::string> getIDNAEncoded(const std::string& s) override;
    };
}

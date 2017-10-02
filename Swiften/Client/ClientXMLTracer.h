/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Client/CoreClient.h>
#include <Swiften/Client/XMLBeautifier.h>

namespace Swift {
    class SWIFTEN_API ClientXMLTracer {
        public:
            ClientXMLTracer(CoreClient* client, bool bosh = false);

        private:
            void printData(char direction, const SafeByteArray& data);
            void printLine(char c);

        private:
            std::unique_ptr<XMLBeautifier> beautifier_;
            bool bosh_;
            boost::signals2::scoped_connection onDataReadConnection_;
            boost::signals2::scoped_connection onDataWrittenConnection_;
    };
}

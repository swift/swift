/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Client/CoreClient.h>
#include <Swiften/Client/XMLBeautifier.h>

namespace Swift {
    class SWIFTEN_API ClientXMLTracer {
        public:
            ClientXMLTracer(CoreClient* client, bool bosh = false);
            ~ClientXMLTracer();

        private:
            void printData(char direction, const SafeByteArray& data);
            void printLine(char c);

        private:
            XMLBeautifier *beautifier;
            bool bosh;
            boost::bsignals::scoped_connection onDataReadConnection;
            boost::bsignals::scoped_connection onDataWrittenConnection;
    };
}

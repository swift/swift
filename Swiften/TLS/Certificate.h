/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
    class CryptoProvider;

    class SWIFTEN_API Certificate {
        public:
            typedef std::shared_ptr<Certificate> ref;

            virtual ~Certificate();

            /**
             * Returns the textual representation of the full Subject
             * name.
             */
            virtual std::string getSubjectName() const = 0;

            virtual std::vector<std::string> getCommonNames() const = 0;
            virtual std::vector<std::string> getSRVNames() const = 0;
            virtual std::vector<std::string> getDNSNames() const = 0;
            virtual std::vector<std::string> getXMPPAddresses() const = 0;

            virtual ByteArray toDER() const = 0;

            static std::string getSHA1Fingerprint(Certificate::ref, CryptoProvider* crypto);

        protected:
            static const char* ID_ON_XMPPADDR_OID;
            static const char* ID_ON_DNSSRV_OID;

    };
}

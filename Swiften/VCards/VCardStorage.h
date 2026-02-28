/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/VCard.h>

namespace Swift {
    class JID;
    class CryptoProvider;

    class SWIFTEN_API VCardStorage {
        public:
            VCardStorage(CryptoProvider*);
            virtual ~VCardStorage();

            virtual VCard::ref getVCard(const JID& jid) const = 0;
            virtual boost::posix_time::ptime getVCardWriteTime(const JID& jid) const = 0;
            virtual void setVCard(const JID&, VCard::ref) = 0;

            virtual std::string getPhotoHash(const JID&) const;

        private:
            CryptoProvider* crypto;
    };
}

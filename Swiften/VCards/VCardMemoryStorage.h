/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/JID/JID.h>
#include <Swiften/VCards/VCardStorage.h>

namespace Swift {
    class SWIFTEN_API VCardMemoryStorage : public VCardStorage {
        public:
            VCardMemoryStorage(CryptoProvider* crypto) : VCardStorage(crypto) {}

            virtual VCard::ref getVCard(const JID& jid) const {
                VCardMap::const_iterator i = vcards.find(jid);
                if (i != vcards.end()) {
                    return i->second;
                }
                else {
                    return VCard::ref();
                }
            }

            virtual boost::posix_time::ptime getVCardWriteTime(const JID& jid) const {
                if (vcardWriteTimes.find(jid) == vcardWriteTimes.end()) {
                    return boost::posix_time::ptime();
                }
                else {
                    return vcardWriteTimes.at(jid);
                }
            }

            virtual void setVCard(const JID& jid, VCard::ref v) {
                vcards[jid] = v;
                vcardWriteTimes[jid] = boost::posix_time::second_clock::universal_time();
            }

        private:
            typedef std::map<JID, VCard::ref> VCardMap;
            typedef std::map<JID, boost::posix_time::ptime> VCardWriteTimeMap;
            VCardMap vcards;
            VCardWriteTimeMap vcardWriteTimes;
    };
}

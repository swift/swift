/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Client/Storages.h>

namespace Swift {
    class VCardMemoryStorage;
    class CryptoProvider;

    /**
     * An implementation of Storages for storing all
     * controller data in memory.
     */
    class SWIFTEN_API MemoryStorages : public Storages {
        public:
            MemoryStorages(CryptoProvider*);
            virtual ~MemoryStorages();

            virtual VCardStorage* getVCardStorage() const;
            virtual AvatarStorage* getAvatarStorage() const;
            virtual CapsStorage* getCapsStorage() const;
            virtual RosterStorage* getRosterStorage() const;
            virtual HistoryStorage* getHistoryStorage() const;

        private:
            VCardMemoryStorage* vcardStorage;
            AvatarStorage* avatarStorage;
            CapsStorage* capsStorage;
            RosterStorage* rosterStorage;
            HistoryStorage* historyStorage;
    };
}

/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/filesystem/path.hpp>

#include <Swiften/Client/Storages.h>

namespace Swift {
    class VCardFileStorage;
    class AvatarFileStorage;
    class CapsFileStorage;
    class RosterFileStorage;
    class HistoryStorage;
    class JID;
    class CryptoProvider;

    /**
     * A storages implementation that stores all controller data on disk.
     */
    class FileStorages : public Storages {
        public:
            /**
             * Creates the storages interface.
             *
             * All data will be stored relative to a base directory, and
             * for some controllers, in a subdirectory for the given profile.
             * The data is stored in the following places:
             * - Avatars: $basedir/avatars
             * - VCards:  $basedir/$profile/vcards
             * - Entity capabilities: $basedir/caps
             *
             * \param baseDir the base dir to store data relative to
             * \param jid the subdir in which profile-specific data will be stored.
             *   The bare JID will be used as the subdir name.
             */
            FileStorages(const boost::filesystem::path& baseDir, const JID& jid, CryptoProvider*);
            ~FileStorages();

            virtual VCardStorage* getVCardStorage() const;
            virtual AvatarStorage* getAvatarStorage() const;
            virtual CapsStorage* getCapsStorage() const;
            virtual RosterStorage* getRosterStorage() const;
            virtual HistoryStorage* getHistoryStorage() const;

        private:
            VCardFileStorage* vcardStorage;
            AvatarFileStorage* avatarStorage;
            CapsFileStorage* capsStorage;
            RosterFileStorage* rosterStorage;
            HistoryStorage* historyStorage;
    };
}

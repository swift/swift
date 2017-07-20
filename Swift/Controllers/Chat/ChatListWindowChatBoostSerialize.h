/*
 * Copyright (c) 2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/version.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include <Swift/Controllers/UIInterfaces/ChatListWindow.h>

BOOST_CLASS_VERSION(Swift::ChatListWindow::Chat, 3)
namespace Swift {
    const boost::archive::library_version_type BoostArchiveSkipVersion(15);
}

namespace boost {
    namespace serialization {
        template<class Archive> void save(Archive& ar, const Swift::JID& jid, const unsigned int /*version*/) {
            std::string jidStr = jid.toString();
            ar << jidStr;
        }

        template<class Archive> void load(Archive& ar, Swift::JID& jid, const unsigned int /*version*/) {
            std::string stringJID;
            ar >> stringJID;
            jid = Swift::JID(stringJID);
        }

        template<class Archive> inline void serialize(Archive& ar, Swift::JID& t, const unsigned int file_version) {
            split_free(ar, t, file_version);
        }

        template<class Archive> void serialize(Archive& ar, Swift::ChatListWindow::Chat& chat, const unsigned int version) {
            auto archiveLibraryVersion = boost::archive::BOOST_ARCHIVE_VERSION();
            int archiveVersion = 0;
            archive::text_oarchive* outputStream = dynamic_cast<archive::text_oarchive*>(&ar);
            if (outputStream) {
                archiveVersion = outputStream->get_library_version();
            }
            archive::text_iarchive* inputStream = dynamic_cast<archive::text_iarchive*>(&ar);
            if (inputStream) {
                archiveVersion = inputStream->get_library_version();
                //Due to https://svn.boost.org/trac10/ticket/13050 the password field may fail to load and crash the client. Therefore we skip loading the values from previous versions.
                if (archiveLibraryVersion == Swift::BoostArchiveSkipVersion && archiveLibraryVersion > archiveVersion) {
                    return;
                }
            }
            ar & chat.jid;
            ar & chat.chatName;
            ar & chat.activity;
            ar & chat.isMUC;
            ar & chat.nick;
            ar & chat.impromptuJIDs;
            if (version > 0) {
                if (outputStream && archiveLibraryVersion == Swift::BoostArchiveSkipVersion) {
                    //The bug does not affect the case boost::optional doesn't have a value. Therefore we store always that value, to avoid problem on future launches of the client.
                    boost::optional<std::string> empty;
                    ar & empty;
                }
                else {
                    ar & chat.password;
                }
            }
            if (version > 1) {
                ar & chat.inviteesNames;
            }
        }
    }
}

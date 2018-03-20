/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <string>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/optional.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/optional.hpp>

namespace Swift {
    class HighlightAction {
        public:
            void setFrontColor(const boost::optional<std::string>& frontColor);
            boost::optional<std::string> getFrontColor() const;

            void setBackColor(const boost::optional<std::string>& backColor);
            boost::optional<std::string> getBackColor() const;

            void setSoundFilePath(const boost::optional<std::string>& soundFilePath);
            boost::optional<std::string> getSoundFilePath() const;

            void setSystemNotificationEnabled(bool systemNotificationEnabled);
            bool isSystemNotificationEnabled() const;

            // @return returns true if the HighlightAction would result in no
            // noticable action to the user.
            bool isEmpty() const;

        private:
            friend class boost::serialization::access;
            template<class Archive> void serialize(Archive & ar, const unsigned int version);

        private:
            // Highlight color.
            boost::optional<std::string> frontColor_;
            boost::optional<std::string> backColor_;

            // Audio notification.
            boost::optional<std::string> soundFilePath_;

            // macOS Notification Center or similar.
            bool systemNotificaitonEnabled_ = false;
    };

    bool operator ==(const HighlightAction& a, const HighlightAction& b);
    bool operator !=(const HighlightAction& a, const HighlightAction& b);


    template<class Archive>
    void HighlightAction::serialize(Archive& ar, const unsigned int version) {
        auto inputStream = dynamic_cast<boost::archive::text_iarchive*>(&ar);
        auto outputStream = dynamic_cast<boost::archive::text_oarchive*>(&ar);

        if (version == 0) {
            if (inputStream) {
                const boost::archive::library_version_type BoostArchiveSkipVersion(15);
                auto archiveLibraryVersion = boost::archive::BOOST_ARCHIVE_VERSION();
                //Due to https://svn.boost.org/trac10/ticket/13050 the boost::optional fields may fail to load and crash the client. Therefore we skip loading the values from previous versions.
                if (archiveLibraryVersion == BoostArchiveSkipVersion && archiveLibraryVersion > inputStream->get_library_version()) {
                    return;
                }
            }
            ar & frontColor_;
            ar & backColor_;
            ar & soundFilePath_;
            ar & systemNotificaitonEnabled_;
        }
        else if (version == 1) {
            //Using a map instead of optional values that may cause a problems when serialised with boost::archive 15 version
            std::map<std::string, std::string> properties;
            if (outputStream) {
                if (frontColor_.is_initialized()) {
                    properties["frontColor"] = frontColor_.get();
                }
                if (backColor_.is_initialized()) {
                    properties["backColor"] = backColor_.get();
                }
                if (soundFilePath_.is_initialized()) {
                    properties["soundFilePath"] = soundFilePath_.get();
                }
            }
            ar & properties;
            ar & systemNotificaitonEnabled_;
            if (inputStream) {
                if (properties.find("frontColor") != properties.end()) {
                    frontColor_ = properties["frontColor"];
                }
                if (properties.find("backColor") != properties.end()) {
                    backColor_ = properties["backColor"];
                }
                if (properties.find("soundFilePath") != properties.end()) {
                    soundFilePath_ = properties["soundFilePath"];
                }
            }
        }
    }
}

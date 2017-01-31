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

#include <string>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/optional.hpp>
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
    void HighlightAction::serialize(Archive& ar, const unsigned int /*version*/) {
        ar & frontColor_;
        ar & backColor_;
        ar & soundFilePath_;
        ar & systemNotificaitonEnabled_;
    }

}

/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <Swift/Controllers/Highlighting/HighlightAction.h>

namespace Swift {

class HighlightConfiguration {
public:
    class ContactHighlight {
    public:
        friend class boost::serialization::access;
        template<class Archive> void serialize(Archive & ar, const unsigned int version);

    public:
        std::string name;
        HighlightAction action;
    };

    class KeywordHightlight {
    public:
        friend class boost::serialization::access;
        template<class Archive> void serialize(Archive & ar, const unsigned int version);

    public:
        std::string keyword;
        bool matchCaseSensitive = false;
        HighlightAction action;
    };

    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive & ar, const unsigned int version);

public:
    std::vector<KeywordHightlight> keywordHighlights;
    std::vector<ContactHighlight> contactHighlights;
    HighlightAction ownMentionAction;
    bool playSoundOnIncomingDirectMessages = false;
    bool showNotificationOnIncomingDirectMessages = false;
    bool playSoundOnIncomingGroupchatMessages = false;
    bool showNotificationOnIncomingGroupchatMessages = false;
};

bool operator ==(HighlightConfiguration const& a, HighlightConfiguration const& b);

template<class Archive>
void HighlightConfiguration::ContactHighlight::serialize(Archive& ar, const unsigned int /*version*/) {
    ar & name;
    ar & action;
}

template<class Archive>
void HighlightConfiguration::KeywordHightlight::serialize(Archive& ar, const unsigned int /*version*/) {
    ar & keyword;
    ar & matchCaseSensitive;
    ar & action;
}

template<class Archive>
void HighlightConfiguration::serialize(Archive& ar, const unsigned int /*version*/) {
    ar & keywordHighlights;
    ar & contactHighlights;
    ar & ownMentionAction;
    ar & playSoundOnIncomingDirectMessages;
    ar & showNotificationOnIncomingDirectMessages;
    ar & playSoundOnIncomingGroupchatMessages;
    ar & showNotificationOnIncomingGroupchatMessages;
}

}

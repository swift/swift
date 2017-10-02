/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Elements/DiscoInfo.h>

#include <algorithm>

namespace Swift {

const std::string DiscoInfo::ChatStatesFeature = std::string("http://jabber.org/protocol/chatstates");
const std::string DiscoInfo::ClientStatesFeature = std::string("urn:xmpp:csi:0");
const std::string DiscoInfo::SecurityLabelsFeature = std::string("urn:xmpp:sec-label:0");
const std::string DiscoInfo::SecurityLabelsCatalogFeature = std::string("urn:xmpp:sec-label:catalog:2");
const std::string DiscoInfo::JabberSearchFeature = std::string("jabber:iq:search");
const std::string DiscoInfo::CommandsFeature = std::string("http://jabber.org/protocol/commands");
const std::string DiscoInfo::MessageCorrectionFeature = std::string("urn:xmpp:message-correct:0");
const std::string DiscoInfo::JingleFeature = std::string("urn:xmpp:jingle:1");
const std::string DiscoInfo::JingleFTFeature = std::string("urn:xmpp:jingle:apps:file-transfer:4");
const std::string DiscoInfo::JingleTransportsIBBFeature = std::string("urn:xmpp:jingle:transports:ibb:1");
const std::string DiscoInfo::JingleTransportsS5BFeature = std::string("urn:xmpp:jingle:transports:s5b:1");
const std::string DiscoInfo::Bytestream = std::string("http://jabber.org/protocol/bytestreams");
const std::string DiscoInfo::MessageDeliveryReceiptsFeature = std::string("urn:xmpp:receipts");
const std::string DiscoInfo::WhiteboardFeature = std::string("http://swift.im/whiteboard");
const std::string DiscoInfo::BlockingCommandFeature = std::string("urn:xmpp:blocking");
const std::string DiscoInfo::MessageCarbonsFeature = std::string("urn:xmpp:carbons:2");

bool DiscoInfo::Identity::operator<(const Identity& other) const {
    if (category_ == other.category_) {
        if (type_ == other.type_) {
            if (lang_ == other.lang_) {
                return name_ < other.name_;
            }
            else {
                return lang_ < other.lang_;
            }
        }
        else {
            return type_ < other.type_;
        }
    }
    else {
        return category_ < other.category_;
    }
}

bool DiscoInfo::hasFeature(const std::string& feature) const {
    return std::find(features_.begin(), features_.end(), feature) != features_.end();
}

}

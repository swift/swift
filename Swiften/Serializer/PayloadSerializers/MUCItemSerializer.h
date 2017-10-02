/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Elements/MUCItem.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {
    class MUCItemSerializer {
        public:
            static std::string affiliationToString(MUCOccupant::Affiliation affiliation) {
                std::string result;
                switch (affiliation) {
                case MUCOccupant::Owner: result = "owner"; break;
                case MUCOccupant::Admin: result = "admin"; break;
                case MUCOccupant::Member: result = "member"; break;
                case MUCOccupant::Outcast: result = "outcast"; break;
                case MUCOccupant::NoAffiliation: result = "none"; break;
                }
                return result;
            }

            static std::string roleToString(MUCOccupant::Role role) {
                std::string result;
                switch (role) {
                case MUCOccupant::Moderator: result = "moderator"; break;
                case MUCOccupant::NoRole: result = "none"; break;
                case MUCOccupant::Participant: result = "participant"; break;
                case MUCOccupant::Visitor: result = "visitor"; break;
                }
                return result;

            }

            static std::shared_ptr<XMLElement> itemToElement(const MUCItem& item) {
                std::shared_ptr<XMLElement> itemElement(new XMLElement("item"));
                if (item.affiliation) {
                    itemElement->setAttribute("affiliation", affiliationToString(item.affiliation.get()));
                }
                if (item.role) {
                    itemElement->setAttribute("role", roleToString(item.role.get()));
                }
                if (item.realJID) {
                    itemElement->setAttribute("jid", item.realJID.get());
                }
                if (item.nick) {
                    itemElement->setAttribute("nick", item.nick.get());
                }
                if (item.actor) {
                    std::shared_ptr<XMLElement> actorElement(new XMLElement("actor"));
                    actorElement->setAttribute("jid", item.actor->toString());
                    itemElement->addNode(actorElement);
                }
                if (item.reason) {
                    std::shared_ptr<XMLElement> reasonElement(new XMLElement("reason"));
                    reasonElement->addNode(std::make_shared<XMLTextNode>(*item.reason));
                    itemElement->addNode(reasonElement);
                }
                return itemElement;
            }
    };
}

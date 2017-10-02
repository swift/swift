/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Client/StanzaChannel.h>

namespace Swift {
    class DummyStanzaChannel : public StanzaChannel {
        public:
            DummyStanzaChannel() {}

            virtual void sendStanza(std::shared_ptr<Stanza> stanza) {
                sentStanzas.push_back(stanza);
            }

            void setAvailable(bool available) {
                available_ = available;
                onAvailableChanged(available);
            }

            virtual void sendIQ(std::shared_ptr<IQ> iq) {
                sentStanzas.push_back(iq);
            }

            virtual void sendMessage(std::shared_ptr<Message> message) {
                sentStanzas.push_back(message);
            }

            virtual void sendPresence(std::shared_ptr<Presence> presence) {
                sentStanzas.push_back(presence);
            }

            virtual std::string getNewIQID() {
                std::string id = "test-id";
                if (uniqueIDs_) {
                    id += "-" + std::to_string(idCounter_++);
                }
                return id;
            }

            virtual bool isAvailable() const {
                return available_;
            }

            virtual bool getStreamManagementEnabled() const {
                return false;
            }

            template<typename T> bool isRequestAtIndex(size_t index, const JID& jid, IQ::Type type) {
                if (index >= sentStanzas.size()) {
                    return false;
                }
                std::shared_ptr<IQ> iqStanza = std::dynamic_pointer_cast<IQ>(sentStanzas[index]);
                return iqStanza && iqStanza->getType() == type && iqStanza->getTo() == jid && iqStanza->getPayload<T>();
            }

            bool isResultAtIndex(size_t index, const std::string& id) {
                if (index >= sentStanzas.size()) {
                    return false;
                }
                std::shared_ptr<IQ> iqStanza = std::dynamic_pointer_cast<IQ>(sentStanzas[index]);
                return iqStanza && iqStanza->getType() == IQ::Result && iqStanza->getID() == id;
            }

            bool isErrorAtIndex(size_t index, const std::string& id) {
                if (index >= sentStanzas.size()) {
                    return false;
                }
                std::shared_ptr<IQ> iqStanza = std::dynamic_pointer_cast<IQ>(sentStanzas[index]);
                return iqStanza && iqStanza->getType() == IQ::Error && iqStanza->getID() == id;
            }

            template<typename T> std::shared_ptr<T> getStanzaAtIndex(size_t index) {
                if (sentStanzas.size() <= index) {
                    return std::shared_ptr<T>();
                }
                return std::dynamic_pointer_cast<T>(sentStanzas[index]);
            }

            template<typename T> size_t countSentStanzaOfType() {
                size_t count = 0;
                for (auto& stanza : sentStanzas) {
                    if (std::dynamic_pointer_cast<T>(stanza)) {
                        count++;
                    }
                }
                return count;
            }

            std::vector<Certificate::ref> getPeerCertificateChain() const {
                return std::vector<Certificate::ref>();
            }

            std::vector<std::shared_ptr<Stanza> > sentStanzas;
            bool available_ = true;
            bool uniqueIDs_ = false;
            unsigned int idCounter_ = 0;
    };
}

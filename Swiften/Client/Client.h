/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Client/CoreClient.h>

namespace Swift {
    class AvatarManager;
    class BlindCertificateTrustChecker;
    class CapsManager;
    class ClientBlockListManager;
    class ClientDiscoManager;
    class DirectedPresenceSender;
    class EntityCapsManager;
    class EntityCapsProvider;
    class FileTransferManager;
    class FileTransferManager;
    class JingleSessionManager;
    class MIXRegistry;
    class MUCManager;
    class MUCRegistry;
    class MemoryStorages;
    class NickManager;
    class NickResolver;
    class PresenceOracle;
    class PresenceSender;
    class PubSubManager;
    class SafeString;
    class SoftwareVersionResponder;
    class StanzaChannelPresenceSender;
    class Storages;
    class SubscriptionManager;
    class VCardManager;
    class WhiteboardSessionManager;
    class XMPPRoster;
    class XMPPRosterController;
    class XMPPRosterImpl;

    /**
     * Provides the core functionality for writing XMPP client software.
     *
     * Besides connecting to an XMPP server, this class also provides interfaces for
     * performing most tasks on the XMPP network.
     */
    class SWIFTEN_API Client : public CoreClient {
        public:
            /**
             * Constructs a client for the given JID with the given password.
             *
             * \param jid The JID to be used for the login. If only a bare
             *    JID is give the server will be asked to generate a resource.
             * \param password The password used for login given as a \ref SafeString.
             * \param networkFactories The network factories that are used
             *    to handle any network related tasks.
             * \param storages The interfaces for storing cache information etc. If
             *    this is NULL,
             *    all data will be stored in memory (and be lost on shutdown)
             */
            Client(const JID& jid, const SafeString& password, NetworkFactories* networkFactories, Storages* storages = nullptr);
            virtual ~Client();


            /**
             * Sets the software version of the client.
             *
             * This will be used to respond to version queries from other entities.
             */
            void setSoftwareVersion(const std::string& name, const std::string& version, const std::string& os = "");

            /**
             * Returns a representation of the roster.
             *
             * The roster is initially empty. To populate it, call requestRoster(),
             * which will request the roster from the server. When the roster has
             * been requested, it will also be kept up to date when it is updated on
             * the server side.
             *
             * This pointer remains the same across the lifetime of Client. All
             * changes to the roster (e.g. after the initial roster request, or after
             * subsequent roster updates) are notified through the XMPPRoster's
             * signals.
             *
             * \see requestRoster()
             */
            XMPPRoster* getRoster() const;

            /**
             * Requests the roster from the server.
             *
             * \see getRoster()
             */
            void requestRoster(bool includeMIX = false);

            /**
             * Returns the last received presence for the given (full) JID.
             */
            std::shared_ptr<Presence> getLastPresence(const JID& jid) const;

            /**
             * Returns the presence with the highest priority received for the given JID.
             */
            std::shared_ptr<Presence> getHighestPriorityPresence(const JID& bareJID) const;

            PresenceOracle* getPresenceOracle() const {
                return presenceOracle;
            }

            PresenceSender* getPresenceSender() const;

            MIXRegistry* getMIXRegistry() const {
                return mixRegistry.get();
            }

            MUCManager* getMUCManager() const {
                return mucManager;
            }

            MUCRegistry* getMUCRegistry() const {
                return mucRegistry;
            }

            VCardManager* getVCardManager() const {
                return vcardManager;
            }

            AvatarManager* getAvatarManager() const {
                return avatarManager;
            }

            EntityCapsProvider* getEntityCapsProvider() const;

            NickManager* getNickManager() const;

            NickResolver* getNickResolver() const {
                return nickResolver;
            }

            SubscriptionManager* getSubscriptionManager() const {
                return subscriptionManager;
            }

            ClientDiscoManager* getDiscoManager() const {
                return discoManager;
            }

            ClientBlockListManager* getClientBlockListManager() const {
                return blockListManager;
            }

            /**
             * Returns a FileTransferManager for the client. This is only available after the onConnected
             * signal has been fired.
             *
             * WARNING: File transfer will only work if Swiften is built in 'experimental' mode.
             */
            FileTransferManager* getFileTransferManager() const;

            /**
             * Configures the client to always trust a non-validating
             * TLS certificate from the server.
             * This is equivalent to setting a BlindCertificateTrustChecker
             * using setCertificateTrustChecker().
             */
            void setAlwaysTrustCertificates();

            WhiteboardSessionManager* getWhiteboardSessionManager() const;

            PubSubManager* getPubSubManager() const {
                return pubsubManager;
            }


        public:
            /**
             * This signal is emitted when a JID changes presence.
             */
            boost::signals2::signal<void (std::shared_ptr<Presence>)> onPresenceChange;

        private:
            Storages* getStorages() const;

        protected:
            void handleConnected();

        private:
            Storages* storages;
            MemoryStorages* memoryStorages;
            SoftwareVersionResponder* softwareVersionResponder;
            XMPPRosterImpl* roster;
            XMPPRosterController* rosterController;
            PresenceOracle* presenceOracle;
            DirectedPresenceSender* directedPresenceSender;
            StanzaChannelPresenceSender* stanzaChannelPresenceSender;
            std::unique_ptr<MIXRegistry> mixRegistry;
            MUCRegistry* mucRegistry;
            VCardManager* vcardManager;
            AvatarManager* avatarManager;
            CapsManager* capsManager;
            EntityCapsManager* entityCapsManager;
            NickManager* nickManager;
            NickResolver* nickResolver;
            SubscriptionManager* subscriptionManager;
            MUCManager* mucManager;
            ClientDiscoManager* discoManager;
            JingleSessionManager* jingleSessionManager;
            FileTransferManager* fileTransferManager;
            BlindCertificateTrustChecker* blindCertificateTrustChecker;
            WhiteboardSessionManager* whiteboardSessionManager;
            ClientBlockListManager* blockListManager;
            PubSubManager* pubsubManager;
    };
}

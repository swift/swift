/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Client/CoreClient.h>

#include <Swiften/Base/SafeString.h>

namespace Swift {
	class SoftwareVersionResponder;
	class BlindCertificateTrustChecker;
	class XMPPRoster;
	class XMPPRosterImpl;
	class MUCManager;
	class XMPPRosterController;
	class PresenceOracle;
	class PresenceSender;
	class DirectedPresenceSender;
	class StanzaChannelPresenceSender;
	class MUCRegistry;
	class Storages;
	class MemoryStorages;
	class VCardManager;
	class AvatarManager;
	class CapsManager;
	class EntityCapsManager;
	class EntityCapsProvider;
	class NickResolver;
	class SubscriptionManager;
	class ClientDiscoManager;
	class NickManager;
	class FileTransferManager;
	class JingleSessionManager;
	class FileTransferManager;

	/**
	 * Provides the core functionality for writing XMPP client software.
	 *
	 * Besides connecting to an XMPP server, this class also provides interfaces for
	 * performing most tasks on the XMPP network.
	 */
	class Client : public CoreClient {
		public:
			/**
			 * Constructs a client for the given JID with the given password.
			 *
			 * \param storages The interfaces for storing cache information etc. If 
			 *	this is NULL,
			 *	all data will be stored in memory (and be lost on shutdown)
			 */
			Client(const JID& jid, const SafeString& password, NetworkFactories* networkFactories, Storages* storages = NULL);
			~Client();


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
			void requestRoster();

			/**
			 * Returns the last received presence for the given (full) JID.
			 */
			boost::shared_ptr<Presence> getLastPresence(const JID& jid) const;

			/**
			 * Returns the presence with the highest priority received for the given JID.
			 */
			boost::shared_ptr<Presence> getHighestPriorityPresence(const JID& bareJID) const;

			PresenceOracle* getPresenceOracle() const {
				return presenceOracle;
			}

			PresenceSender* getPresenceSender() const;

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
		
		public:
			/**
			 * This signal is emitted when a JID changes presence.
			 */
			boost::signal<void (boost::shared_ptr<Presence>)> onPresenceChange;

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
	};
}

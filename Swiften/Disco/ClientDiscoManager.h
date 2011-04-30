/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/CapsInfo.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Presence/PayloadAddingPresenceSender.h>

namespace Swift {
	class IQRouter;
	class DiscoInfoResponder;
	class PayloadAddingPresenceSender;
	class PresenceSender;

	/**
	 * Class responsible for managing outgoing disco information for a client.
	 *
	 * The manager will respond to disco#info requests, and add entity capabilities information
	 * to outgoing presence.
	 *
	 * To use this class, call setCapsNode() once with the caps URI of the client. After this,
	 * call setDiscoInfo() with the capabilities for the client. This can be
	 * called whenever the capabilities change.
	 */
	class ClientDiscoManager {
		public:
			/**
			 * Constructs the manager
			 *
			 * \param iqRouter the router on which requests will be answered
			 * \param presenceSender the presence sender to which all outgoing presence
			 *   (with caps information) will be sent.
			 */
			ClientDiscoManager(IQRouter* iqRouter, PresenceSender* presenceSender);
			~ClientDiscoManager();

			/**
			 * Needs to be called before calling setDiscoInfo().
			 */
			void setCapsNode(const std::string& node);

			/**
			 * Sets the capabilities of the client.
			 */
			void setDiscoInfo(const DiscoInfo& info);

			/**
			 * Returns the presence sender through which all outgoing presence
			 * should be sent.
			 * The manager will add the necessary caps information, and forward it to
			 * the presence sender passed at construction time.
			 */
			PresenceSender* getPresenceSender() const {
				return presenceSender;
			}

		private:
			PayloadAddingPresenceSender* presenceSender;
			DiscoInfoResponder* discoInfoResponder;
			std::string capsNode;
			CapsInfo::ref capsInfo;
	};
}

/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <boost/shared_ptr.hpp>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/String.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/DiscoItems.h>
#include <Swiften/Elements/ErrorPayload.h>

namespace Swift {
	class IQRouter;
	/**
	 * Recursively walk service discovery trees to find all services offered.
	 * This stops on any disco item that's not reporting itself as a server.
	 */
	class DiscoServiceWalker : public boost::signals::trackable {
		public:
			DiscoServiceWalker(const JID& service, IQRouter* iqRouter, size_t maxSteps = 200);
			/** Start the walk. Call this exactly once.*/
			void beginWalk();
			/** Emitted for each service found. */
			boost::signal<void(const JID&, boost::shared_ptr<DiscoInfo>)> onServiceFound;
			/** Emitted when walking is complete.*/
			boost::signal<void()> onWalkComplete;
		private:
			void handleReceivedDiscoItem(const JID& item);
			void walkNode(const JID& jid);
			void markNodeCompleted(const JID& jid);
			void handleDiscoInfoResponse(boost::shared_ptr<DiscoInfo> info, ErrorPayload::ref error, const JID& jid);
			void handleDiscoItemsResponse(boost::shared_ptr<DiscoItems> items, ErrorPayload::ref error, const JID& jid);
			void handleDiscoError(const JID& jid, ErrorPayload::ref error);
			JID service_;
			IQRouter* iqRouter_;
			size_t maxSteps_;
			std::vector<JID> servicesBeingSearched_;
			std::vector<JID> searchedServices_;
	};
}

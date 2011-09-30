/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <set>

#include <boost/shared_ptr.hpp>
#include <Swiften/Base/boost_bsignals.h>
#include <string>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/DiscoItems.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Disco/GetDiscoInfoRequest.h>
#include <Swiften/Disco/GetDiscoItemsRequest.h>

namespace Swift {
	class IQRouter;
	/**
	 * Recursively walk service discovery trees to find all services offered.
	 * This stops on any disco item that's not reporting itself as a server.
	 */
	class DiscoServiceWalker {
		public:
			DiscoServiceWalker(const JID& service, IQRouter* iqRouter, size_t maxSteps = 200);

			/**
			 * Start the walk.
			 *
			 * Call this exactly once.
			 */
			void beginWalk();

			/**
			 * End the walk.
			 */
			void endWalk();

			bool isActive() const {
				return active_;
			}

			/** Emitted for each service found. */
			boost::signal<void(const JID&, boost::shared_ptr<DiscoInfo>)> onServiceFound;

			/** Emitted when walking is complete.*/
			boost::signal<void()> onWalkComplete;

		private:
			void walkNode(const JID& jid);
			void markNodeCompleted(const JID& jid);
			void handleDiscoInfoResponse(boost::shared_ptr<DiscoInfo> info, ErrorPayload::ref error, GetDiscoInfoRequest::ref request);
			void handleDiscoItemsResponse(boost::shared_ptr<DiscoItems> items, ErrorPayload::ref error, GetDiscoItemsRequest::ref request);
			void handleDiscoError(const JID& jid, ErrorPayload::ref error);

		private:
			JID service_;
			IQRouter* iqRouter_;
			size_t maxSteps_;
			bool active_;
			std::set<JID> servicesBeingSearched_;
			std::set<JID> searchedServices_;
			std::set<GetDiscoInfoRequest::ref> pendingDiscoInfoRequests_;
			std::set<GetDiscoItemsRequest::ref> pendingDiscoItemsRequests_;
	};
}

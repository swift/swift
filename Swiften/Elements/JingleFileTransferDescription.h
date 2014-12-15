/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include <Swiften/Elements/JingleDescription.h>
#include <Swiften/Elements/StreamInitiationFileInfo.h>

namespace Swift {
	class JingleFileTransferDescription : public JingleDescription {
		public:
			typedef boost::shared_ptr<JingleFileTransferDescription> ref;

			void addOffer(const StreamInitiationFileInfo& offer) {
				offers.push_back(offer);
			}
			

			const std::vector<StreamInitiationFileInfo>& getOffers() const {
				return offers;
			}
			
			void addRequest(const StreamInitiationFileInfo& request) {
				reqeusts.push_back(request);
			}
			
			const std::vector<StreamInitiationFileInfo>& getRequests() const {
				return reqeusts;
			}

		private:
			std::vector<StreamInitiationFileInfo> offers;
			std::vector<StreamInitiationFileInfo> reqeusts;
	};
}

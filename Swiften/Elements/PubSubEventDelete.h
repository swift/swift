/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <boost/shared_ptr.hpp>
#include <string>

#include <Swiften/Elements/PubSubEventPayload.h>
#include <Swiften/Elements/PubSubEventRedirect.h>

namespace Swift {
	class SWIFTEN_API PubSubEventDelete : public PubSubEventPayload {
		public:
			
			PubSubEventDelete();
			
			virtual ~PubSubEventDelete();

			const std::string& getNode() const {
				return node;
			}

			void setNode(const std::string& value) {
				this->node = value ;
			}

			boost::shared_ptr<PubSubEventRedirect> getRedirects() const {
				return redirects;
			}

			void setRedirects(boost::shared_ptr<PubSubEventRedirect> value) {
				this->redirects = value ;
			}


		private:
			std::string node;
			boost::shared_ptr<PubSubEventRedirect> redirects;
	};
}

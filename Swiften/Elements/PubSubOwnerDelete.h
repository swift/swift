/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <boost/shared_ptr.hpp>
#include <string>

#include <Swiften/Elements/PubSubOwnerPayload.h>
#include <Swiften/Elements/PubSubOwnerRedirect.h>

namespace Swift {
	class SWIFTEN_API PubSubOwnerDelete : public PubSubOwnerPayload {
		public:
			
			PubSubOwnerDelete();
			PubSubOwnerDelete(const std::string& node) : node(node) {}
			virtual ~PubSubOwnerDelete();

			const std::string& getNode() const {
				return node;
			}

			void setNode(const std::string& value) {
				this->node = value ;
			}

			boost::shared_ptr<PubSubOwnerRedirect> getRedirect() const {
				return redirect;
			}

			void setRedirect(boost::shared_ptr<PubSubOwnerRedirect> value) {
				this->redirect = value ;
			}


		private:
			std::string node;
			boost::shared_ptr<PubSubOwnerRedirect> redirect;
	};
}

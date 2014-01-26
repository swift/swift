/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

#include <Swiften/Elements/PubSubOptions.h>
#include <Swiften/Elements/PubSubPayload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class SWIFTEN_API PubSubSubscribe : public PubSubPayload {
		public:
			
			PubSubSubscribe();
			
			virtual ~PubSubSubscribe();

			const boost::optional< std::string >& getNode() const {
				return node;
			}

			void setNode(const boost::optional< std::string >& value) {
				this->node = value ;
			}

			const JID& getJID() const {
				return jid;
			}

			void setJID(const JID& value) {
				this->jid = value ;
			}

			boost::shared_ptr<PubSubOptions> getOptions() const {
				return options;
			}

			void setOptions(boost::shared_ptr<PubSubOptions> value) {
				this->options = value ;
			}


		private:
			boost::optional< std::string > node;
			JID jid;
			boost::shared_ptr<PubSubOptions> options;
	};
}

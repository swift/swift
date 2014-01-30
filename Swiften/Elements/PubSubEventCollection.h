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

#include <Swiften/Elements/PubSubEventAssociate.h>
#include <Swiften/Elements/PubSubEventDisassociate.h>
#include <Swiften/Elements/PubSubEventPayload.h>

namespace Swift {
	class SWIFTEN_API PubSubEventCollection : public PubSubEventPayload {
		public:
			
			PubSubEventCollection();
			
			virtual ~PubSubEventCollection();

			const boost::optional< std::string >& getNode() const {
				return node;
			}

			void setNode(const boost::optional< std::string >& value) {
				this->node = value ;
			}

			boost::shared_ptr<PubSubEventDisassociate> getDisassociate() const {
				return disassociate;
			}

			void setDisassociate(boost::shared_ptr<PubSubEventDisassociate> value) {
				this->disassociate = value ;
			}

			boost::shared_ptr<PubSubEventAssociate> getAssociate() const {
				return associate;
			}

			void setAssociate(boost::shared_ptr<PubSubEventAssociate> value) {
				this->associate = value ;
			}


		private:
			boost::optional< std::string > node;
			boost::shared_ptr<PubSubEventDisassociate> disassociate;
			boost::shared_ptr<PubSubEventAssociate> associate;
	};
}

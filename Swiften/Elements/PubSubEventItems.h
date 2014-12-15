/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

#include <Swiften/Elements/PubSubEventItem.h>
#include <Swiften/Elements/PubSubEventPayload.h>
#include <Swiften/Elements/PubSubEventRetract.h>

namespace Swift {
	class SWIFTEN_API PubSubEventItems : public PubSubEventPayload {
		public:
			
			PubSubEventItems();
			
			virtual ~PubSubEventItems();

			const std::string& getNode() const {
				return node;
			}

			void setNode(const std::string& value) {
				this->node = value ;
			}

			const std::vector< boost::shared_ptr<PubSubEventItem> >& getItems() const {
				return items;
			}

			void setItems(const std::vector< boost::shared_ptr<PubSubEventItem> >& value) {
				this->items = value ;
			}

			void addItem(boost::shared_ptr<PubSubEventItem> value) {
				this->items.push_back(value);
			}

			const std::vector< boost::shared_ptr<PubSubEventRetract> >& getRetracts() const {
				return retracts;
			}

			void setRetracts(const std::vector< boost::shared_ptr<PubSubEventRetract> >& value) {
				this->retracts = value ;
			}

			void addRetract(boost::shared_ptr<PubSubEventRetract> value) {
				this->retracts.push_back(value);
			}


		private:
			std::string node;
			std::vector< boost::shared_ptr<PubSubEventItem> > items;
			std::vector< boost::shared_ptr<PubSubEventRetract> > retracts;
	};
}

/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubOwnerPayload.h>

namespace Swift {
	class SWIFTEN_API PubSubOwnerConfigure : public PubSubOwnerPayload {
		public:
			
			PubSubOwnerConfigure();
			PubSubOwnerConfigure(const std::string& node) : node(node) {}
			virtual ~PubSubOwnerConfigure();

			const boost::optional< std::string >& getNode() const {
				return node;
			}

			void setNode(const boost::optional< std::string >& value) {
				this->node = value ;
			}

			boost::shared_ptr<Form> getData() const {
				return data;
			}

			void setData(boost::shared_ptr<Form> value) {
				this->data = value ;
			}


		private:
			boost::optional< std::string > node;
			boost::shared_ptr<Form> data;
	};
}

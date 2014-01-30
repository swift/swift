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

#include <Swiften/Elements/Form.h>
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

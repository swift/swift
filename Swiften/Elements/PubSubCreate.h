/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubConfigure.h>
#include <Swiften/Elements/PubSubPayload.h>

namespace Swift {
	class SWIFTEN_API PubSubCreate : public PubSubPayload {
		public:
			
			PubSubCreate();
			PubSubCreate(const std::string& node) : node(node) {}
			virtual ~PubSubCreate();

			const std::string& getNode() const {
				return node;
			}

			void setNode(const std::string& value) {
				this->node = value ;
			}

			boost::shared_ptr<PubSubConfigure> getConfigure() const {
				return configure;
			}

			void setConfigure(boost::shared_ptr<PubSubConfigure> value) {
				this->configure = value ;
			}


		private:
			std::string node;
			boost::shared_ptr<PubSubConfigure> configure;
	};
}

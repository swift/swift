/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <boost/optional.hpp>
#include <string>

#include <Swiften/Elements/PubSubPayload.h>

namespace Swift {
	class SWIFTEN_API PubSubDefault : public PubSubPayload {
		public:
			enum Type {
				None,
				Collection,
				Leaf
			};

			PubSubDefault();
			
			virtual ~PubSubDefault();

			const boost::optional< std::string >& getNode() const {
				return node;
			}

			void setNode(const boost::optional< std::string >& value) {
				this->node = value ;
			}

			Type getType() const {
				return type;
			}

			void setType(Type value) {
				this->type = value ;
			}


		private:
			boost::optional< std::string > node;
			Type type;
	};
}

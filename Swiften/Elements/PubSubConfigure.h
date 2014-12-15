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

#include <Swiften/Elements/Form.h>

namespace Swift {
	class SWIFTEN_API PubSubConfigure : public Payload {
		public:
			
			PubSubConfigure();
			
			virtual ~PubSubConfigure();

			boost::shared_ptr<Form> getData() const {
				return data;
			}

			void setData(boost::shared_ptr<Form> value) {
				this->data = value ;
			}


		private:
			boost::shared_ptr<Form> data;
	};
}

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

#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/PubSubOwnerPayload.h>

namespace Swift {
	class SWIFTEN_API PubSubOwnerDefault : public PubSubOwnerPayload {
		public:
			
			PubSubOwnerDefault();
			
			virtual ~PubSubOwnerDefault();

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

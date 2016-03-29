/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
	class SWIFTEN_API MUCOwnerPayload : public Payload {
		public:
			typedef boost::shared_ptr<MUCOwnerPayload> ref;

			MUCOwnerPayload() {
			}

			boost::shared_ptr<Payload> getPayload() const {
				return payload;
			}

			void setPayload(boost::shared_ptr<Payload> p) {
				payload = p;
			}

			Form::ref getForm() {
				return boost::dynamic_pointer_cast<Form>(payload);
			}

		private:
			boost::shared_ptr<Payload> payload;
	};
}

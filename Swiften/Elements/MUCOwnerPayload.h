/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/Form.h>

namespace Swift {
	class MUCOwnerPayload : public Payload {
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

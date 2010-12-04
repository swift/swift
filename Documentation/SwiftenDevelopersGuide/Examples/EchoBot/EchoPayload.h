/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */
//...
#pragma once

#include <Swiften/Swiften.h>

using namespace Swift;
//...
class EchoPayload : public Payload {
	public:
		EchoPayload() {}

		const String& getMessage() const {
			return message;
		}

		void setMessage(const String& message) {
			this->message = message;
		}

	private:
		String message;
};

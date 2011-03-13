/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */
//...
#pragma once

#include <Swiften/Swiften.h>

//...
class EchoPayload : public Swift::Payload {
	public:
		EchoPayload() {}

		const std::string& getMessage() const {
			return message;
		}

		void setMessage(const std::string& message) {
			this->message = message;
		}

	private:
		std::string message;
};

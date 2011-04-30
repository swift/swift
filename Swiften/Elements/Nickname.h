/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Payload.h>
#include <string>

namespace Swift {
	class Nickname : public Payload {
		public:
			Nickname(const std::string& nickname = "") : nickname(nickname) {
			}

			void setNickname(const std::string& nickname) {
				this->nickname = nickname;
			}

			const std::string& getNickname() const {
				return nickname;
			}

		private:
			std::string nickname;
	};
}

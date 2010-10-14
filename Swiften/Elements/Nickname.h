/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Elements/Payload.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class Nickname : public Payload {
		public:
			Nickname(const String& nickname = "") : nickname(nickname) {
			}

			void setNickname(const String& nickname) {
				this->nickname = nickname;
			}

			const String& getNickname() const {
				return nickname;
			}

		private:
			String nickname;
	};
}

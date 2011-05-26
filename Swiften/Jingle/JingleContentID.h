/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <Swiften/Elements/JingleContentPayload.h>

namespace Swift {
	class JingleContentID {
		public:
			JingleContentID(const std::string& name, JingleContentPayload::Creator creator) : name(name), creator(creator) {
			}
			
			const std::string getName() const {
				return this->name;
			}
			
			JingleContentPayload::Creator getCreator() const {
				return this->creator;
			}

		private:
			std::string name;
			JingleContentPayload::Creator creator;
	};
}

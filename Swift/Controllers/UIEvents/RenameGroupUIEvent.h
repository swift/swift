/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/UIEvents/UIEvent.h>
#include <string>

namespace Swift {
	class RenameGroupUIEvent : public UIEvent {
		public:
			RenameGroupUIEvent(const std::string& group, const std::string& newName) : group(group), newName(newName) {
			}

			const std::string& getGroup() const {
				return group;
			}

			const std::string& getNewName() const {
				return newName;
			}

		private:
			std::string group;
			std::string newName;
	};
}

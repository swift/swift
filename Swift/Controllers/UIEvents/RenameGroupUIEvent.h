/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIEvents/UIEvent.h>
#include <Swiften/Base/String.h>

namespace Swift {
	class RenameGroupUIEvent : public UIEvent {
		public:
			RenameGroupUIEvent(const String& group, const String& newName) : group(group), newName(newName) {
			}

			const String& getGroup() const {
				return group;
			}

			const String& getNewName() const {
				return newName;
			}

		private:
			String group;
			String newName;
	};
}

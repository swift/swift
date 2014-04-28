/*
 * Copyright (c) 2010-2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
	class SWIFTEN_API StreamManagementEnabled : public ToplevelElement {
		public:
			StreamManagementEnabled();
			~StreamManagementEnabled();

			void setResumeSupported() {
				resumeSupported = true;
			}

			bool getResumeSupported() const {
				return resumeSupported;
			}

			void setResumeID(const std::string& id) {
				resumeID = id;
			}

			const std::string& getResumeID() const {
				return resumeID;
			}

		private:
			bool resumeSupported;
			std::string resumeID;
	};
}

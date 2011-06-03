/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <boost/optional.hpp>

#include <Swiften/Elements/Element.h>

namespace Swift {
	class StreamResume : public Element {
		public:
			StreamResume();
			~StreamResume();

			void setResumeID(const std::string& id) {
				resumeID = id;
			}

			const std::string& getResumeID() const {
				return resumeID;
			}

			const boost::optional<unsigned int> getHandledStanzasCount() const {
				return handledStanzasCount;
			}

			void setHandledStanzasCount(unsigned int i) {
				handledStanzasCount = i;
			}

		private:
			std::string resumeID;
			boost::optional<unsigned int> handledStanzasCount;
	};
}

/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <string>
#include <Swiften/Elements/JinglePayload.h>
#include <Swiften/Elements/JingleContent.h>
#include <Swiften/Base/foreach.h>

namespace Swift {
	class JingleSession {
			friend class JingleResponder;
		public:
			typedef boost::shared_ptr<JingleSession> ref;

			JingleSession(const std::string& id, const std::vector<JingleContent::ref>& contents);
			virtual ~JingleSession();

			std::string getID() const {
				return id;
			}

			template<typename T>
			JingleContent::ref getContentWithDescription() const {
				foreach (JingleContent::ref content, contents) {
					if (content->getDescription<T>()) {
						return content;
					}
				}
				return JingleContent::ref();
			}

			const std::vector<JingleContent::ref> getContents() const {
				return contents;
			}

			void terminate(JinglePayload::Reason::Type reason);

		private:
			void handleIncomingAction(JinglePayload::ref);

		private:
			std::string id;
			std::vector<JingleContent::ref> contents;
	};
}

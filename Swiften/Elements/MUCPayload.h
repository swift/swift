/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "Swiften/JID/JID.h"
#include <string>
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class MUCPayload : public Payload {
		public:
			typedef boost::shared_ptr<MUCPayload> ref;

			MUCPayload() {
				maxChars_ = -1;
				maxStanzas_ = -1;
				seconds_ = -1;
			}

			void setMaxChars(int maxChars) {
				maxChars_ = maxChars;
			}

			void setMaxStanzas(int maxStanzas) {
				maxStanzas_ = maxStanzas;
			}

			void setSeconds(int seconds) {
				seconds_ = seconds;
			}

			void setSince(boost::posix_time::ptime since) {
				since_ = since;
			}

			int getMaxChars() {
				return maxChars_;
			}

			int getMaxStanzas() {
				return maxStanzas_;
			}

			int getSeconds() {
				return seconds_;
			}

			boost::posix_time::ptime getSince() {
				return since_;
			}

		private:
			int maxChars_;
			int maxStanzas_;
			int seconds_;
			boost::posix_time::ptime since_;
	};
}

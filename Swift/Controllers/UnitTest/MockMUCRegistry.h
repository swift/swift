/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/MUC/MUCRegistry.h"

namespace Swift {
	class JID;

	class MockMUCRegistry : public MUCRegistry {
		public:
			virtual ~MockMUCRegistry() {};

			void setNext(bool next) {next_ = next;}
			virtual bool isMUC(const JID&) const {return next_;}

		private:
			bool next_;
	};
}

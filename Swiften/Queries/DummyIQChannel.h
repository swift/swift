/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Queries/IQChannel.h>

namespace Swift {
	class DummyIQChannel : public IQChannel {
		public:
			DummyIQChannel() {}

			virtual void sendIQ(boost::shared_ptr<IQ> iq) {
				iqs_.push_back(iq);
			}

			virtual std::string getNewIQID() {
				return "test-id";
			}

			virtual bool isAvailable() const {
				return true;
			}

			std::vector<boost::shared_ptr<IQ> > iqs_;
	};
}

#ifndef SWIFTEN_DummyIQChannel_H
#define SWIFTEN_DummyIQChannel_H

#include <vector>

#include "Swiften/Queries/IQChannel.h"

namespace Swift {
	class DummyIQChannel : public IQChannel {
		public:
			DummyIQChannel() {}

			virtual void sendIQ(boost::shared_ptr<IQ> iq) {
				iqs_.push_back(iq);
			}

			virtual String getNewIQID() {
				return "test-id";
			}

			std::vector<boost::shared_ptr<IQ> > iqs_;
	};
}

#endif

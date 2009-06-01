#ifndef SWIFTEN_IQChannel_H
#define SWIFTEN_IQChannel_H

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/IQ.h"

namespace Swift {
	class IQChannel {
		public:
			virtual ~IQChannel();

			virtual void sendIQ(boost::shared_ptr<IQ>) = 0;
			virtual String getNewIQID() = 0;

			boost::signal<void (boost::shared_ptr<IQ>)> onIQReceived;
	};
}

#endif

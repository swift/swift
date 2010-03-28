#ifndef SWIFTEN_IQHandler_H
#define SWIFTEN_IQHandler_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/IQ.h"

namespace Swift {
	class IQRouter;

	class IQHandler {
		public:
			virtual ~IQHandler();

			virtual bool handleIQ(boost::shared_ptr<IQ>) = 0;
	};
}

#endif

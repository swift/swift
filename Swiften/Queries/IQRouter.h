#ifndef SWIFTEN_IQRouter_H
#define SWIFTEN_IQRouter_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/IQ.h"

namespace Swift {
	class IQChannel;
	class IQHandler;

	class IQRouter {
		public:
			IQRouter(IQChannel* channel);

			void addHandler(IQHandler* handler);
			void removeHandler(IQHandler* handler);

			void sendIQ(boost::shared_ptr<IQ> iq);
			String getNewIQID();

		private:
			void handleIQ(boost::shared_ptr<IQ> iq);

		private:
			IQChannel* channel_;
			std::vector<IQHandler*> handlers_;
	};
}

#endif

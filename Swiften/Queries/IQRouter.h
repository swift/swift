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
			~IQRouter();

			void addHandler(IQHandler* handler);
			void removeHandler(IQHandler* handler);
			void addHandler(boost::shared_ptr<IQHandler> handler);
			void removeHandler(boost::shared_ptr<IQHandler> handler);

			void sendIQ(boost::shared_ptr<IQ> iq);
			String getNewIQID();
			
			bool isAvailable();

		private:
			void handleIQ(boost::shared_ptr<IQ> iq);
			void processPendingRemoves();

		private:
			IQChannel* channel_;
			std::vector< boost::shared_ptr<IQHandler> > handlers_;
			std::vector< boost::shared_ptr<IQHandler> > queuedRemoves_;
			bool queueRemoves_;
	};
}

#endif

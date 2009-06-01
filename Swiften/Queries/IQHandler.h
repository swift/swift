#ifndef SWIFTEN_IQHandler_H
#define SWIFTEN_IQHandler_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/IQ.h"

namespace Swift {
	class IQRouter;

	class IQHandler {
		public:
			IQHandler(IQRouter* router);
			virtual ~IQHandler();

			virtual bool handleIQ(boost::shared_ptr<IQ>) = 0;

		protected:
			IQRouter* getRouter() const {
				return router_;
			}
	
		private:
			IQRouter* router_;
	};
}

#endif

#pragma once

#include "Swiften/Queries/Responder.h"

namespace Swift {
	template<typename T>
	class GetResponder : public Responder<T> {
		public:
			GetResponder(IQRouter* router) : Responder<T>(router) {}

		private:
			virtual bool handleSetRequest(const JID&, const String&, boost::shared_ptr<T>) { return false; }
	};
}

#pragma once

#include "Swiften/Base/Error.h"

namespace Swift {
	class DomainNameResolveError : public Error {
		public:
			DomainNameResolveError() {}
	};
}

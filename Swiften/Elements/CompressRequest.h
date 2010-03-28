#ifndef SWIFTEN_CompressRequest_H
#define SWIFTEN_CompressRequest_H

#include "Swiften/Elements/Element.h"

namespace Swift {
	class CompressRequest : public Element
	{
		public:
			CompressRequest(const String& method = "") : method_(method) {}

			const String& getMethod() const {
				return method_;
			}

			void setMethod(const String& method) { 
				method_ = method;
			}

		private:
			String method_;
	};
}

#endif

/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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

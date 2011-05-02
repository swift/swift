/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Element.h>

namespace Swift {
	class CompressRequest : public Element
	{
		public:
			CompressRequest(const std::string& method = "") : method_(method) {}

			const std::string& getMethod() const {
				return method_;
			}

			void setMethod(const std::string& method) { 
				method_ = method;
			}

		private:
			std::string method_;
	};
}

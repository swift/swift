/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

#include "Swiften/Elements/Payload.h"

namespace Swift {
	class Replace : public Payload {
		public:
			typedef boost::shared_ptr<Replace> ref;
			Replace(std::string id = "") : replaceID_(id) {};
			std::string getId() {
				return replaceID_;
			}
			void setId(std::string id) {
				replaceID_ = id;
			}
		private:
			std::string replaceID_;
	};
}

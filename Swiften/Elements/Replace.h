/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

#include <Swiften/Elements/Payload.h>

namespace Swift {
	class Replace : public Payload {
		public:
			typedef boost::shared_ptr<Replace> ref;
			Replace(const std::string& id = std::string()) : replaceID_(id) {};
			const std::string& getID() const {
				return replaceID_;
			}
			void setID(const std::string& id) {
				replaceID_ = id;
			}
		private:
			std::string replaceID_;
	};
}

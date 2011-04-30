/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

#include <Swiften/Elements/Element.h>

namespace Swift {
	class ComponentHandshake : public Element {
		public:
			typedef boost::shared_ptr<ComponentHandshake> ref;

			ComponentHandshake(const std::string& data = "") : data(data) {
			}

			void setData(const std::string& d) {
				data = d;
			}

			const std::string& getData() const {
				return data;
			}

		private:
			std::string data;
	};
}

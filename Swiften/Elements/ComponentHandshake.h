/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Elements/Element.h"
#include "Swiften/Base/String.h"
#include "Swiften/Base/Shared.h"

namespace Swift {
	class ComponentHandshake : public Element, public Shared<ComponentHandshake> {
		public:
			ComponentHandshake(const String& data = "") : data(data) {
			}

			void setData(const String& d) {
				data = d;
			}

			const String& getData() const {
				return data;
			}

		private:
			String data;
	};
}

/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/JingleTransport.h>
#include <Swiften/Elements/Bytestreams.h>

namespace Swift {
	class JingleS5BTransport : public JingleTransport {
		public:
			const Bytestreams& getInfo() const {
				return info;
			}

			void setInfo(const Bytestreams& info) {
				this->info = info;
			}

		private:
			Bytestreams info;
	};
}

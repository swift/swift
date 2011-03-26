/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/JingleTransportPayload.h>
#include <Swiften/Elements/Bytestreams.h>

// FIXME: Remove Bytestreams, and replace by our own candidate

namespace Swift {
	class JingleS5BTransportPayload : public JingleTransportPayload {
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

/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_StatusShow_H
#define SWIFTEN_StatusShow_H

#include "Swiften/Elements/Payload.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class StatusShow : public Payload {
		public:
			enum Type { Online, Away, FFC, XA, DND, None };

			StatusShow(const Type& type = Online) : type_(type) {
			}

			void setType(const Type& type) {
				type_ = type;
			}

			const Type& getType() const {
				return type_;
			}

			static String typeToFriendlyName(Type type) {
				switch (type) {
				case Online: return "Available";
				case FFC: return "Available";
				case Away: return "Away";
				case XA: return "Away";
				case DND: return "Busy";
				case None: return "Offline";
				}
				return "Unknown";
			}

		private:
			Type type_;
	};
}

#endif

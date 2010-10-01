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

			/**
			 * Can be used for rough ordering of Types.
			 * Greater magnitude = more available.
			 */
			static int typeToAvailabilityOrdering(Type type) {
				switch (type) {
				case Online: return 4;
				case FFC: return 5;
				case Away: return 2;
				case XA: return 1;
				case DND: return 3;
				case None: return 0;
				}
				return -1;
			}

		private:
			Type type_;
	};
}

#endif

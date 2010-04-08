/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Elements/VCard.h"

namespace Swift {
	class JID;
	class VCardStorage;

	class VCardManager {
		public:
			VCardManager(IQRouter* iqRouter, VCardStorage* vcardStorage);

			virtual boost::shared_ptr<VCard> getVCardAndRequestWhenNeeded(const JID& jid) const ;

		public:
			boost::signal<void (const JID&)> onVCardChanged;

		private:
			IQRouter* iqRouter;
			VCardStorage* storage;
	};
}

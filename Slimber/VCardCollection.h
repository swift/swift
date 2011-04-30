/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/VCard.h>

namespace Swift {
	class VCardCollection {
		public:
			virtual ~VCardCollection();

			virtual boost::shared_ptr<VCard> getOwnVCard() const = 0;
			virtual void setOwnVCard(boost::shared_ptr<VCard> vcard) = 0;
	};
}

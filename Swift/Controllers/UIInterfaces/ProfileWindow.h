/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/VCard.h>

namespace Swift {
	class ProfileWindow {
		public:
			virtual ~ProfileWindow() {};

			virtual void setVCard(VCard::ref vcard) = 0;

			virtual void setEnabled(bool b) = 0;
			virtual void setProcessing(bool b) = 0;
			virtual void setError(const std::string&) = 0;

			virtual void show() = 0;
			virtual void hide() = 0;

			boost::signal<void (VCard::ref)> onVCardChangeRequest;
	};
}

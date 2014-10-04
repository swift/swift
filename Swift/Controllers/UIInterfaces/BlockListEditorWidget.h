/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <Swiften/JID/JID.h>
#include <Swiften/Base/boost_bsignals.h>

namespace Swift {

	class ClientBlockListManager;

	class BlockListEditorWidget {
		public:
			virtual ~BlockListEditorWidget() {}

			virtual void show() = 0;
			virtual void hide() = 0;

			virtual void setCurrentBlockList(const std::vector<JID>& blockedJIDs) = 0;
			virtual void setBusy(bool isBusy) = 0;
			virtual void setError(const std::string&) = 0;

			virtual std::vector<JID> getCurrentBlockList() const = 0;

			boost::signal<void (const std::vector<JID>& /* blockedJID */)> onSetNewBlockList;
	};

}

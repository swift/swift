/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Queries/SetResponder.h>
#include <Swiften/Elements/JinglePayload.h>

namespace Swift {
	class IQRouter;
	class JingleSessionManager;

	class JingleResponder : public SetResponder<JinglePayload> {
		public:
			JingleResponder(JingleSessionManager* sessionManager, IQRouter* router);
			virtual ~JingleResponder();
		private:
			virtual bool handleSetRequest(const JID& from, const JID& to, const std::string& id, boost::shared_ptr<JinglePayload> payload);

		private:
			JingleSessionManager* sessionManager;
			IQRouter* router;
	};
}

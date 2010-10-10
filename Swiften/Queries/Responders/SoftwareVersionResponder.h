/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Queries/GetResponder.h"
#include "Swiften/Elements/SoftwareVersion.h"

namespace Swift {
	class IQRouter;

	class SoftwareVersionResponder : public GetResponder<SoftwareVersion> {
		public:
			SoftwareVersionResponder(IQRouter* router);

			void setVersion(const String& client, const String& version);

		private:
			virtual bool handleGetRequest(const JID& from, const String& id, boost::shared_ptr<SoftwareVersion> payload);

		private:
			String client;
			String version;
	};
}

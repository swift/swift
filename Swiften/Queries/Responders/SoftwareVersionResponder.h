/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_SoftwareVersionResponder_H
#define SWIFTEN_SoftwareVersionResponder_H

#include "Swiften/Queries/GetResponder.h"
#include "Swiften/Elements/SoftwareVersion.h"

namespace Swift {
	class IQRouter;

	class SoftwareVersionResponder : public GetResponder<SoftwareVersion> {
		public:
			SoftwareVersionResponder(const String& client, const String& version, IQRouter* router);

		private:
			virtual bool handleGetRequest(const JID& from, const String& id, boost::shared_ptr<SoftwareVersion> payload);

		private:
			String client_;
			String version_;
	};
}

#endif

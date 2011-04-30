/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include <Swiften/Queries/GetResponder.h>
#include <Swiften/Elements/DiscoInfo.h>

namespace Swift {
	class IQRouter;

	class DiscoInfoResponder : public GetResponder<DiscoInfo> {
		public:
			DiscoInfoResponder(IQRouter* router);

			void clearDiscoInfo();
			void setDiscoInfo(const DiscoInfo& info);
			void setDiscoInfo(const std::string& node, const DiscoInfo& info);

		private:
			virtual bool handleGetRequest(const JID& from, const JID& to, const std::string& id, boost::shared_ptr<DiscoInfo> payload);

		private:
			DiscoInfo info_;
			std::map<std::string, DiscoInfo> nodeInfo_;
	};
}

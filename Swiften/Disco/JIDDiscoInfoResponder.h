/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include <Swiften/Queries/GetResponder.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class IQRouter;

	class JIDDiscoInfoResponder : public GetResponder<DiscoInfo> {
		public:
			JIDDiscoInfoResponder(IQRouter* router);

			void clearDiscoInfo(const JID& jid);
			void setDiscoInfo(const JID& jid, const DiscoInfo& info);
			void setDiscoInfo(const JID& jid, const std::string& node, const DiscoInfo& info);

		private:
			virtual bool handleGetRequest(const JID& from, const JID& to, const std::string& id, boost::shared_ptr<DiscoInfo> payload);

		private:
			struct JIDDiscoInfo {
				DiscoInfo discoInfo;
				std::map<std::string, DiscoInfo> nodeDiscoInfo;
			};
			typedef std::map<JID, JIDDiscoInfo> JIDDiscoInfoMap;
			JIDDiscoInfoMap info;
	};
}

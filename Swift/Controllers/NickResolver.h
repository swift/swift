#ifndef SWIFTEN_NickResolver_H
#define SWIFTEN_NickResolver_H

#include <map>
#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class XMPPRoster;
	class NickResolver {
		public:
			NickResolver(boost::shared_ptr<XMPPRoster> xmppRoster);
			String jidToNick(const JID& jid);
		
		private:
			std::map<JID, String> map_;
			boost::shared_ptr<XMPPRoster> xmppRoster_;
	};
}
#endif



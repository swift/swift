#ifndef SWIFTEN_DiscoInfoResponder_H
#define SWIFTEN_DiscoInfoResponder_H

#include <map>

#include "Swiften/Queries/Responder.h"
#include "Swiften/Elements/DiscoInfo.h"

namespace Swift {
	class IQRouter;

	class DiscoInfoResponder : public Responder<DiscoInfo> {
		public:
			DiscoInfoResponder(IQRouter* router);

			void setDiscoInfo(const DiscoInfo& info);
			void setDiscoInfo(const String& node, const DiscoInfo& info);

		private:
			virtual bool handleGetRequest(const JID& from, const String& id, boost::shared_ptr<DiscoInfo> payload);
			virtual bool handleSetRequest(const JID& from, const String& id, boost::shared_ptr<DiscoInfo> payload);

		private:
			DiscoInfo info_;
			std::map<String, DiscoInfo> nodeInfo_;
	};
}

#endif

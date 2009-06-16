#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "Swiften/Client/Client.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/Queries/Requests/GetRosterRequest.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Elements/CapsInfo.h"
#include "Swiften/Queries/Responders/DiscoInfoResponder.h"
#include "Swiften/Disco/CapsInfoGenerator.h"

using namespace Swift;
using namespace boost;

class TuneBot {
	public:
		TuneBot(const JID& jid, const String& password) {
			client_ = new Client(jid, password);
			router_ = new IQRouter(client_);
			
			DiscoInfo discoInfo;
			discoInfo.addIdentity(DiscoInfo::Identity("TuneBot", "client", "bot"));
			discoInfo.addFeature("http://jabber.org/protocol/tune+notify");
			capsInfo_ = boost::shared_ptr<CapsInfo>(new CapsInfo(CapsInfoGenerator("http://el-tramo.be/tunebot").generateCapsInfo(discoInfo)));
			discoResponder_ = new DiscoInfoResponder(router_);
			discoResponder_->setDiscoInfo(discoInfo);
			discoResponder_->setDiscoInfo(capsInfo_->getNode() + "#" + capsInfo_->getVersion(), discoInfo);

			client_->onConnected.connect(bind(&TuneBot::handleSessionStarted, this));
			client_->onMessageReceived.connect(bind(&TuneBot::handleMessage, this, _1));
			client_->connect();
		}

		void handleSessionStarted() {
			boost::shared_ptr<GetRosterRequest> rosterRequest(new GetRosterRequest(router_));
			rosterRequest->onResponse.connect(bind(&TuneBot::handleRosterReceived, this, _1));
			rosterRequest->send();
		}

		void handleRosterReceived(shared_ptr<Payload>) {
			boost::shared_ptr<Presence> presence(new Presence());
			presence->addPayload(capsInfo_);
			presence->setPriority(-1);
			client_->sendPresence(presence);
		}

		void handleMessage(shared_ptr<Message> message) {
			// TODO
		}
	
	private:
		Client* client_;
		IQRouter* router_;
		DiscoInfoResponder* discoResponder_;
		boost::shared_ptr<CapsInfo> capsInfo_;
};


int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <jid> <password>" << std::endl;
		return -1;
	}

	SimpleEventLoop eventLoop;

	TuneBot bot(JID(argv[1]), argv[2]);
	eventLoop.run();
}

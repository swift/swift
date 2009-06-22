#include "Swift/Controllers/MainController.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <stdlib.h>

#include "Swiften/Application/Application.h"
#include "Swiften/Application/ApplicationMessageDisplay.h"
#include "Swift/Controllers/ChatController.h"
#include "Swift/Controllers/ChatWindowFactory.h"
#include "Swift/Controllers/EventController.h"
#include "Swift/Controllers/LoginWindow.h"
#include "Swift/Controllers/LoginWindowFactory.h"
#include "Swift/Controllers/MainWindow.h"
#include "Swift/Controllers/MainWindowFactory.h"
#include "Swift/Controllers/MUCController.h"
#include "Swift/Controllers/NickResolver.h"
#include "Swift/Controllers/RosterController.h"
#include "Swift/Controllers/SystemTray.h"
#include "Swift/Controllers/SystemTrayController.h"
#include "Swift/Controllers/XMPPRosterController.h"
#include "Swiften/Base/foreach.h"
#include "Swiften/Base/String.h"
#include "Swiften/Client/Client.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/VCardUpdate.h"
#include "Swiften/Roster/XMPPRoster.h"
#include "Swiften/Queries/Responders/SoftwareVersionResponder.h"
#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Settings/SettingsProvider.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Queries/Responders/DiscoInfoResponder.h"
#include "Swiften/Disco/CapsInfoGenerator.h"
#include "Swiften/Queries/Requests/GetDiscoInfoRequest.h"
#include "Swiften/Queries/Requests/GetVCardRequest.h"
#include "Swiften/Avatars/AvatarFileStorage.h"
#include "Swiften/Avatars/AvatarManager.h"
#include "Swiften/StringCodecs/SHA1.h"

namespace {
	void printIncomingData(const Swift::String& data) {
		std::cout << "<- " << data << std::endl;
	}

	void printOutgoingData(const Swift::String& data) {
		std::cout << "-> " << data << std::endl;
	}
}

namespace Swift {

static const String CLIENT_NAME = "Swift";
static const String CLIENT_VERSION = "0.3";
static const String CLIENT_NODE = "http://swift.im";

typedef std::pair<JID, ChatController*> JIDChatControllerPair;
typedef std::pair<JID, MUCController*> JIDMUCControllerPair;

MainController::MainController(ChatWindowFactory* chatWindowFactory, MainWindowFactory *mainWindowFactory, LoginWindowFactory *loginWindowFactory, TreeWidgetFactory *treeWidgetFactory, SettingsProvider *settings, Application* application, SystemTray* systemTray)
		: client_(NULL), chatWindowFactory_(chatWindowFactory), mainWindowFactory_(mainWindowFactory), loginWindowFactory_(loginWindowFactory), treeWidgetFactory_(treeWidgetFactory), settings_(settings),
		xmppRosterController_(NULL), rosterController_(NULL), loginWindow_(NULL), clientVersionResponder_(NULL), nickResolver_(NULL), discoResponder_(NULL), 
		serverDiscoInfo_(new DiscoInfo()), presenceOracle_(NULL), avatarManager_(NULL) {
	application_ = application;

	avatarStorage_ = new AvatarFileStorage(application_->getAvatarDir());

	eventController_ = new EventController();
	eventController_->onEventQueueLengthChange.connect(boost::bind(&MainController::handleEventQueueLengthChange, this, _1));
	systemTrayController_ = new SystemTrayController(eventController_, systemTray);
	loginWindow_ = loginWindowFactory_->createLoginWindow(settings->getStringSetting("jid"), settings->getStringSetting("pass"), settings->getStringSetting("certificate"));
	loginWindow_->onLoginRequest.connect(boost::bind(&MainController::handleLoginRequest, this, _1, _2, _3, _4));
}

MainController::~MainController() {
	delete avatarManager_;
	delete discoResponder_;
	delete clientVersionResponder_;
	delete xmppRosterController_;
	delete rosterController_;
	foreach (JIDChatControllerPair controllerPair, chatControllers_) {
		delete controllerPair.second;
	}
	foreach (JIDMUCControllerPair controllerPair, mucControllers_) {
		delete controllerPair.second;
	}
	delete presenceOracle_;
	delete nickResolver_;
	delete client_;
	delete systemTrayController_;
	delete avatarStorage_;
}

void MainController::handleConnected() {
	delete presenceOracle_;
	presenceOracle_ = new PresenceOracle(client_);

	lastSentPresence_ = boost::shared_ptr<Presence>();

	client_->onPresenceReceived.connect(boost::bind(&MainController::handleIncomingPresence, this, _1));

	boost::shared_ptr<XMPPRoster> xmppRoster(new XMPPRoster());


	delete nickResolver_;
	nickResolver_ = new NickResolver(xmppRoster);

	delete rosterController_;
	rosterController_ = new RosterController(xmppRoster, mainWindowFactory_, treeWidgetFactory_);
	rosterController_->onStartChatRequest.connect(boost::bind(&MainController::handleChatRequest, this, _1));
	rosterController_->onJoinMUCRequest.connect(boost::bind(&MainController::handleJoinMUCRequest, this, _1, _2));
	rosterController_->onChangeStatusRequest.connect(boost::bind(&MainController::handleChangeStatusRequest, this, _1, _2));

	delete xmppRosterController_;
	xmppRosterController_ = new XMPPRosterController(client_, xmppRoster);
	xmppRosterController_->requestRoster();

	delete clientVersionResponder_;
	clientVersionResponder_ = new SoftwareVersionResponder(CLIENT_NAME, CLIENT_VERSION, client_);
	loginWindow_->morphInto(rosterController_->getWindow());

	delete avatarManager_;
	avatarManager_ = new AvatarManager(client_, client_, avatarStorage_, this);

	DiscoInfo discoInfo;
	discoInfo.addIdentity(DiscoInfo::Identity(CLIENT_NAME, "client", "pc"));
	capsInfo_ = boost::shared_ptr<CapsInfo>(new CapsInfo(CapsInfoGenerator(CLIENT_NODE).generateCapsInfo(discoInfo)));
	discoResponder_ = new DiscoInfoResponder(client_);
	discoResponder_->setDiscoInfo(discoInfo);
	discoResponder_->setDiscoInfo(capsInfo_->getNode() + "#" + capsInfo_->getVersion(), discoInfo);

	serverDiscoInfo_ = boost::shared_ptr<DiscoInfo>(new DiscoInfo());
	boost::shared_ptr<GetDiscoInfoRequest> discoInfoRequest(new GetDiscoInfoRequest(JID(), client_));
	discoInfoRequest->onResponse.connect(boost::bind(&MainController::handleServerDiscoInfoResponse, this, _1, _2));
	discoInfoRequest->send();

	boost::shared_ptr<GetVCardRequest> vCardRequest(new GetVCardRequest(JID(), client_));
	vCardRequest->onResponse.connect(boost::bind(&MainController::handleOwnVCardReceived, this, _1, _2));
	vCardRequest->send();
	
	//Send presence last to catch all the incoming presences.
	boost::shared_ptr<Presence> initialPresence(new Presence());
	initialPresence->addPayload(capsInfo_);
	sendPresence(initialPresence);
}

void MainController::handleEventQueueLengthChange(int count) {
	application_->getApplicationMessageDisplay()->setMessage(count == 0 ? "" : boost::lexical_cast<std::string>(count).c_str());
}

void MainController::handleChangeStatusRequest(StatusShow::Type show, const String &statusText) {
	boost::shared_ptr<Presence> presence(new Presence());
	presence->addPayload(capsInfo_);
	if (show == StatusShow::None) {
		// FIXME: This is wrong. None doesn't mean unavailable
		presence->setType(Presence::Unavailable);
	}
	else {
		presence->setShow(show);
	}
	presence->setStatus(statusText);
	sendPresence(presence);
}

void MainController::sendPresence(boost::shared_ptr<Presence> presence) {
	if (!vCardPhotoHash_.isEmpty()) {
		presence->addPayload(boost::shared_ptr<VCardUpdate>(new VCardUpdate(vCardPhotoHash_)));
	}
	lastSentPresence_ = presence;
	client_->sendPresence(presence);
	if (presence->getType() == Presence::Unavailable) {
		logout();
	}
}

void MainController::handleIncomingPresence(boost::shared_ptr<Presence> presence) {
	rosterController_->handleIncomingPresence(presence);
}

void MainController::handleLoginRequest(const String &username, const String &password, const String& certificateFile, bool remember) {
	loginWindow_->setMessage("");

	settings_->storeString("jid", username);
	settings_->storeString("certificate", certificateFile);
	settings_->storeString("pass", remember ? password : "");

	delete client_;
	jid_ = JID(username);
	client_ = new Swift::Client(jid_, password);
	//client_->onDataRead.connect(&printIncomingData);
	//client_->onDataWritten.connect(&printOutgoingData);
	if (!certificateFile.isEmpty()) {
		client_->setCertificate(certificateFile);
	}
	client_->onError.connect(boost::bind(&MainController::handleError, this, _1));
	client_->onConnected.connect(boost::bind(&MainController::handleConnected, this));
	client_->onMessageReceived.connect(boost::bind(&MainController::handleIncomingMessage, this, _1));
	client_->connect();
}

void MainController::handleError(const ClientError& error) {
	String message;
	switch(error.getType()) {
		case ClientError::NoError: assert(false); break;
		case ClientError::DomainNameResolveError: message = "Unable to find server"; break;
		case ClientError::ConnectionError: message = "Error connecting to server"; break;
		case ClientError::ConnectionReadError: message = "Error while receiving server data"; break;
		case ClientError::XMLError: message = "Error parsing server data"; break;
		case ClientError::AuthenticationFailedError: message = "Login/password invalid"; break;
		case ClientError::NoSupportedAuthMechanismsError: message = "Authentication mechanisms not supported"; break;
		case ClientError::UnexpectedElementError: message = "Unexpected response"; break;
		case ClientError::ResourceBindError: message = "Error binding resource"; break;
		case ClientError::SessionStartError: message = "Error starting session"; break;
		case ClientError::TLSError: message = "Encryption error"; break;
		case ClientError::ClientCertificateLoadError: message = "Error loading certificate (Invalid password?)"; break;
		case ClientError::ClientCertificateError: message = "Certificate not authorized"; break;
	}
	loginWindow_->setMessage(message);
	logout();
}

void MainController::logout() {
	loginWindow_->loggedOut();

	delete discoResponder_;
	discoResponder_ = NULL;
	delete clientVersionResponder_;
	clientVersionResponder_ = NULL;
	foreach (JIDChatControllerPair controllerPair, chatControllers_) {
		delete controllerPair.second;
	}
	client_->disconnect();
	chatControllers_.clear();
	foreach (JIDMUCControllerPair controllerPair, mucControllers_) {
		delete controllerPair.second;
	}
	mucControllers_.clear();
}


void MainController::handleChatRequest(const String &contact) {
	getChatController(JID(contact))->showChatWindow();
}

ChatController* MainController::getChatController(const JID &contact) {
	JID lookupContact(contact);
	if (chatControllers_.find(lookupContact) == chatControllers_.end()) {
		lookupContact = JID(contact.toBare());
	}
	if (chatControllers_.find(lookupContact) == chatControllers_.end()) {
		chatControllers_[contact] = new ChatController(jid_, client_, client_, chatWindowFactory_, contact, nickResolver_, presenceOracle_, avatarManager_);
		chatControllers_[contact]->setAvailableServerFeatures(serverDiscoInfo_);
		lookupContact = contact;
	}
	return chatControllers_[lookupContact];
}

void MainController::handleChatControllerJIDChanged(const JID& from, const JID& to) {
	chatControllers_[to] = chatControllers_[from];
	chatControllers_.erase(from);
}

void MainController::handleJoinMUCRequest(const JID &muc, const String &nick) {
	mucControllers_[muc] = new MUCController(jid_, muc, nick, client_, client_, chatWindowFactory_, treeWidgetFactory_, presenceOracle_, avatarManager_);
	mucControllers_[muc]->setAvailableServerFeatures(serverDiscoInfo_);
}

void MainController::handleIncomingMessage(boost::shared_ptr<Message> message) {
	JID jid = message->getFrom();
	boost::shared_ptr<MessageEvent> event(new MessageEvent(message));

	// Try to deliver it to a MUC
	if (message->getType() == Message::Groupchat || message->getType() == Message::Error) {
		std::map<JID, MUCController*>::iterator i = mucControllers_.find(jid.toBare());
		if (i != mucControllers_.end()) {
			i->second->handleIncomingMessage(event);
			return;
		}
		else if (message->getType() == Message::Groupchat) {
			//FIXME: Error handling - groupchat messages from an unknown muc.
			return;
		}
	}
	
	//if not a mucroom
	eventController_->handleIncomingEvent(event);

	// FIXME: This logic should go into a chat manager
	if (event->isReadable()) {
		getChatController(jid)->handleIncomingMessage(event);
	}
}

void MainController::handleServerDiscoInfoResponse(boost::shared_ptr<DiscoInfo> info, const boost::optional<Error>& error) {
	if (!error) {
		serverDiscoInfo_ = info;
		foreach (JIDChatControllerPair pair, chatControllers_) {
			pair.second->setAvailableServerFeatures(info);
		}
		foreach (JIDMUCControllerPair pair, mucControllers_) {
			pair.second->setAvailableServerFeatures(info);
		}
	}
}

bool MainController::isMUC(const JID& jid) const {
  return mucControllers_.find(jid.toBare()) != mucControllers_.end();
}

void MainController::handleOwnVCardReceived(boost::shared_ptr<VCard> vCard, const boost::optional<Error>& error) {
	if (!error && !vCard->getPhoto().isEmpty()) {
		vCardPhotoHash_ = SHA1::getHexHash(vCard->getPhoto());
		if (lastSentPresence_) {
			sendPresence(lastSentPresence_);
		}
		avatarManager_->setAvatar(jid_, vCard->getPhoto());
	}
}

}

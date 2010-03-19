#include "Swift/Controllers/MainController.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <stdlib.h>

#include "Swiften/Application/Application.h"
#include "Swiften/Application/ApplicationMessageDisplay.h"
#include "Swift/Controllers/Chat/ChatController.h"
#include "Swift/Controllers/UIInterfaces/ChatWindowFactory.h"
#include "Swift/Controllers/Chat/ChatsManager.h"
#include "Swift/Controllers/EventController.h"
#include "Swift/Controllers/EventWindowController.h"
#include "Swift/Controllers/UIInterfaces/LoginWindow.h"
#include "Swift/Controllers/UIInterfaces/LoginWindowFactory.h"
#include "Swift/Controllers/UIInterfaces/EventWindowFactory.h"
#include "Swift/Controllers/MainWindow.h"
#include "Swift/Controllers/MainWindowFactory.h"
#include "Swift/Controllers/Chat/MUCController.h"
#include "Swift/Controllers/NickResolver.h"
#include "Swift/Controllers/ProfileSettingsProvider.h"
#include "Swift/Controllers/RosterController.h"
#include "Swift/Controllers/SoundEventController.h"
#include "Swift/Controllers/SoundPlayer.h"
#include "Swift/Controllers/SystemTray.h"
#include "Swift/Controllers/SystemTrayController.h"
#include "Swift/Controllers/XMLConsoleController.h"
#include "Swift/Controllers/XMPPRosterController.h"
#include "Swift/Controllers/UIInterfaces/XMLConsoleWidgetFactory.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swiften/Base/foreach.h"
#include "Swiften/Base/String.h"
#include "Swiften/Client/Client.h"
#include "Swiften/Presence/PresenceSender.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/VCardUpdate.h"
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
#include "Swiften/StringCodecs/Hexify.h"

namespace Swift {

static const String CLIENT_NAME = "Swift";
static const String CLIENT_VERSION = "0.3";
static const String CLIENT_NODE = "http://swift.im";


MainController::MainController(ChatWindowFactory* chatWindowFactory, MainWindowFactory *mainWindowFactory, LoginWindowFactory *loginWindowFactory, TreeWidgetFactory *treeWidgetFactory, EventWindowFactory* eventWindowFactory, SettingsProvider *settings, Application* application, SystemTray* systemTray, SoundPlayer* soundPlayer, XMLConsoleWidgetFactory* xmlConsoleWidgetFactory)
		: timerFactory_(&boostIOServiceThread_.getIOService()), idleDetector_(&idleQuerier_, &timerFactory_, 100), client_(NULL), presenceSender_(NULL), chatWindowFactory_(chatWindowFactory), mainWindowFactory_(mainWindowFactory), loginWindowFactory_(loginWindowFactory), treeWidgetFactory_(treeWidgetFactory), settings_(settings), xmppRosterController_(NULL), rosterController_(NULL), loginWindow_(NULL), clientVersionResponder_(NULL), nickResolver_(NULL), discoResponder_(NULL) {
	application_ = application;
	presenceOracle_ = NULL;
	avatarManager_ = NULL;
	chatsManager_ = NULL;
	eventController_ = NULL;
	eventWindowFactory_ = eventWindowFactory;
	uiEventStream_ = new UIEventStream();

	avatarStorage_ = new AvatarFileStorage(application_->getAvatarDir());
	eventController_ = new EventController();
	eventController_->onEventQueueLengthChange.connect(boost::bind(&MainController::handleEventQueueLengthChange, this, _1));

	systemTrayController_ = new SystemTrayController(eventController_, systemTray);
	soundEventController_ = new SoundEventController(eventController_, soundPlayer, settings->getBoolSetting("playSounds", true));
	loginWindow_ = loginWindowFactory_->createLoginWindow(uiEventStream_);
	foreach (String profile, settings->getAvailableProfiles()) {
		ProfileSettingsProvider* profileSettings = new ProfileSettingsProvider(profile, settings);
		loginWindow_->addAvailableAccount(profileSettings->getStringSetting("jid"), profileSettings->getStringSetting("pass"), profileSettings->getStringSetting("certificate"));
		delete profileSettings;
	}
	loginWindow_->selectUser(settings_->getStringSetting("lastLoginJID"));
	loginWindow_->onLoginRequest.connect(boost::bind(&MainController::handleLoginRequest, this, _1, _2, _3, _4));
	loginWindow_->onCancelLoginRequest.connect(boost::bind(&MainController::handleCancelLoginRequest, this));

	idleDetector_.setIdleTimeSeconds(600);
	idleDetector_.onIdleChanged.connect(boost::bind(&MainController::handleInputIdleChanged, this, _1));

	xmlConsoleController_ = new XMLConsoleController(uiEventStream_, xmlConsoleWidgetFactory);
}

MainController::~MainController() {
	delete systemTrayController_;
	delete soundEventController_;
	delete avatarStorage_;
	delete xmlConsoleController_;
	delete uiEventStream_;
	delete eventController_;
	resetClient();
}

void MainController::resetClient() {
	serverDiscoInfo_ = boost::shared_ptr<DiscoInfo>();
	xmppRoster_ = boost::shared_ptr<XMPPRoster>();
	delete chatsManager_;
	chatsManager_ = NULL;
	delete presenceOracle_;
	presenceOracle_ = NULL;
	delete nickResolver_;
	nickResolver_ = NULL;
	delete avatarManager_;
	avatarManager_ = NULL;
	delete eventWindowController_;
	eventWindowController_ = NULL;
	delete rosterController_;
	rosterController_ = NULL;
	delete xmppRosterController_;
	xmppRosterController_ = NULL;
	delete clientVersionResponder_;
	clientVersionResponder_ = NULL;
	delete discoResponder_;
	discoResponder_ = NULL;
	delete presenceSender_;
	presenceSender_ = NULL;
	delete client_;
	client_ = NULL;
	
}

void MainController::handleConnected() {
	//FIXME: this freshLogin thing is temporary so I can see what's what before I split into a seperate method.
	bool freshLogin = rosterController_ == NULL;
	if (freshLogin) {
		serverDiscoInfo_ = boost::shared_ptr<DiscoInfo>(new DiscoInfo());
		xmppRoster_ = boost::shared_ptr<XMPPRoster>(new XMPPRoster());
		presenceOracle_ = new PresenceOracle(client_);
		nickResolver_ = new NickResolver(xmppRoster_);		
		lastSentPresence_ = boost::shared_ptr<Presence>();

		client_->onPresenceReceived.connect(boost::bind(&MainController::handleIncomingPresence, this, _1));

		chatsManager_ = new ChatsManager(jid_, client_, client_, eventController_, chatWindowFactory_, treeWidgetFactory_, nickResolver_, presenceOracle_, serverDiscoInfo_, presenceSender_);
		client_->onMessageReceived.connect(boost::bind(&ChatsManager::handleIncomingMessage, chatsManager_, _1));

		avatarManager_ = new AvatarManager(client_, client_, avatarStorage_, chatsManager_);

		chatsManager_->setAvatarManager(avatarManager_);


		rosterController_ = new RosterController(jid_, xmppRoster_, avatarManager_, mainWindowFactory_, treeWidgetFactory_, nickResolver_);
		rosterController_->onChangeStatusRequest.connect(boost::bind(&MainController::handleChangeStatusRequest, this, _1, _2));
		rosterController_->onSignOutRequest.connect(boost::bind(&MainController::signOut, this));
				rosterController_->onStartChatRequest.connect(boost::bind(&ChatsManager::handleChatRequest, chatsManager_, _1));
		rosterController_->onJoinMUCRequest.connect(boost::bind(&ChatsManager::handleJoinMUCRequest, chatsManager_, _1, _2));

		xmppRosterController_ = new XMPPRosterController(client_, xmppRoster_);
		xmppRosterController_->requestRoster();

		eventWindowController_ = new EventWindowController(eventController_, eventWindowFactory_);

		clientVersionResponder_ = new SoftwareVersionResponder(CLIENT_NAME, CLIENT_VERSION, client_);
		loginWindow_->morphInto(rosterController_->getWindow());

		DiscoInfo discoInfo;
		discoInfo.addIdentity(DiscoInfo::Identity(CLIENT_NAME, "client", "pc"));
		discoInfo.addFeature("urn:xmpp:sec-label:0");
		capsInfo_ = boost::shared_ptr<CapsInfo>(new CapsInfo(CapsInfoGenerator(CLIENT_NODE).generateCapsInfo(discoInfo)));

		discoResponder_ = new DiscoInfoResponder(client_);
		discoResponder_->setDiscoInfo(discoInfo);
		discoResponder_->setDiscoInfo(capsInfo_->getNode() + "#" + capsInfo_->getVersion(), discoInfo);
		serverDiscoInfo_ = boost::shared_ptr<DiscoInfo>(new DiscoInfo());
	}
	
	boost::shared_ptr<GetDiscoInfoRequest> discoInfoRequest(new GetDiscoInfoRequest(JID(), client_));
	discoInfoRequest->onResponse.connect(boost::bind(&MainController::handleServerDiscoInfoResponse, this, _1, _2));
	discoInfoRequest->send();

	boost::shared_ptr<GetVCardRequest> vCardRequest(new GetVCardRequest(JID(), client_));
	vCardRequest->onResponse.connect(boost::bind(&MainController::handleOwnVCardReceived, this, _1, _2));
	vCardRequest->send();

	//Send presence last to catch all the incoming presences.
	boost::shared_ptr<Presence> initialPresence;
	if (queuedPresence_.get() != NULL) {
		initialPresence = queuedPresence_;
	} else {
		initialPresence = boost::shared_ptr<Presence>(new Presence());
	}
	initialPresence->addPayload(capsInfo_);
	setManagersEnabled(true);
	sendPresence(initialPresence);
}

void MainController::handleEventQueueLengthChange(int count) {
	application_->getApplicationMessageDisplay()->setMessage(count == 0 ? "" : boost::lexical_cast<std::string>(count).c_str());
}

void MainController::reconnectAfterError() {
	performLoginFromCachedCredentials();
	sendPresence(queuedPresence_);
}

void MainController::handleChangeStatusRequest(StatusShow::Type show, const String &statusText) {
	boost::shared_ptr<Presence> presence(new Presence());
	if (show == StatusShow::None) {
		// FIXME: This is wrong. None doesn't mean unavailable
		presence->setType(Presence::Unavailable);
	}
	else {
		presence->setShow(show);
	}
	presence->setStatus(statusText);
	if (presence->getType() != Presence::Unavailable && !client_->isAvailable()) {
		performLoginFromCachedCredentials();
		queuedPresence_ = presence;
	} 
	else {
		sendPresence(presence);
	}
}

void MainController::sendPresence(boost::shared_ptr<Presence> presence) {
	// Copy presence before adding extra information
	lastSentPresence_ = presence->clone();

	// Add information and send
	if (!vCardPhotoHash_.isEmpty()) {
		presence->addPayload(boost::shared_ptr<VCardUpdate>(new VCardUpdate(vCardPhotoHash_)));
	}
	presence->addPayload(capsInfo_);
	presenceSender_->sendPresence(presence);
	if (presence->getType() == Presence::Unavailable) {
		logout();
	}
}

void MainController::handleInputIdleChanged(bool idle) {
	if (!client_ || !client_->isAvailable()) {
		return;
	}
	if (idle) {
		preIdlePresence_ = lastSentPresence_;
		boost::shared_ptr<Presence> presence(new Presence());
		presence->setShow(StatusShow::Away);
		presence->setStatus("Auto-away");
		sendPresence(presence);
	}
	else {
		if (client_) {
			sendPresence(preIdlePresence_);
		} 
		else {
			queuedPresence_ = preIdlePresence_;
		}
	}
}

void MainController::handleIncomingPresence(boost::shared_ptr<Presence> presence) {
	//FIXME: subscribe, subscribed
	rosterController_->handleIncomingPresence(presence);
}

void MainController::handleLoginRequest(const String &username, const String &password, const String& certificateFile, bool remember) {
	loginWindow_->setMessage("");
	ProfileSettingsProvider* profileSettings = new ProfileSettingsProvider(username, settings_);
	profileSettings->storeString("jid", username);
	profileSettings->storeString("certificate", certificateFile);
	profileSettings->storeString("pass", remember ? password : "");
	settings_->storeString("lastLoginJID", username);
	loginWindow_->addAvailableAccount(profileSettings->getStringSetting("jid"), profileSettings->getStringSetting("pass"), profileSettings->getStringSetting("certificate"));
	delete profileSettings;
	jid_ = JID(username);
	password_ = password;
	certificateFile_ = certificateFile;
	performLoginFromCachedCredentials();
}

void MainController::performLoginFromCachedCredentials() {
	if (!client_) {
		client_ = new Swift::Client(jid_, password_);
		presenceSender_ = new PresenceSender(client_);
		client_->onDataRead.connect(boost::bind(
				&XMLConsoleController::handleDataRead, xmlConsoleController_, _1));
		client_->onDataWritten.connect(boost::bind(
				&XMLConsoleController::handleDataWritten, xmlConsoleController_, _1));
		if (!certificateFile_.isEmpty()) {
			client_->setCertificate(certificateFile_);
		}
		client_->onError.connect(boost::bind(&MainController::handleError, this, _1));
		client_->onConnected.connect(boost::bind(&MainController::handleConnected, this));
	}
	client_->connect();
}

void MainController::handleError(const ClientError& error) {
	String message;
	switch(error.getType()) {
		case ClientError::UnknownError: message = "Unknown Error"; break;
		case ClientError::DomainNameResolveError: message = "Unable to find server"; break;
		case ClientError::ConnectionError: message = "Error connecting to server"; break;
		case ClientError::ConnectionReadError: message = "Error while receiving server data"; break;
		case ClientError::ConnectionWriteError: message = "Error while sending data to the server"; break;
		case ClientError::XMLError: message = "Error parsing server data"; break;
		case ClientError::AuthenticationFailedError: message = "Login/password invalid"; break;
		case ClientError::CompressionFailedError: message = "Error while compressing stream"; break;
		case ClientError::ServerVerificationFailedError: message = "Server verification failed"; break;
		case ClientError::NoSupportedAuthMechanismsError: message = "Authentication mechanisms not supported"; break;
		case ClientError::UnexpectedElementError: message = "Unexpected response"; break;
		case ClientError::ResourceBindError: message = "Error binding resource"; break;
		case ClientError::SessionStartError: message = "Error starting session"; break;
		case ClientError::TLSError: message = "Encryption error"; break;
		case ClientError::ClientCertificateLoadError: message = "Error loading certificate (Invalid password?)"; break;
		case ClientError::ClientCertificateError: message = "Certificate not authorized"; break;
	}
	if (!rosterController_) { //hasn't been logged in yet
		signOut();
		loginWindow_->setMessage(message);
	}
	logout();
	if (rosterController_) {
		//reconnectAfterError();
	}
}

void MainController::handleCancelLoginRequest() {
	signOut();
}

void MainController::signOut() {
	logout();
	loginWindow_->loggedOut();
	delete rosterController_;
	rosterController_ = NULL;
	resetClient();
}

void MainController::logout() {
	if (client_ && client_->isAvailable()) {
		client_->disconnect();
	}
	if (rosterController_) {
		rosterController_->getWindow()->setMyStatusType(StatusShow::None);
		rosterController_->getWindow()->setMyStatusText("");
	}
	setManagersEnabled(false);
}

void MainController::setManagersEnabled(bool enabled) {
	if (chatsManager_) {
		chatsManager_->setEnabled(enabled);
	}
	if (rosterController_) {
		rosterController_->setEnabled(enabled);
	}
}



void MainController::handleServerDiscoInfoResponse(boost::shared_ptr<DiscoInfo> info, const boost::optional<ErrorPayload>& error) {
	if (!error) {
		serverDiscoInfo_ = info;
		chatsManager_->setServerDiscoInfo(info);
	}
}

void MainController::handleOwnVCardReceived(boost::shared_ptr<VCard> vCard, const boost::optional<ErrorPayload>& error) {
	if (!error && !vCard->getPhoto().isEmpty()) {
		vCardPhotoHash_ = Hexify::hexify(SHA1::getHash(vCard->getPhoto()));
		if (lastSentPresence_) {
			sendPresence(lastSentPresence_);
		}
		avatarManager_->setAvatar(jid_, vCard->getPhoto());
	}
}

}

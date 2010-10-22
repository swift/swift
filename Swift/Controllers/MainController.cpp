/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/MainController.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <stdlib.h>
#include <sstream>

#include "Swiften/Network/BoostTimerFactory.h"
#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Network/MainBoostIOServiceThread.h"
#include "Swift/Controllers/BuildVersion.h"
#include "Swiften/VCards/VCardStorageFactory.h"
#include "Swiften/VCards/VCardManager.h"
#include "Swiften/VCards/VCardStorage.h"
#include "Swift/Controllers/Chat/MUCSearchController.h"
#include "Swift/Controllers/Chat/ChatsManager.h"
#include "Swift/Controllers/XMPPEvents/EventController.h"
#include "Swift/Controllers/EventWindowController.h"
#include "Swift/Controllers/UIInterfaces/LoginWindow.h"
#include "Swift/Controllers/UIInterfaces/LoginWindowFactory.h"
#include "Swift/Controllers/UIInterfaces/MainWindow.h"
#include "Swift/Controllers/Chat/MUCController.h"
#include "Swift/Controllers/NickResolver.h"
#include "Swift/Controllers/RosterController.h"
#include "Swift/Controllers/SoundEventController.h"
#include "Swift/Controllers/SoundPlayer.h"
#include "Swift/Controllers/StatusTracker.h"
#include "Swift/Controllers/SystemTray.h"
#include "Swift/Controllers/SystemTrayController.h"
#include "Swift/Controllers/XMLConsoleController.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/PresenceNotifier.h"
#include "Swift/Controllers/EventNotifier.h"
#include "SwifTools/Dock/Dock.h"
#include "Swiften/Base/foreach.h"
#include "Swiften/Base/String.h"
#include "Swiften/Client/Client.h"
#include "Swiften/Presence/PresenceSender.h"
#include "Swiften/Elements/ChatState.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/VCardUpdate.h"
#include "Swift/Controllers/Settings/SettingsProvider.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Disco/DiscoInfoResponder.h"
#include "Swiften/Disco/CapsInfoGenerator.h"
#include "Swiften/Disco/GetDiscoInfoRequest.h"
#include "Swiften/VCards/GetVCardRequest.h"
#include "Swiften/Avatars/AvatarStorage.h"
#include "Swiften/Avatars/AvatarManagerImpl.h"
#include "Swiften/Disco/CapsFileStorage.h"
#include "Swiften/Disco/CapsManager.h"
#include "Swiften/Disco/EntityCapsManager.h"
#include "Swiften/StringCodecs/SHA1.h"
#include "Swiften/StringCodecs/Hexify.h"
#include "Swift/Controllers/UIEvents/RequestChatUIEvent.h"
#include "Swift/Controllers/UIEvents/ToggleNotificationsUIEvent.h"

namespace Swift {

static const String CLIENT_NAME = "Swift";
//static const String CLIENT_VERSION = "1.0-beta1";
static const String CLIENT_VERSION = "1.0-devel";
static const String CLIENT_NODE = "http://swift.im";


MainController::MainController(
		ChatWindowFactory* chatWindowFactory,
		MainWindowFactory *mainWindowFactory,
		LoginWindowFactory *loginWindowFactory,
		EventWindowFactory* eventWindowFactory,
		SettingsProvider *settings,
		SystemTray* systemTray,
		SoundPlayer* soundPlayer,
		XMLConsoleWidgetFactory* xmlConsoleWidgetFactory,
		ChatListWindowFactory* chatListWindowFactory,
		MUCSearchWindowFactory* mucSearchWindowFactory,
		AvatarStorage* avatarStorage,
		CapsStorage* capsStorage,
		VCardStorageFactory* vcardStorageFactory,
		Dock* dock,
		Notifier* notifier,
		bool useDelayForLatency) :
			timerFactory_(&boostIOServiceThread_.getIOService()),
			idleDetector_(&idleQuerier_, &timerFactory_, 100),
			chatWindowFactory_(chatWindowFactory),
			mainWindowFactory_(mainWindowFactory),
			loginWindowFactory_(loginWindowFactory),
			settings_(settings),
			vcardStorageFactory_(vcardStorageFactory),
			loginWindow_(NULL) ,
			useDelayForLatency_(useDelayForLatency) {

	statusTracker_ = NULL;
	client_ = NULL;
	presenceSender_ = NULL;
	mucRegistry_ = NULL;
	vcardManager_ = NULL;
	avatarManager_ = NULL;
	capsManager_ = NULL;
	entityCapsManager_ = NULL;
	presenceNotifier_ = NULL;
	eventNotifier_ = NULL;
	nickResolver_ = NULL;
	rosterController_ = NULL;
	chatsManager_ = NULL;
	eventWindowController_ = NULL;
	discoResponder_ = NULL;
	mucSearchController_ = NULL;


	timeBeforeNextReconnect_ = -1;
	mucSearchWindowFactory_ = mucSearchWindowFactory;
	eventWindowFactory_ = eventWindowFactory;
	dock_ = dock;
	notifier_ = notifier;
	chatListWindowFactory_ = chatListWindowFactory;
	uiEventStream_ = new UIEventStream();

	avatarStorage_ = avatarStorage;
	capsStorage_ = capsStorage;
	eventController_ = new EventController();
	eventController_->onEventQueueLengthChange.connect(boost::bind(&MainController::handleEventQueueLengthChange, this, _1));

	systemTrayController_ = new SystemTrayController(eventController_, systemTray);
	loginWindow_ = loginWindowFactory_->createLoginWindow(uiEventStream_);
	soundEventController_ = new SoundEventController(eventController_, soundPlayer, settings, uiEventStream_);

	String selectedLoginJID = settings_->getStringSetting("lastLoginJID");
	bool loginAutomatically = settings_->getBoolSetting("loginAutomatically", false);
	String cachedPassword;
	String cachedCertificate;
	foreach (String profile, settings->getAvailableProfiles()) {
		ProfileSettingsProvider* profileSettings = new ProfileSettingsProvider(profile, settings);
		String password = profileSettings->getStringSetting("pass");
		String certificate = profileSettings->getStringSetting("certificate");
		String jid = profileSettings->getStringSetting("jid");
		loginWindow_->addAvailableAccount(jid, password, certificate);
		if (jid == selectedLoginJID) {
			cachedPassword = password;
			cachedCertificate = certificate;
		}
		delete profileSettings;
	}
	loginWindow_->selectUser(selectedLoginJID);
	loginWindow_->setLoginAutomatically(loginAutomatically);
	loginWindow_->onLoginRequest.connect(boost::bind(&MainController::handleLoginRequest, this, _1, _2, _3, _4, _5));
	loginWindow_->onCancelLoginRequest.connect(boost::bind(&MainController::handleCancelLoginRequest, this));

	idleDetector_.setIdleTimeSeconds(600);
	idleDetector_.onIdleChanged.connect(boost::bind(&MainController::handleInputIdleChanged, this, _1));

	xmlConsoleController_ = new XMLConsoleController(uiEventStream_, xmlConsoleWidgetFactory);

	bool enabled = settings_->getBoolSetting("showNotifications", true);
	notifier_->setEnabled(enabled);
	uiEventStream_->send(boost::shared_ptr<ToggleNotificationsUIEvent>(new ToggleNotificationsUIEvent(enabled)));
	uiEventStream_->onUIEvent.connect(boost::bind(&MainController::handleUIEvent, this, _1));

	if (loginAutomatically) {
		profileSettings_ = new ProfileSettingsProvider(selectedLoginJID, settings_);
		handleLoginRequest(selectedLoginJID, cachedPassword, cachedCertificate, true, true);
	} else {
		profileSettings_ = NULL;
	}
}

MainController::~MainController() {
	eventController_->disconnectAll();
	delete systemTrayController_;
	delete soundEventController_;
	delete xmlConsoleController_;
	delete uiEventStream_;
	resetClient();
	delete eventController_;
	for(VCardStorageMap::iterator i = vcardStorages_.begin(); i != vcardStorages_.end(); ++i) {
		delete i->second;
	}
}

void MainController::resetClient() {
	resetCurrentError();
	resetPendingReconnects();
	delete mucSearchController_;
	mucSearchController_ = NULL;
	if (discoResponder_) {
		discoResponder_->stop();
		delete discoResponder_;
		discoResponder_ = NULL;
	}
	delete eventWindowController_;
	eventWindowController_ = NULL;
	delete chatsManager_;
	chatsManager_ = NULL;
	delete rosterController_;
	rosterController_ = NULL;
	delete eventNotifier_;
	eventNotifier_ = NULL;
	delete presenceNotifier_;
	presenceNotifier_ = NULL;
	delete entityCapsManager_;
	entityCapsManager_ = NULL;
	delete capsManager_;
	capsManager_ = NULL;
	delete avatarManager_;
	avatarManager_ = NULL;
	delete nickResolver_;
	nickResolver_ = NULL;
	delete vcardManager_;
	vcardManager_ = NULL;
	delete mucRegistry_;
	mucRegistry_ = NULL;
	delete presenceSender_;
	presenceSender_ = NULL;
	delete client_;
	client_ = NULL;
	delete statusTracker_;
	statusTracker_ = NULL;
	delete profileSettings_;
	profileSettings_ = NULL;
}

void MainController::handleUIEvent(boost::shared_ptr<UIEvent> event) {
	boost::shared_ptr<ToggleNotificationsUIEvent> notificationsEvent = boost::dynamic_pointer_cast<ToggleNotificationsUIEvent>(event);
	if (notificationsEvent) {
		bool enabled = notificationsEvent->getEnabled();
		if (enabled != notifier_->getEnabled()) {
			notifier_->setEnabled(enabled);
			settings_->storeBool("showNotifications", enabled);
		}
	}
}

void MainController::resetPendingReconnects() {
	timeBeforeNextReconnect_ = -1;
	if (reconnectTimer_) {
		reconnectTimer_->stop();
		reconnectTimer_.reset();
	}
	resetCurrentError();
}

void MainController::resetCurrentError() {
	if (lastDisconnectError_) {
		lastDisconnectError_->conclude();
		lastDisconnectError_ = boost::shared_ptr<ErrorEvent>();
	}
}

void MainController::handleConnected() {
	boundJID_ = client_->getJID();
	loginWindow_->setIsLoggingIn(false);
	resetCurrentError();
	resetPendingReconnects();
	bool freshLogin = rosterController_ == NULL;
	myStatusLooksOnline_ = true;
	if (freshLogin) {
		rosterController_ = new RosterController(jid_, client_->getRoster(), avatarManager_, mainWindowFactory_, nickResolver_, client_->getPresenceOracle(), presenceSender_, eventController_, uiEventStream_, client_->getIQRouter(), settings_);
		rosterController_->onChangeStatusRequest.connect(boost::bind(&MainController::handleChangeStatusRequest, this, _1, _2));
		rosterController_->onSignOutRequest.connect(boost::bind(&MainController::signOut, this));

		chatsManager_ = new ChatsManager(jid_, client_->getStanzaChannel(), client_->getIQRouter(), eventController_, chatWindowFactory_, nickResolver_, client_->getPresenceOracle(), presenceSender_, uiEventStream_, chatListWindowFactory_, useDelayForLatency_, &timerFactory_, mucRegistry_, entityCapsManager_);
		client_->onMessageReceived.connect(boost::bind(&ChatsManager::handleIncomingMessage, chatsManager_, _1));
		chatsManager_->setAvatarManager(avatarManager_);

		eventWindowController_ = new EventWindowController(eventController_, eventWindowFactory_);

		loginWindow_->morphInto(rosterController_->getWindow());

		DiscoInfo discoInfo;
		discoInfo.addIdentity(DiscoInfo::Identity(CLIENT_NAME, "client", "pc"));
		discoInfo.addFeature("urn:xmpp:sec-label:0");
		discoInfo.addFeature(ChatState::getFeatureNamespace());
		capsInfo_ = boost::shared_ptr<CapsInfo>(new CapsInfo(CapsInfoGenerator(CLIENT_NODE).generateCapsInfo(discoInfo)));

		discoResponder_ = new DiscoInfoResponder(client_->getIQRouter());
		discoResponder_->setDiscoInfo(discoInfo);
		discoResponder_->setDiscoInfo(capsInfo_->getNode() + "#" + capsInfo_->getVersion(), discoInfo);
		discoResponder_->start();

		mucSearchController_ = new MUCSearchController(jid_, uiEventStream_, mucSearchWindowFactory_, client_->getIQRouter());
	}
	
	client_->requestRoster();

	GetDiscoInfoRequest::ref discoInfoRequest = GetDiscoInfoRequest::create(JID(), client_->getIQRouter());
	discoInfoRequest->onResponse.connect(boost::bind(&MainController::handleServerDiscoInfoResponse, this, _1, _2));
	discoInfoRequest->send();

	vcardManager_->requestOwnVCard();
	
	setManagersOnline(true);
	//Send presence last to catch all the incoming presences.
	sendPresence(statusTracker_->getNextPresence());
}

void MainController::handleEventQueueLengthChange(int count) {
	dock_->setMessage(count == 0 ? "" : boost::lexical_cast<std::string>(count).c_str());
}

void MainController::reconnectAfterError() {
	if (reconnectTimer_) {
		reconnectTimer_->stop();
	}
	performLoginFromCachedCredentials();
}

void MainController::handleChangeStatusRequest(StatusShow::Type show, const String &statusText) {
	boost::shared_ptr<Presence> presence(new Presence());
	if (show == StatusShow::None) {
		// Note: this is misleading, None doesn't mean unavailable on the wire.
		presence->setType(Presence::Unavailable);
		resetPendingReconnects();
		myStatusLooksOnline_ = false;
	}
	else {
		presence->setShow(show);
	}
	presence->setStatus(statusText);
	statusTracker_->setRequestedPresence(presence);
	if (presence->getType() != Presence::Unavailable) {
		profileSettings_->storeInt("lastShow", presence->getShow());
		profileSettings_->storeString("lastStatus", presence->getStatus());
	}
	if (presence->getType() != Presence::Unavailable && !client_->isAvailable()) {
		performLoginFromCachedCredentials();
	} else {
		sendPresence(presence);
	}
}

void MainController::sendPresence(boost::shared_ptr<Presence> presence) {
	rosterController_->getWindow()->setMyStatusType(presence->getShow());
	rosterController_->getWindow()->setMyStatusText(presence->getStatus());
	systemTrayController_->setMyStatusType(presence->getShow());
	notifier_->setEnabled(presence->getShow() != StatusShow::DND);

	// Add information and send
	if (!vCardPhotoHash_.isEmpty()) {
		presence->updatePayload(boost::shared_ptr<VCardUpdate>(new VCardUpdate(vCardPhotoHash_)));
	}
	presence->updatePayload(capsInfo_);
	presenceSender_->sendPresence(presence);
	if (presence->getType() == Presence::Unavailable) {
		logout();
	}
}

void MainController::handleInputIdleChanged(bool idle) {
	if (!statusTracker_) {
		//Haven't logged in yet.
		return;
	}
	if (idle) {
		if (statusTracker_->goAutoAway()) {
			if (client_ && client_->isAvailable()) {
				sendPresence(statusTracker_->getNextPresence());
			}
		}
	} else {
		if (statusTracker_->goAutoUnAway()) {
			if (client_ && client_->isAvailable()) {
				sendPresence(statusTracker_->getNextPresence());
			}
		}
	}
}

void MainController::handleLoginRequest(const String &username, const String &password, const String& certificateFile, bool remember, bool loginAutomatically) {
	loginWindow_->setMessage("");
	loginWindow_->setIsLoggingIn(true);
	profileSettings_ = new ProfileSettingsProvider(username, settings_);
	profileSettings_->storeString("jid", username);
	profileSettings_->storeString("certificate", certificateFile);
	profileSettings_->storeString("pass", (remember || loginAutomatically) ? password : "");
	settings_->storeString("lastLoginJID", username);
	settings_->storeBool("loginAutomatically", loginAutomatically);
	loginWindow_->addAvailableAccount(profileSettings_->getStringSetting("jid"), profileSettings_->getStringSetting("pass"), profileSettings_->getStringSetting("certificate"));
	jid_ = JID(username);
	password_ = password;
	certificateFile_ = certificateFile;
	performLoginFromCachedCredentials();
}

void MainController::performLoginFromCachedCredentials() {
	if (!statusTracker_) {
		statusTracker_  = new StatusTracker();
	}
	if (!client_) {
		client_ = new Swift::Client(jid_, password_);
		client_->onDataRead.connect(boost::bind(&XMLConsoleController::handleDataRead, xmlConsoleController_, _1));
		client_->onDataWritten.connect(boost::bind(&XMLConsoleController::handleDataWritten, xmlConsoleController_, _1));
		client_->onError.connect(boost::bind(&MainController::handleError, this, _1));
		client_->onConnected.connect(boost::bind(&MainController::handleConnected, this));

		client_->setSoftwareVersion(CLIENT_NAME, buildVersion);

		presenceSender_ = new PresenceSender(client_->getStanzaChannel());
		mucRegistry_ = new MUCRegistry();
		vcardManager_ = new VCardManager(jid_, client_->getIQRouter(), getVCardStorageForProfile(jid_));
		vcardManager_->onVCardChanged.connect(boost::bind(&MainController::handleVCardReceived, this, _1, _2));
		nickResolver_ = new NickResolver(this->jid_.toBare(), client_->getRoster(), vcardManager_, mucRegistry_);
		avatarManager_ = new AvatarManagerImpl(vcardManager_, client_->getStanzaChannel(), avatarStorage_, mucRegistry_);
		capsManager_ = new CapsManager(capsStorage_, client_->getStanzaChannel(), client_->getIQRouter());
		entityCapsManager_ = new EntityCapsManager(capsManager_, client_->getStanzaChannel());
		presenceNotifier_ = new PresenceNotifier(client_->getStanzaChannel(), notifier_, mucRegistry_, avatarManager_, nickResolver_, client_->getPresenceOracle(), &timerFactory_);
		presenceNotifier_->onNotificationActivated.connect(boost::bind(&MainController::handleNotificationClicked, this, _1));
		eventNotifier_ = new EventNotifier(eventController_, notifier_, avatarManager_, nickResolver_);
		eventNotifier_->onNotificationActivated.connect(boost::bind(&MainController::handleNotificationClicked, this, _1));
		if (!certificateFile_.isEmpty()) {
			client_->setCertificate(certificateFile_);
		}
		boost::shared_ptr<Presence> presence(new Presence());
		presence->setShow(static_cast<StatusShow::Type>(profileSettings_->getIntSetting("lastShow", StatusShow::Online)));
		presence->setStatus(profileSettings_->getStringSetting("lastStatus"));
		statusTracker_->setRequestedPresence(presence);
	} else {
		/* In case we're in the middle of another login, make sure they don't overlap */
		client_->disconnect();
	}
	/* If we logged in with a bare JID, and we have a full bound JID, re-login with the
	 * bound JID to try and keep dynamically assigned resources */
	if (boundJID_.isValid() && jid_.isBare() && boundJID_.toBare() == jid_) {
		client_->connect(boundJID_);
	} else {
		client_->connect();
	}
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
	} else {
		logout();
		setReconnectTimer();
		if (lastDisconnectError_) {
			std::stringstream ss;
			ss << "Reconnect to " << jid_.getDomain() << " failed: " << message << ". Will retry in " << timeBeforeNextReconnect_ << " seconds.";
			message = ss.str();
			lastDisconnectError_->conclude();
		} else {
			message = "Disconnected from " + jid_.getDomain() + ": " + message;
		}
		lastDisconnectError_ = boost::shared_ptr<ErrorEvent>(new ErrorEvent(JID(jid_.getDomain()), message));
		eventController_->handleIncomingEvent(lastDisconnectError_);
	}

}

void MainController::setReconnectTimer() {
	if (timeBeforeNextReconnect_ < 0) {
		timeBeforeNextReconnect_ = 1;
	} else {
		timeBeforeNextReconnect_ = timeBeforeNextReconnect_ >= 150 ? 300 : timeBeforeNextReconnect_ * 2; // Randomly selected by roll of a die, as required by 3920bis
	}
	if (reconnectTimer_) {
		reconnectTimer_->stop();
	}
	reconnectTimer_ = timerFactory_.createTimer(timeBeforeNextReconnect_ * 1000);
	reconnectTimer_->onTick.connect(boost::bind(&MainController::reconnectAfterError, this));
	reconnectTimer_->start();
}

void MainController::handleCancelLoginRequest() {
	signOut();
}

void MainController::signOut() {
	logout();
	loginWindow_->loggedOut();
	resetClient();
}

void MainController::logout() {
	systemTrayController_->setMyStatusType(StatusShow::None);
	if (client_ /*&& client_->isAvailable()*/) {
		client_->disconnect();
	}
	if (rosterController_ && myStatusLooksOnline_) {
		rosterController_->getWindow()->setMyStatusType(StatusShow::None);
		rosterController_->getWindow()->setMyStatusText("");
		myStatusLooksOnline_ = false;
	}
	setManagersOnline(false);
}

void MainController::setManagersOnline(bool enabled) {
	if (chatsManager_) {
		chatsManager_->setOnline(enabled);
	}
	if (rosterController_) {
		rosterController_->setEnabled(enabled);
	}
}



void MainController::handleServerDiscoInfoResponse(boost::shared_ptr<DiscoInfo> info, const boost::optional<ErrorPayload>& error) {
	if (!error) {
		chatsManager_->setServerDiscoInfo(info);
	}
}

void MainController::handleVCardReceived(const JID& jid, VCard::ref vCard) {
	if (!jid.equals(jid_, JID::WithoutResource) || !vCard || vCard->getPhoto().isEmpty()) {
		return;
	}
	vCardPhotoHash_ = Hexify::hexify(SHA1::getHash(vCard->getPhoto()));
	if (client_ && client_->isAvailable()) {
		sendPresence(statusTracker_->getNextPresence());
	}
}

void MainController::handleNotificationClicked(const JID& jid) {
	assert(chatsManager_);
	uiEventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(jid)));
}

VCardStorage* MainController::getVCardStorageForProfile(const JID& jid) {
	String profile = jid.toBare().toString();
	std::pair<VCardStorageMap::iterator, bool> r = vcardStorages_.insert(std::make_pair<String, VCardStorage*>(profile, NULL));
	if (r.second) {
		r.first->second = vcardStorageFactory_->createVCardStorage(profile);
	}
	return r.first->second;
}


}

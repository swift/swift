#include "Swift/Controllers/MUCController.h"

#include <boost/bind.hpp>

#include "Swiften/Base/foreach.h"
#include "Swift/Controllers/ChatWindow.h"
#include "Swift/Controllers/ChatWindowFactory.h"
#include "Swiften/MUC/MUC.h"
#include "Swiften/Client/StanzaChannel.h"
#include "Swiften/Roster/Roster.h"
#include "Swiften/Roster/SetPresence.h"
#include "Swiften/Roster/TreeWidgetFactory.h"

namespace Swift {
	
/**
 * The controller does not gain ownership of the stanzaChannel, nor the factory.
 */
MUCController::MUCController (
		const JID &muc, 
		const String &nick, 
		StanzaChannel* stanzaChannel, 
		IQRouter* iqRouter, 
		ChatWindowFactory* chatWindowFactory, 
		TreeWidgetFactory *treeWidgetFactory,
		PresenceOracle* presenceOracle) : 
			ChatControllerBase(stanzaChannel, iqRouter, chatWindowFactory, muc, presenceOracle),
			muc_(new MUC(stanzaChannel, muc)), 
			nick_(nick), 
			treeWidgetFactory_(treeWidgetFactory) { 
	roster_ = new Roster(chatWindow_->getTreeWidget(), treeWidgetFactory_);
	chatWindow_->onClosed.connect(boost::bind(&MUCController::handleWindowClosed, this));
	muc_->joinAs(nick);
	muc_->onOccupantJoined.connect(boost::bind(&MUCController::handleOccupantJoined, this, _1));
	muc_->onOccupantPresenceChange.connect(boost::bind(&MUCController::handleOccupantPresenceChange, this, _1));
	muc_->onOccupantLeft.connect(boost::bind(&MUCController::handleOccupantLeft, this, _1, _2, _3));
	chatWindow_->convertToMUC();
	chatWindow_->show();
}

MUCController::~MUCController() {
	delete muc_;
	//don't crash on exit by masking this. FIXME.
	//delete roster_;
}

void MUCController::handleWindowClosed() {
	muc_->part();
}

void MUCController::handleOccupantJoined(const MUCOccupant& occupant) {
	roster_->addContact(JID(toJID_.getNode(), toJID_.getDomain(), occupant.getNick()), occupant.getNick(), "Occupants");
}

void MUCController::handleOccupantLeft(const MUCOccupant& occupant, MUC::LeavingType, const String& /*reason*/) {
	roster_->removeContact(JID(toJID_.getNode(), toJID_.getDomain(), occupant.getNick()));
}

void MUCController::handleOccupantPresenceChange(boost::shared_ptr<Presence> presence) {
	roster_->applyOnItems(SetPresence(presence, JID::WithResource));
}

bool MUCController::isIncomingMessageFromMe(boost::shared_ptr<Message> message) {
	JID from = message->getFrom();
	return nick_ == from.getResource();
}

String MUCController::senderDisplayNameFromMessage(JID from) {
	return from.getResource();
}

void MUCController::preSendMessageRequest(boost::shared_ptr<Message> message) {
	message->setType(Swift::Message::Groupchat);
}

}

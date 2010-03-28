#include "Swiften/Chat/ChatStateMessageSender.h"

#include <boost/bind.hpp>

#include "Swiften/Client/StanzaChannel.h"

namespace Swift {

ChatStateMessageSender::ChatStateMessageSender(ChatStateNotifier* notifier, StanzaChannel* stanzaChannel, const JID& contact) : contact_(contact) {
	notifier_ = notifier;
	stanzaChannel_ = stanzaChannel;
	notifier_->onChatStateChanged.connect(boost::bind(&ChatStateMessageSender::handleChatStateChanged, this, _1));
}

void ChatStateMessageSender::handleChatStateChanged(ChatState::ChatStateType state) {
	boost::shared_ptr<Message> message(new Message());
	message->setTo(contact_);
	message->addPayload(boost::shared_ptr<Payload>(new ChatState(state)));
	stanzaChannel_->sendMessage(message);
}

}

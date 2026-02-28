/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Highlighting/Highlighter.h>

#include <set>
#include <string>

#include <Swiften/Base/String.h>
#include <Swiften/Base/format.h>
#include <Swiften/Client/NickResolver.h>

#include <Swift/Controllers/Highlighting/HighlightManager.h>
#include <Swift/Controllers/Intl.h>
#include <Swift/Controllers/XMPPEvents/MessageEvent.h>

namespace Swift {

Highlighter::Highlighter(HighlightManager* manager, NickResolver* nickResolver)
    : manager_(manager), nickResolver_(nickResolver) {
}

void Highlighter::handleSystemNotifications(const ChatWindow::ChatMessage& message, std::shared_ptr<MessageEvent> event) {
    if (std::shared_ptr<MessageEvent> messageEvent = std::dynamic_pointer_cast<MessageEvent>(event)) {
        JID jid = messageEvent->getStanza()->getFrom();
        std::string nickname = nickResolver_->jidToNick(jid);

        std::string messageText = messageEvent->getStanza()->getBody().get_value_or("");
        if (boost::starts_with(messageText, "/me ")) {
            messageText = "*" + String::getSplittedAtFirst(messageText, ' ').second + "*";
        }

        if (message.getHighlightActionDirectMessage().isSystemNotificationEnabled()) {
            // title: Romeo says
            // message: message
            std::string title = str(format(QT_TRANSLATE_NOOP("", "%1% says")) % nickname);
            event->addNotification(title, messageText);
        }
        if (message.getHighlightActionGroupMessage().isSystemNotificationEnabled()) {
            // title: Romeo in $roomJID says
            // message: message
            std::string roomName = jid.getNode();
            std::string title = str(format(QT_TRANSLATE_NOOP("", "%1% in %2% says")) % nickname % roomName);
            event->addNotification(title, messageText);
        }
        if (message.getHighlightActionOwnMention().isSystemNotificationEnabled()) {
            // title: Romeo mentioned you in $roomJID
            // message: message
            std::string roomName = jid.getNode();
            std::string title = str(format(QT_TRANSLATE_NOOP("", "%1% mentioned you in %2%")) % nickname % roomName);
            event->addNotification(title, messageText);
        }
        if (message.getHighlightActionSender().isSystemNotificationEnabled()) {
            // title: Romeo says
            // message: message
            auto title = str(format(QT_TRANSLATE_NOOP("", "%1% says")) % nickname);
            event->addNotification(title, messageText);
        }
        for (auto&& part : message.getParts()) {
            auto highlightPart = std::dynamic_pointer_cast<ChatWindow::ChatHighlightingMessagePart>(part);
            if (highlightPart && highlightPart->action.isSystemNotificationEnabled()) {
                // title: Romeo mentioned '$keyword'
                // message: message
                auto title = str(format(QT_TRANSLATE_NOOP("", "%1% mentioned '%2%'")) % nickname % highlightPart->text);
                event->addNotification(title, messageText);
            }
        }
    }
}

void Highlighter::handleSoundNotifications(const ChatWindow::ChatMessage& chatMessage) {
    std::set<std::string> playedSoundPaths;
    std::vector<HighlightAction> actionsToPlay;

    // collect unique sounds to play
    auto checkSoundActionAndQueueUnique = [&](const HighlightAction& action) {
        if (action.getSoundFilePath()) {
            auto soundFilePath = action.getSoundFilePath().get_value_or("");
            if (playedSoundPaths.find(soundFilePath) == playedSoundPaths.end()) {
                playedSoundPaths.insert(soundFilePath);
                actionsToPlay.push_back(action);
            }
        }
    };

    for (auto&& part : chatMessage.getParts()) {
        auto highlightMessage = std::dynamic_pointer_cast<ChatWindow::ChatHighlightingMessagePart>(part);
        if (highlightMessage) {
            checkSoundActionAndQueueUnique(highlightMessage->action);
        }
    }

    checkSoundActionAndQueueUnique(chatMessage.getHighlightActionSender());
    checkSoundActionAndQueueUnique(chatMessage.getHighlightActionOwnMention());
    checkSoundActionAndQueueUnique(chatMessage.getHighlightActionDirectMessage());
    checkSoundActionAndQueueUnique(chatMessage.getHighlightActionGroupMessage());

    // play sounds
    for (const auto& action : actionsToPlay) {
        manager_->onHighlight(action);
    }
}

}

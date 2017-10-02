/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <QWidget>

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

namespace Swift {
    class HighlightAction;
    class SecurityLabel;

    class QtChatView : public QWidget {
        Q_OBJECT
        public:
            QtChatView(QWidget* parent);
            virtual ~QtChatView();

            /** Add message to window.
             * @return id of added message (for acks).
             */
            virtual std::string addMessage(const ChatWindow::ChatMessage& message, const std::string& senderName, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time) = 0;
            /** Adds action to window.
             * @return id of added message (for acks);
             */
            virtual std::string addAction(const ChatWindow::ChatMessage& message, const std::string& senderName, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time) = 0;

            virtual std::string addSystemMessage(const ChatWindow::ChatMessage& message, ChatWindow::Direction direction) = 0;
            virtual void addPresenceMessage(const ChatWindow::ChatMessage& message, ChatWindow::Direction direction) = 0;

            virtual void addErrorMessage(const ChatWindow::ChatMessage& message) = 0;
            virtual void replaceMessage(const ChatWindow::ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time) = 0;
            virtual void replaceSystemMessage(const ChatWindow::ChatMessage& message, const std::string& id, const ChatWindow::TimestampBehaviour /*timestampBehaviour*/) = 0;
            virtual void replaceWithAction(const ChatWindow::ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time) = 0;
            virtual void replaceLastMessage(const ChatWindow::ChatMessage& message, const ChatWindow::TimestampBehaviour /*timestampBehaviour*/) = 0;
            virtual void setAckState(const std::string& id, ChatWindow::AckState state) = 0;

            virtual std::string addFileTransfer(const std::string& senderName, const std::string& avatarPath, bool senderIsSelf, const std::string& filename, const boost::uintmax_t sizeInBytes, const std::string& description) = 0;
            virtual void setFileTransferProgress(std::string, const int percentageDone) = 0;
            virtual void setFileTransferStatus(std::string, const ChatWindow::FileTransferState state, const std::string& msg = "") = 0;
            virtual void addMUCInvitation(const std::string& senderName, const JID& jid, const std::string& reason, const std::string& password, bool direct, bool isImpromptu, bool isContinuation) = 0;
            virtual std::string addWhiteboardRequest(const QString& contact, bool senderIsSelf) = 0;
            virtual void setWhiteboardSessionStatus(const std::string& id, const ChatWindow::WhiteboardSessionState state) = 0;
            virtual void setMessageReceiptState(const std::string& id, ChatWindow::ReceiptState state) = 0;

            virtual void showEmoticons(bool show) = 0;
            virtual void addLastSeenLine() = 0;

        public slots:
            virtual void resizeFont(int fontSizeSteps) = 0;
            virtual void scrollToBottom() = 0;
            virtual void handleKeyPressEvent(QKeyEvent* event) = 0;

    };
}

/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <QTextEdit>
#include <QWidget>

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

#include <Swift/QtUI/QtChatView.h>
#include <Swift/QtUI/QtChatWindow.h>

class QTextEdit;
class QProgressBar;

namespace Swift {
    class HighlightAction;
    class SecurityLabel;

    class QtPlainChatView : public QtChatView {
        Q_OBJECT
        public:
            QtPlainChatView(QtChatWindow *window, UIEventStream* eventStream);
            virtual ~QtPlainChatView();

            /** Add message to window.
             * @return id of added message (for acks).
             */
            virtual std::string addMessage(const ChatWindow::ChatMessage& /*message*/, const std::string& /*senderName*/, bool /*senderIsSelf*/, std::shared_ptr<SecurityLabel> /*label*/, const std::string& /*avatarPath*/, const boost::posix_time::ptime& /*time*/);
            /** Adds action to window.
             * @return id of added message (for acks);
             */
            virtual std::string addAction(const ChatWindow::ChatMessage& /*message*/, const std::string& /*senderName*/, bool /*senderIsSelf*/, std::shared_ptr<SecurityLabel> /*label*/, const std::string& /*avatarPath*/, const boost::posix_time::ptime& /*time*/);

            virtual std::string addSystemMessage(const ChatWindow::ChatMessage& /*message*/, ChatWindow::Direction /*direction*/);
            virtual void addPresenceMessage(const ChatWindow::ChatMessage& /*message*/, ChatWindow::Direction /*direction*/);
            virtual void addErrorMessage(const ChatWindow::ChatMessage& /*message*/);

            virtual void replaceMessage(const ChatWindow::ChatMessage& /*message*/, const std::string& /*id*/, const boost::posix_time::ptime& /*time*/);
            virtual void replaceWithAction(const ChatWindow::ChatMessage& /*message*/, const std::string& /*id*/, const boost::posix_time::ptime& /*time*/);
            virtual void replaceLastMessage(const ChatWindow::ChatMessage& /*message*/, const ChatWindow::TimestampBehaviour /*timestampBehaviour*/);
            virtual void replaceSystemMessage(const ChatWindow::ChatMessage& message, const std::string& id, const ChatWindow::TimestampBehaviour /*timestampBehaviour*/);
            virtual void setAckState(const std::string& /*id*/, ChatWindow::AckState /*state*/);

            virtual std::string addFileTransfer(const std::string& /*senderName*/, const std::string& /*avatarPath*/, bool /*senderIsSelf*/, const std::string& /*filename*/, const boost::uintmax_t /*sizeInBytes*/, const std::string& /*description*/);
            virtual void setFileTransferProgress(std::string, const int /*percentageDone*/);
            virtual void setFileTransferStatus(std::string, const ChatWindow::FileTransferState /*state*/, const std::string& /*msg*/ = "");
            virtual void addMUCInvitation(const std::string& /*senderName*/, const JID& /*jid*/, const std::string& /*reason*/, const std::string& /*password*/, bool /*direct*/, bool /*isImpromptu*/, bool /*isContinuation*/);
            virtual std::string addWhiteboardRequest(const QString& /*contact*/, bool /*senderIsSelf*/) {return "";}
            virtual void setWhiteboardSessionStatus(const std::string& /*id*/, const ChatWindow::WhiteboardSessionState /*state*/) {}
            virtual void setMessageReceiptState(const std::string& /*id*/, ChatWindow::ReceiptState /*state*/) {}

            virtual void showEmoticons(bool /*show*/) {}
            virtual void addLastSeenLine() {}

        public slots:
            virtual void resizeFont(int /*fontSizeSteps*/) {}
            virtual void scrollToBottom();
            virtual void handleKeyPressEvent(QKeyEvent* /*event*/) {}
            virtual void fileTransferAccept();
            virtual void fileTransferReject();
            virtual void fileTransferFinish();
            virtual void acceptMUCInvite();
            virtual void rejectMUCInvite();

        private:
            struct PopupDialog {
                PopupDialog(QtPlainChatView* parent) {
                    dialog_ = new QFrame(parent);
                    dialog_->setFrameShape(QFrame::Panel);
                    dialog_->setFrameShadow(QFrame::Raised);
                    layout_ = new QHBoxLayout;
                    dialog_->setLayout(layout_);
                }
                virtual ~PopupDialog() {
                    delete dialog_;
                }
                QFrame* dialog_;
                QHBoxLayout* layout_;
            };

            struct AcceptMUCInviteAction : public QPushButton {
                AcceptMUCInviteAction(PopupDialog* parent, const std::string& text, const JID& jid, const std::string& senderName, const std::string& password, bool isImpromptu, bool isContinuation)
                : QPushButton(P2QSTRING(text)), parent_(parent), jid_(jid), senderName_(senderName), password_(password), isImpromptu_(isImpromptu), isContinuation_(isContinuation) {}
                PopupDialog *parent_;
                JID jid_;
                std::string senderName_;
                std::string password_;
                bool isImpromptu_;
                bool isContinuation_;
            };

            struct FileTransfer : public PopupDialog {
                struct Action : QPushButton {
                    Action(const std::string& text, const std::string& id)
                    : QPushButton(P2QSTRING(text)), id_(id) {}
                    std::string id_;
                };
                FileTransfer(QtPlainChatView* parent, bool senderIsSelf, const std::string& ftId, const std::string& filename, const ChatWindow::FileTransferState state, const std::string& desc, const std::string& msg, bool initializing);
                QProgressBar* bar_;
                bool senderIsSelf_;
                std::string ftId_;
                std::string filename_;
                std::string description_;
                std::string message_;
                bool initializing_;
            };

            class LogTextEdit : public QTextEdit {
            public:
                LogTextEdit(QWidget* parent) : QTextEdit(parent) {}
                virtual ~LogTextEdit() {}
                virtual void contextMenuEvent(QContextMenuEvent *event);
            };

            typedef std::map<std::string, FileTransfer*> FileTransferMap;
            QtChatWindow* window_;
            UIEventStream* eventStream_;
            LogTextEdit* log_;
            FileTransferMap fileTransfers_;
            std::map<std::string, std::shared_ptr<SecurityLabel> > lastMessageLabel_;
            int idGenerator_;

    };
}

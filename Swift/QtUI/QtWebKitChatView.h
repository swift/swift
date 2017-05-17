/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <QList>
#include <QString>
#include <QWebElement>
#include <QWidget>

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

#include <Swift/QtUI/ChatSnippet.h>
#include <Swift/QtUI/QtChatView.h>

class QWebPage;
class QUrl;
class QDate;

namespace Swift {
    class QtWebView;
    class QtChatTheme;
    class QtChatWindowJSBridge;
    class UIEventStream;
    class QtChatWindow;
    class SettingsProvider;
    class QtWebKitChatView : public QtChatView {
            Q_OBJECT

        public:
            static const QString ButtonWhiteboardSessionCancel;
            static const QString ButtonWhiteboardSessionAcceptRequest;
            static const QString ButtonWhiteboardShowWindow;
            static const QString ButtonFileTransferCancel;
            static const QString ButtonFileTransferSetDescription;
            static const QString ButtonFileTransferSendRequest;
            static const QString ButtonFileTransferAcceptRequest;
            static const QString ButtonFileTransferOpenFile;
            static const QString ButtonMUCInvite;
            static const QString ButtonResendMessage;
            static const QString ButtonResendPopup;
        public:
            QtWebKitChatView(QtChatWindow* window, UIEventStream* eventStream, QtChatTheme* theme, QWidget* parent, SettingsProvider* settings, bool disableAutoScroll = false);
            ~QtWebKitChatView() override;

            /** Add message to window.
             * @return id of added message (for acks).
             */
            virtual std::string addMessage(const ChatWindow::ChatMessage& message, const std::string& senderName, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time) override;
            /** Adds action to window.
             * @return id of added message (for acks);
             */
            virtual std::string addAction(const ChatWindow::ChatMessage& message, const std::string& senderName, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time) override;

            virtual std::string addSystemMessage(const ChatWindow::ChatMessage& message, ChatWindow::Direction direction) override;
            virtual void addPresenceMessage(const ChatWindow::ChatMessage& message, ChatWindow::Direction direction) override;

            virtual void addErrorMessage(const ChatWindow::ChatMessage& message) override;
            virtual void replaceMessage(const ChatWindow::ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time) override;
            virtual void replaceSystemMessage(const ChatWindow::ChatMessage& message, const std::string& id, ChatWindow::TimestampBehaviour timestampBehaviour) override;
            virtual void replaceWithAction(const ChatWindow::ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time) override;
            virtual void replaceLastMessage(const ChatWindow::ChatMessage& message, const ChatWindow::TimestampBehaviour timestampBehaviour) override;
            virtual void setAckState(const std::string& id, ChatWindow::AckState state) override;

            virtual std::string addFileTransfer(const std::string& senderName, const std::string& avatarPath, bool senderIsSelf, const std::string& filename, const boost::uintmax_t sizeInBytes, const std::string& description) override;
            virtual void setFileTransferProgress(std::string, const int percentageDone) override;
            virtual void setFileTransferStatus(std::string, const ChatWindow::FileTransferState state, const std::string& msg = "") override;
            virtual void addMUCInvitation(const std::string& senderName, const JID& jid, const std::string& reason, const std::string& password, bool direct, bool isImpromptu, bool isContinuation) override;
            virtual std::string addWhiteboardRequest(const QString& contact, bool senderIsSelf) override;
            virtual void setWhiteboardSessionStatus(const std::string& id, const ChatWindow::WhiteboardSessionState state) override;
            virtual void setMessageReceiptState(const std::string& id, ChatWindow::ReceiptState state) override;

            virtual void showEmoticons(bool show) override;
            void addMessageTop(std::shared_ptr<ChatSnippet> snippet);
            void addMessageBottom(std::shared_ptr<ChatSnippet> snippet);

            int getSnippetPositionByDate(const QDate& date); // FIXME : This probably shouldn't have been public
            virtual void addLastSeenLine() override;

        private: // previously public, now private
            void replaceLastMessage(const QString& newMessage, const ChatWindow::TimestampBehaviour timestampBehaviour);
            void replaceLastMessage(const QString& newMessage, const QString& note);
            void replaceMessage(const QString& newMessage, const QString& id, const QDateTime& time);
            void replaceSystemMessage(const QString& newMessage, const QString&id, const ChatWindow::TimestampBehaviour timestampBehaviour);
            void rememberScrolledToBottom();
            void setAckXML(const QString& id, const QString& xml);
            void setReceiptXML(const QString& id, const QString& xml);
            void displayReceiptInfo(const QString& id, bool showIt);

            QString getLastSentMessage();
            void addToJSEnvironment(const QString&, QObject*);
            void setFileTransferProgress(QString id, const int percentageDone);
            void setFileTransferStatus(QString id, const ChatWindow::FileTransferState state, const QString& msg);
            void setFileTransferWarning(QString id, QString warningText);
            void removeFileTransferWarning(QString id);
            void setWhiteboardSessionStatus(QString id, const ChatWindow::WhiteboardSessionState state);
            void setMUCInvitationJoined(QString id);
            void askDesktopToOpenFile(const QString& filename);

        signals:
            void gotFocus();
            void fontResized(int);
            void logCleared();
            void scrollRequested(int pos);
            void scrollReachedTop();
            void scrollReachedBottom();

        public slots:
            void copySelectionToClipboard();
            void handleLinkClicked(const QUrl&);
            void resetView();
            void resetTopInsertPoint();
            void increaseFontSize(int numSteps = 1);
            void decreaseFontSize();
            virtual void resizeFont(int fontSizeSteps) override;
            virtual void scrollToBottom() override;
            virtual void handleKeyPressEvent(QKeyEvent* event) override;

        private slots:
            void handleViewLoadFinished(bool);
            void handleFrameSizeChanged();
            void handleClearRequested();
            void handleHTMLButtonClicked(QString id, QString arg1, QString arg2, QString arg3, QString arg4, QString arg5);
            void handleVerticalScrollBarPositionChanged(double position);

        private:
            enum PreviousMessageKind {
                PreviosuMessageWasNone,
                PreviousMessageWasMessage,
                PreviousMessageWasSystem,
                PreviousMessageWasPresence,
                PreviousMessageWasFileTransfer,
                PreviousMessageWasMUCInvite
            };
            std::string addMessage(
                    const QString& message,
                    const std::string& senderName,
                    bool senderIsSelf,
                    std::shared_ptr<SecurityLabel> label,
                    const std::string& avatarPath,
                    const QString& style,
                    const boost::posix_time::ptime& time,
                    const HighlightAction& highlight,
                    ChatSnippet::Direction direction);
            void replaceMessage(
                    const QString& message,
                    const std::string& id,
                    const boost::posix_time::ptime& time,
                    const QString& style,
                    const HighlightAction& highlight);
            bool appendToPreviousCheck(PreviousMessageKind messageKind, const std::string& senderName, bool senderIsSelf, const std::shared_ptr<SecurityLabel>& label = nullptr);
            static ChatSnippet::Direction getActualDirection(const ChatWindow::ChatMessage& message, ChatWindow::Direction direction);
            QString getHighlightSpanStart(const std::string& text, const std::string& background);
            QString getHighlightSpanStart(const HighlightAction& highlight);
            QString chatMessageToHTML(const ChatWindow::ChatMessage& message);
            static QString buildChatWindowButton(const QString& name, const QString& id, const QString& arg1 = QString(), const QString& arg2 = QString(), const QString& arg3 = QString(), const QString& arg4 = QString(), const QString& arg5 = QString());
            static QString buildChatWindowPopupImageButton(const QString& title, const QString& path, const QString& arg1 = QString(), const QString& arg2 = QString(), const QString& arg3 = QString(), const QString& arg4 = QString(), const QString& arg5 = QString());

        protected:
            void resizeEvent(QResizeEvent* event) override;

        private:
            void headerEncode();
            void messageEncode();
            void addToDOM(std::shared_ptr<ChatSnippet> snippet);

            QtChatWindow* window_;
            UIEventStream* eventStream_;
            bool viewReady_;
            bool isAtBottom_;
            bool topMessageAdded_;
            int scrollBarMaximum_;
            QtWebView* webView_;
            QWebPage* webPage_;
            int fontSizeSteps_;
            QtChatTheme* theme_;
            QWebElement lineSeparator_;
            QWebElement lastElement_;
            QWebElement firstElement_;
            QWebElement document_;
            bool disableAutoScroll_;
            QtChatWindowJSBridge* jsBridge;
            PreviousMessageKind previousMessageKind_;
            bool previousMessageWasSelf_;
            bool showEmoticons_;
            bool insertingLastLine_;
            int idCounter_;
            QString previousSenderName_;
            std::map<QString, QString> descriptions_;
            std::map<QString, QString> filePaths_;
            std::string previousMessageDisplayMarking_;
            SettingsProvider* settings_ = nullptr;
    };
}

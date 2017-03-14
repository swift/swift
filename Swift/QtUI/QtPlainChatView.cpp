/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtPlainChatView.h>

#include <QDialog>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollBar>
#include <QTextEdit>
#include <QVBoxLayout>

#include <Swiften/Base/FileSize.h>

#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>

#include <Swift/QtUI/ChatSnippet.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtUtilities.h>

namespace Swift {

QtPlainChatView::QtPlainChatView(QtChatWindow *window, UIEventStream* eventStream)
: QtChatView(window), window_(window), eventStream_(eventStream), idGenerator_(0) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    log_ = new LogTextEdit(this);
    log_->setReadOnly(true);
    log_->setAccessibleName(tr("Chat Messages"));
    mainLayout->addWidget(log_);
}

QtPlainChatView::~QtPlainChatView() {
}

QString chatMessageToString(const ChatWindow::ChatMessage& message) {
    QString result;
    for (auto&& part : message.getParts()) {
        std::shared_ptr<ChatWindow::ChatTextMessagePart> textPart;
        std::shared_ptr<ChatWindow::ChatURIMessagePart> uriPart;
        std::shared_ptr<ChatWindow::ChatEmoticonMessagePart> emoticonPart;
        std::shared_ptr<ChatWindow::ChatHighlightingMessagePart> highlightPart;

        if ((textPart = std::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(part))) {
            QString text = QtUtilities::htmlEscape(P2QSTRING(textPart->text));
            text.replace("\n","<br/>");
            result += text;
            continue;
        }
        if ((uriPart = std::dynamic_pointer_cast<ChatWindow::ChatURIMessagePart>(part))) {
            QString uri = QtUtilities::htmlEscape(P2QSTRING(uriPart->target));
            result += "<a href='" + uri + "' >" + uri + "</a>";
            continue;
        }
        if ((emoticonPart = std::dynamic_pointer_cast<ChatWindow::ChatEmoticonMessagePart>(part))) {
            result += P2QSTRING(emoticonPart->alternativeText);
            continue;
        }
        if ((highlightPart = std::dynamic_pointer_cast<ChatWindow::ChatHighlightingMessagePart>(part))) {
            //FIXME: Maybe do something here. Anything, really.
            continue;
        }
    }
    return result;
}

std::string QtPlainChatView::addMessage(const ChatWindow::ChatMessage& message, const std::string& senderName, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const std::string& /*avatarPath*/, const boost::posix_time::ptime& time) {
    QString text = "<p>";
    if (label) {
        text += P2QSTRING(label->getLabel()) + "<br/>";
    }
    QString name = senderIsSelf ? "you" : P2QSTRING(senderName);
    text += QString(tr("At %1 %2 said:")).arg(ChatSnippet::timeToEscapedString(B2QDATE(time))).arg(name) + "<br/>";
    text += chatMessageToString(message);
    text += "</p>";
    log_->append(text);
    const std::string idx = senderIsSelf ? "" : senderName;
    lastMessageLabel_[idx] = label;
    return idx;
}

std::string QtPlainChatView::addAction(const ChatWindow::ChatMessage& message, const std::string& senderName, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const std::string& /*avatarPath*/, const boost::posix_time::ptime& time) {
    QString text = "<p>";
    if (label) {
        text += P2QSTRING(label->getLabel()) + "<br/>";
    }
    QString name = senderIsSelf ? "you" : P2QSTRING(senderName);
    text += QString(tr("At %1 <i>%2 ")).arg(ChatSnippet::timeToEscapedString(B2QDATE(time))).arg(name);
    text += chatMessageToString(message);
    text += "</i></p>";
    log_->append(text);
    const std::string idx = senderIsSelf ? "" : senderName;
    lastMessageLabel_[idx] = label;
    return idx;
}

std::string QtPlainChatView::addSystemMessage(const ChatWindow::ChatMessage& message, ChatWindow::Direction /*direction*/)
{
    QString text = "<p><i>";
    text += chatMessageToString(message);
    text += "</i></p>";
    log_->append(text);
    return "";
}

void QtPlainChatView::addPresenceMessage(const ChatWindow::ChatMessage& message, ChatWindow::Direction /*direction*/)
{
    QString text = "<p><i>";
    text += chatMessageToString(message);
    text += "</i></p>";
    log_->append(text);
}

void QtPlainChatView::addErrorMessage(const ChatWindow::ChatMessage& message)
{
    QString text = "<p><i>";
    text += chatMessageToString(message);
    text += "</i></p>";
    log_->append(text);
}

void QtPlainChatView::replaceMessage(const ChatWindow::ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time)
{
    QString text = "<p>";
    if (lastMessageLabel_[id]) {
        text += P2QSTRING(lastMessageLabel_[id]->getLabel()) + "<br/>";
    }
    QString name = id.empty() ? "you" : P2QSTRING(id);
    text += QString(tr("At %1 %2 corrected the last message to:")).arg(ChatSnippet::timeToEscapedString(B2QDATE(time))).arg(name) + "<br/>";
    text += chatMessageToString(message);
    text += "</p>";
    log_->append(text);
}

void QtPlainChatView::replaceWithAction(const ChatWindow::ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time)
{
    QString text = "<p>";
    if (lastMessageLabel_[id]) {
        text += P2QSTRING(lastMessageLabel_[id]->getLabel()) + "<br/>";
    }
    QString name = id.empty() ? "you" : P2QSTRING(id);
    text += QString(tr("At %1 %2 corrected the last action to: <i>")).arg(ChatSnippet::timeToEscapedString(B2QDATE(time))).arg(name);
    text += chatMessageToString(message);
    text += "</i></p>";
    log_->append(text);
}

void QtPlainChatView::replaceLastMessage(const ChatWindow::ChatMessage& message, const ChatWindow::TimestampBehaviour /*timestampBehaviour*/)
{
    QString text = "<p>The last message was corrected to:<br/>";
    text += chatMessageToString(message);
    text += "</p>";
    log_->append(text);
}

void QtPlainChatView::replaceSystemMessage(const ChatWindow::ChatMessage& message, const std::string& /*id*/, const ChatWindow::TimestampBehaviour /*timestampBehaviour*/) {
    QString text = "<p><i>";
    text += chatMessageToString(message);
    text += "</i></p>";
    log_->append(text);
}

void QtPlainChatView::setAckState(const std::string& /*id*/, ChatWindow::AckState state)
{
    if (state == ChatWindow::Failed) {
        addSystemMessage(ChatWindow::ChatMessage("Message delivery failed due to disconnection from server."), ChatWindow::DefaultDirection);
    }
}

std::string QtPlainChatView::addFileTransfer(const std::string& senderName, const std::string& /*avatarPath*/, bool senderIsSelf, const std::string& filename, const boost::uintmax_t sizeInBytes, const std::string& description)
{
    const std::string ftId = "ft" + boost::lexical_cast<std::string>(idGenerator_++);
    const std::string sizeString = formatSize(sizeInBytes);

    FileTransfer* transfer;
    if (senderIsSelf) {
        QString description = QInputDialog::getText(this, tr("File transfer description"),
            tr("Description:"), QLineEdit::Normal, "");
        /* NOTE: it is not possible to abort if description is not provided, since we must always return a valid transfer id */
        const std::string descriptionMessage = description.isEmpty() ? "" : (" \"" + Q2PSTRING(description) + "\"");
        const std::string message = std::string() + "Confirm file transfer: <i>" + filename + " (" + sizeString + " bytes)</i>" + descriptionMessage;
        transfer = new FileTransfer(this, senderIsSelf, ftId, filename, ChatWindow::WaitingForAccept, Q2PSTRING(description), message, true);
        addSystemMessage(ChatWindow::ChatMessage("Preparing to start file transfer..."), ChatWindow::DefaultDirection);
    } else { /* incoming transfer */
        const std::string descriptionMessage = description.empty() ? "" : (" \"" + description + "\"");
        const std::string message = std::string() + "Incoming file transfer: <i>" + filename + " (" + sizeString + " bytes)</i>" + descriptionMessage;
        transfer = new FileTransfer(this, senderIsSelf, ftId, filename, ChatWindow::WaitingForAccept, "", message, true);
        addSystemMessage("Incoming file transfer from " + senderName + "...", ChatWindow::DefaultDirection);
    }

    fileTransfers_[ftId] = transfer;
    layout()->addWidget(transfer->dialog_);

    return ftId;
}

void QtPlainChatView::setFileTransferProgress(std::string id, const int percentageDone)
{
    FileTransferMap::iterator transfer = fileTransfers_.find(id);
    if (transfer != fileTransfers_.end()) {
        transfer->second->bar_->setValue(percentageDone);
    }
}

void QtPlainChatView::setFileTransferStatus(std::string id, const ChatWindow::FileTransferState state, const std::string& msg)
{
    FileTransferMap::iterator transferIter = fileTransfers_.find(id);
    if (transferIter == fileTransfers_.end()) {
        return;
    }

    /* store the layout index so we can restore it to the same location */
    FileTransfer* oldTransfer = transferIter->second;
    const int layoutIndex = layout()->indexOf(oldTransfer->dialog_);
    layout()->removeWidget(oldTransfer->dialog_);
    const std::string &label = (!msg.empty() ? msg : oldTransfer->message_);
    FileTransfer* transfer = new FileTransfer(this, oldTransfer->senderIsSelf_, oldTransfer->ftId_, oldTransfer->filename_, state, oldTransfer->description_, label, false);
    fileTransfers_[oldTransfer->ftId_] = transfer; /* replace the transfer object for this file id */
    delete oldTransfer;

    /* insert the new dialog at the old position in the layout list */
    QBoxLayout* parentLayout = dynamic_cast<QBoxLayout*>(layout());
    assert(parentLayout);
    parentLayout->insertWidget(layoutIndex, transfer->dialog_);

    /* log the transfer end result as a system message */
    if (state == ChatWindow::Finished) {
        addSystemMessage(ChatWindow::ChatMessage("The file transfer completed successfully."), ChatWindow::DefaultDirection);
    } else if (state == ChatWindow::Canceled) {
        addSystemMessage(ChatWindow::ChatMessage("The file transfer was canceled."), ChatWindow::DefaultDirection);
    } else if (state == ChatWindow::FTFailed) {
        addSystemMessage(ChatWindow::ChatMessage("The file transfer failed."), ChatWindow::DefaultDirection);
    }
}

void QtPlainChatView::addMUCInvitation(const std::string& senderName, const JID& jid, const std::string& /*reason*/, const std::string& password, bool /*direct*/, bool isImpromptu, bool isContinuation)
{
    PopupDialog *invite = new PopupDialog(this);

    QLabel* statusLabel = new QLabel;
    std::string msg = senderName + " has invited you to join " + jid.toString() + "...";
    statusLabel->setText(P2QSTRING(msg));
    invite->layout_->addWidget(statusLabel);
    invite->layout_->addWidget(new QLabel); /* padding */

    AcceptMUCInviteAction* accept = new AcceptMUCInviteAction(invite, "Accept", jid, senderName, password, isImpromptu, isContinuation);
    connect(accept, SIGNAL(clicked()), SLOT(acceptMUCInvite()));
    invite->layout_->addWidget(accept);

    AcceptMUCInviteAction* reject = new AcceptMUCInviteAction(invite, "Reject", jid, senderName, password, isImpromptu, isContinuation);
    connect(reject, SIGNAL(clicked()), SLOT(rejectMUCInvite()));
    invite->layout_->addWidget(reject);
    statusLabel->setText(P2QSTRING(msg));

    layout()->addWidget(invite->dialog_);

    addSystemMessage(ChatWindow::ChatMessage(msg), ChatWindow::DefaultDirection);
}

void QtPlainChatView::scrollToBottom()
{
    log_->ensureCursorVisible();
    log_->verticalScrollBar()->setValue(log_->verticalScrollBar()->maximum());
}

void QtPlainChatView::fileTransferAccept()
{
    FileTransfer::Action* action = dynamic_cast<FileTransfer::Action*>(sender());
    if (!action) {
        return;
    }

    FileTransferMap::iterator transferIter = fileTransfers_.find(action->id_);
    if (transferIter == fileTransfers_.end()) {
        return;
    }

    FileTransfer* transfer = transferIter->second;

    const std::string message = transfer->message_;

    if (transfer->senderIsSelf_) { /* if we are the sender, kick of the transfer */
        window_->onFileTransferStart(transfer->ftId_, transfer->description_);
    } else { /* ask the user where to save the file first */
        QString path = QFileDialog::getSaveFileName(this, tr("Save File"), P2QSTRING(transfer->filename_));
        if (path.isEmpty()) {
            fileTransferReject(); /* because the user did not select a desintation path */
            return;
        }
        window_->onFileTransferAccept(transfer->ftId_, Q2PSTRING(path));
    }

    setFileTransferStatus(transfer->ftId_, ChatWindow::Negotiating, message);
}

void QtPlainChatView::fileTransferReject()
{
    FileTransfer::Action* action = dynamic_cast<FileTransfer::Action*>(sender());
    if (action) {
        window_->onFileTransferCancel(action->id_);
        fileTransferFinish();
    }
}

void QtPlainChatView::fileTransferFinish()
{
    FileTransfer::Action* action = dynamic_cast<FileTransfer::Action*>(sender());
    if (action) {
        FileTransferMap::iterator transferIter = fileTransfers_.find(action->id_);
        if (transferIter != fileTransfers_.end()) {
            delete transferIter->second; /* cause the dialog to close */
            fileTransfers_.erase(transferIter);
        }
    }
}

void QtPlainChatView::acceptMUCInvite()
{
    AcceptMUCInviteAction *action = dynamic_cast<AcceptMUCInviteAction*>(sender());
    if (action) {
        eventStream_->send(std::make_shared<JoinMUCUIEvent>(action->jid_.toString(), action->password_, boost::optional<std::string>(), false, false, action->isImpromptu_, action->isContinuation_));
        delete action->parent_;
    }
}

void QtPlainChatView::rejectMUCInvite()
{
    AcceptMUCInviteAction *action = dynamic_cast<AcceptMUCInviteAction*>(sender());
    if (action) {
        /* NOTE: no action required to reject an invite? */
        delete action->parent_;
    }
}

QtPlainChatView::FileTransfer::FileTransfer(QtPlainChatView* parent, bool senderIsSelf, const std::string& ftId, const std::string& filename, const ChatWindow::FileTransferState state, const std::string &desc, const std::string& msg, bool initializing)
: PopupDialog(parent), bar_(nullptr), senderIsSelf_(senderIsSelf), ftId_(ftId), filename_(filename), description_(desc), message_(msg), initializing_(initializing)
{
    QHBoxLayout* layout = new QHBoxLayout;
    QLabel* statusLabel = new QLabel;
    layout_->addWidget(statusLabel);
    layout_->addWidget(new QLabel); /* padding */

    if (initializing_) {
        FileTransfer::Action* accept = new FileTransfer::Action(senderIsSelf?"Confirm":"Accept", ftId);
        parent->connect(accept, SIGNAL(clicked()), SLOT(fileTransferAccept()));
        layout_->addWidget(accept);
        FileTransfer::Action* reject = new FileTransfer::Action(senderIsSelf?"Cancel":"Reject", ftId);
        parent->connect(reject, SIGNAL(clicked()), SLOT(fileTransferReject()));
        layout_->addWidget(reject);
        statusLabel->setText(P2QSTRING(msg));
        return;
    }

    std::string status = msg;

    switch (state) {
        case ChatWindow::Initialisation: {
            status = "Preparing to send <i>"+ filename + "</i>...";
            FileTransfer::Action* cancel = new FileTransfer::Action("Cancel", ftId);
            parent->connect(cancel, SIGNAL(clicked()), SLOT(fileTransferReject()));
            layout_->addWidget(cancel);
            break;
        }
        case ChatWindow::WaitingForAccept: {
            status = "Waiting for user to accept <i>" + filename + "</i>...";
            FileTransfer::Action* cancel = new FileTransfer::Action("Cancel", ftId);
            parent->connect(cancel, SIGNAL(clicked()), SLOT(fileTransferReject()));
            layout_->addWidget(cancel);
            break;
        }
        case ChatWindow::Negotiating: {
            status = "Preparing to transfer <i>" + filename + "</i>...";
            FileTransfer::Action* cancel = new FileTransfer::Action("Cancel", ftId);
            parent->connect(cancel, SIGNAL(clicked()), SLOT(fileTransferReject()));
            layout_->addWidget(cancel);
            break;
        }
        case ChatWindow::Transferring: {
            status = "Transferring <i>" + filename + "</i>...";
            bar_ = new QProgressBar;
            bar_->setRange(0, 100);
            bar_->setValue(0);
            layout->addWidget(bar_);
            FileTransfer::Action* cancel = new FileTransfer::Action("Cancel", ftId);
            parent->connect(cancel, SIGNAL(clicked()), SLOT(fileTransferReject()));
            layout_->addWidget(cancel);
            break;
        }
        case ChatWindow::Canceled: {
            status = "File <i>" + filename + "</i> was canceled.";
            FileTransfer::Action* finish = new FileTransfer::Action("Hide", ftId);
            parent->connect(finish, SIGNAL(clicked()), SLOT(fileTransferFinish()));
            layout_->addWidget(finish);
            break;
        }
        case ChatWindow::Finished: {
            status = "File <i>" + filename + "</i> was transfered successfully.";
            FileTransfer::Action* finish = new FileTransfer::Action("Hide", ftId);
            parent->connect(finish, SIGNAL(clicked()), SLOT(fileTransferFinish()));
            layout_->addWidget(finish);
            break;
        }
        case ChatWindow::FTFailed: {
            status = "File transfer failed: <i>" + filename + "</i>";
            FileTransfer::Action* finish = new FileTransfer::Action("Hide", ftId);
            parent->connect(finish, SIGNAL(clicked()), SLOT(fileTransferFinish()));
            layout_->addWidget(finish);
            break;
        }
    }

    statusLabel->setText(P2QSTRING(status));
}

void QtPlainChatView::LogTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();
    menu->exec(event->globalPos());
    delete menu;
}

}

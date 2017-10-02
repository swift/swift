/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/UserSearch/QtUserSearchFirstPage.h>

#include <Swiften/Base/Log.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtUserSearchFirstPage::QtUserSearchFirstPage(UserSearchWindow::Type type, const QString& title, SettingsProvider* settings) {
    setupUi(this);
    setTitle(title);
    setSubTitle(QString(tr("%1. If you know their address you can enter it directly, or you can search for them.")).arg(type == UserSearchWindow::Type::AddContact ? tr("Add another user to your contact list") : tr("Chat to another user")));
    jid_ = new QtSuggestingJIDInput(this, settings);
    horizontalLayout_2->addWidget(jid_);
    jidWarning_ = new QLabel(this);
    jidWarning_->setPixmap(QPixmap(":icons/warn.png"));
    jidWarning_->hide();
    horizontalLayout_2->addWidget(jidWarning_);
    setTabOrder(byJID_, jid_);
    setTabOrder(jid_, byLocalSearch_);
    setTabOrder(byLocalSearch_, byRemoteSearch_);
    connect(jid_, SIGNAL(textChanged(const QString&)), this, SLOT(emitCompletenessCheck()));
    connect(jid_, SIGNAL(editingDone()), this, SLOT(emitCompletenessCheck()));
    connect(service_->lineEdit(), SIGNAL(textChanged(const QString&)), this, SLOT(emitCompletenessCheck()));
}

bool QtUserSearchFirstPage::isComplete() const {
    bool complete = false;
    if (byJID_->isChecked()) {
        complete = JID(Q2PSTRING(jid_->text().trimmed())).isValid() && jidWarning_->toolTip().isEmpty();
    } else if (byLocalSearch_->isChecked()) {
        complete = true;
    } else if (byRemoteSearch_->isChecked()) {
        complete = JID(Q2PSTRING(service_->currentText().trimmed())).isValid();
    }
    return complete;
}

void QtUserSearchFirstPage::emitCompletenessCheck() {
    emit completeChanged();
}

}

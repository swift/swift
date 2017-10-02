/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtAdHocCommandWithJIDWindow.h>

#include <boost/bind.hpp>

#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>

#include <Swiften/Elements/Command.h>

#include <Swift/Controllers/UIEvents/RequestAdHocWithJIDUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>

#include <Swift/QtUI/QtFormWidget.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {
QtAdHocCommandWithJIDWindow::QtAdHocCommandWithJIDWindow(UIEventStream* uiEventStream) : uiEventStream_(uiEventStream) {
    QVBoxLayout* hlayout = new QVBoxLayout(this);

    QLabel* jidLabel = new QLabel("JID:", this);
    hlayout->addWidget(jidLabel);
    jid_ = new QLineEdit(this);
    hlayout->addWidget(jid_);

    QLabel* commandLabel = new QLabel("Command:", this);
    hlayout->addWidget(commandLabel);
    node_ = new QLineEdit(this);
    hlayout->addWidget(node_);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    QPushButton* rejectButton = buttonBox->addButton("Cancel", QDialogButtonBox::RejectRole);
    connect(rejectButton, SIGNAL(clicked()), this, SLOT(handleRejectClick()));
    QPushButton* acceptButton = buttonBox->addButton("Complete", QDialogButtonBox::AcceptRole);
    connect(acceptButton, SIGNAL(clicked()), this, SLOT(handleAcceptClick()));
    hlayout->addWidget(buttonBox);

    setLayout(hlayout);
    show();
}

QtAdHocCommandWithJIDWindow::~QtAdHocCommandWithJIDWindow() {
}

void QtAdHocCommandWithJIDWindow::handleAcceptClick() {
    const JID jid = JID(Q2PSTRING(jid_->text()));
    const std::string node = Q2PSTRING(node_->text());
    std::shared_ptr<UIEvent> event(new RequestAdHocWithJIDUIEvent(jid, node));
    uiEventStream_->send(event);
    accept();
}

void QtAdHocCommandWithJIDWindow::handleRejectClick() {
    reject();
}

}

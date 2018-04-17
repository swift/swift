/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtChatOverview.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QVBoxLayout>

#include <Swift/Controllers/Chat/Chattables.h>

#include <Swift/QtUI/ChattablesModel.h>
#include <Swift/QtUI/QtChatOverviewBundle.h>

namespace Swift {

QtChatOverview::QtChatOverview(Chattables& chattables, QWidget* parent) : QWidget(parent), chattables_(chattables) {
    QPalette newPalette = palette();
    newPalette.setColor(QPalette::Background, {38, 81, 112});
    setAutoFillBackground(true);
    newPalette.setColor(QPalette::Foreground, {255, 255, 255});
    setPalette(newPalette);

    auto mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    auto headerLayout = new QHBoxLayout();
    mainLayout->addLayout(headerLayout);
    auto allLabel = new QLabel(tr("All"), this);
    allLabel->setStyleSheet("color: white;");
    auto peopleLabel = new QLabel(tr("People"), this);
    peopleLabel->setStyleSheet("color: white;");
    auto roomsLabel = new QLabel(tr("Rooms"), this);
    roomsLabel->setStyleSheet("color: white;");
    headerLayout->addWidget(allLabel);
    headerLayout->addWidget(peopleLabel);
    headerLayout->addWidget(roomsLabel);

    rootModel_ = new ChattablesModel(chattables_, this);

    auto unreadBundle = new QtChatOverviewBundle(rootModel_, "UNREAD", true, this);
    connect(unreadBundle, SIGNAL(clicked(JID)), this, SLOT(handleItemClicked(JID)));
    mainLayout->addWidget(unreadBundle);

    auto peopleBundle = new QtChatOverviewBundle(rootModel_, "PEOPLE", false, this);
    connect(peopleBundle, SIGNAL(clicked(JID)), this, SLOT(handleItemClicked(JID)));
    mainLayout->addWidget(peopleBundle);

    auto roomsBundle = new QtChatOverviewBundle(rootModel_, "ROOMS", false, this);
    connect(roomsBundle, SIGNAL(clicked(JID)), this, SLOT(handleItemClicked(JID)));
    mainLayout->addWidget(roomsBundle);

    mainLayout->addStretch();
}

QtChatOverview::~QtChatOverview() {}

void QtChatOverview::handleItemClicked(JID jid) {
    chattables_.onActivated(jid);
}

} // namespace Swift

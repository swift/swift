/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtMainWindow.h>

#include <memory>

#include <boost/bind.hpp>
#include <boost/optional.hpp>

#include <QAction>
#include <QBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenuBar>
#include <QPushButton>
#include <QTabWidget>
#include <QToolBar>

#include <Swiften/Base/Platform.h>

#include <Swift/Controllers/SettingConstants.h>
#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestAdHocUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestAddUserDialogUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestBlockListDialogUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChatWithUserDialogUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestHistoryUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestJoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestProfileEditorUIEvent.h>

#include <Swift/QtUI/QtAdHocCommandWithJIDWindow.h>
#include <Swift/QtUI/QtLoginWindow.h>
#include <Swift/QtUI/QtSettingsProvider.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtTabWidget.h>
#include <Swift/QtUI/QtUISettingConstants.h>
#include <Swift/QtUI/Roster/QtFilterWidget.h>
#include <Swift/QtUI/Roster/QtRosterWidget.h>
#if defined(SWIFTEN_PLATFORM_MACOSX)
#include <Swift/QtUI/CocoaUIHelpers.h>
#elif defined(SWIFTEN_PLATFORM_WINDOWS)
#include <Swift/QtUI/WinUIHelpers.h>
#else
#include <Swift/QtUI/QtCertificateViewerDialog.h>
#endif

namespace Swift {

QtMainWindow::QtMainWindow(SettingsProvider* settings, UIEventStream* uiEventStream, QtLoginWindow::QtMenus loginMenus, StatusCache* statusCache, bool emoticonsExist, bool enableAdHocCommandOnJID) : QWidget(), MainWindow(false), loginMenus_(loginMenus) {
    uiEventStream_ = uiEventStream;
    settings_ = settings;
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    meView_ = new QtRosterHeader(settings, statusCache, this);
    mainLayout->addWidget(meView_);
    connect(meView_, SIGNAL(onChangeStatusRequest(StatusShow::Type, const QString&)), this, SLOT(handleStatusChanged(StatusShow::Type, const QString&)));
    connect(meView_, SIGNAL(onEditProfileRequest()), this, SLOT(handleEditProfileRequest()));
    connect(meView_, SIGNAL(onShowCertificateInfo()), this, SLOT(handleShowCertificateInfo()));

    tabs_ = new QtTabWidget(this);
#if QT_VERSION >= 0x040500
    tabs_->setDocumentMode(true);
#endif
    tabs_->setTabPosition(QTabWidget::South);
    mainLayout->addWidget(tabs_);
    contactsTabWidget_ = new QWidget(this);
    contactsTabWidget_->setContentsMargins(0, 0, 0, 0);
    QBoxLayout *contactTabLayout = new QBoxLayout(QBoxLayout::TopToBottom, contactsTabWidget_);
    contactsTabWidget_->setLayout(contactTabLayout);
    contactTabLayout->setSpacing(0);
    contactTabLayout->setContentsMargins(0, 0, 0, 0);

    treeWidget_ = new QtRosterWidget(uiEventStream_, settings_, this);

    contactTabLayout->addWidget(treeWidget_);
    new QtFilterWidget(this, treeWidget_, uiEventStream_, contactTabLayout);

    tabs_->addTab(contactsTabWidget_, tr("&Contacts"));

    eventWindow_ = new QtEventWindow(uiEventStream_);
    connect(eventWindow_, SIGNAL(onNewEventCountUpdated(int)), this, SLOT(handleEventCountUpdated(int)));

    chatListWindow_ = new QtChatListWindow(uiEventStream_, settings_);
    connect(chatListWindow_, SIGNAL(onCountUpdated(int)), this, SLOT(handleChatCountUpdated(int)));

    tabs_->addTab(chatListWindow_, tr("C&hats"));
    tabs_->addTab(eventWindow_, tr("&Notices"));

    tabs_->setCurrentIndex(settings_->getSetting(QtUISettingConstants::CURRENT_ROSTER_TAB));

    connect(tabs_, SIGNAL(currentChanged(int)), this, SLOT(handleTabChanged(int)));

    tabBarCombo_ = nullptr;
    if (settings_->getSetting(QtUISettingConstants::USE_SCREENREADER)) {
        tabs_->tabBar()->hide();
        tabBarCombo_ = new QComboBox(this);
        tabBarCombo_->setAccessibleName("Current View");
        tabBarCombo_->addItem(tr("Contacts"));
        tabBarCombo_->addItem(tr("Chats"));
        tabBarCombo_->addItem(tr("Notices"));
        tabBarCombo_->setCurrentIndex(tabs_->currentIndex());
        mainLayout->addWidget(tabBarCombo_);
        connect(tabBarCombo_, SIGNAL(currentIndexChanged(int)), tabs_, SLOT(setCurrentIndex(int)));
    }


    this->setLayout(mainLayout);

    QMenu* viewMenu = new QMenu(tr("&View"), this);
    menus_.push_back(viewMenu);

    compactRosterAction_ = new QAction(tr("&Compact Roster"), this);
    compactRosterAction_->setCheckable(true);
    compactRosterAction_->setChecked(false);
    connect(compactRosterAction_, SIGNAL(toggled(bool)), SLOT(handleCompactRosterToggled(bool)));
    viewMenu->addAction(compactRosterAction_);
    handleCompactRosterToggled(settings_->getSetting(QtUISettingConstants::COMPACT_ROSTER));

    showOfflineAction_ = new QAction(tr("&Show offline contacts"), this);
    showOfflineAction_->setCheckable(true);
    showOfflineAction_->setChecked(false);
    connect(showOfflineAction_, SIGNAL(toggled(bool)), SLOT(handleShowOfflineToggled(bool)));
    viewMenu->addAction(showOfflineAction_);
    handleShowOfflineToggled(settings_->getSetting(SettingConstants::SHOW_OFFLINE));

    if (emoticonsExist) {
        showEmoticonsAction_ = new QAction(tr("&Show Emoticons"), this);
        showEmoticonsAction_->setCheckable(true);
        showEmoticonsAction_->setChecked(false);
        connect(showEmoticonsAction_, SIGNAL(toggled(bool)), SLOT(handleShowEmoticonsToggled(bool)));
        viewMenu->addAction(showEmoticonsAction_);
        handleShowEmoticonsToggled(settings_->getSetting(QtUISettingConstants::SHOW_EMOTICONS));
    }

    QMenu* actionsMenu = new QMenu(tr("&Actions"), this);
    menus_.push_back(actionsMenu);
    QAction* editProfileAction = new QAction(tr("Edit &Profile…"), this);
    connect(editProfileAction, SIGNAL(triggered()), SLOT(handleEditProfileAction()));
    actionsMenu->addAction(editProfileAction);
    onlineOnlyActions_ << editProfileAction;
    QAction* joinMUCAction = new QAction(tr("Enter &Room…"), this);
    connect(joinMUCAction, SIGNAL(triggered()), SLOT(handleJoinMUCAction()));
    actionsMenu->addAction(joinMUCAction);
    onlineOnlyActions_ << joinMUCAction;
#ifdef SWIFT_EXPERIMENTAL_HISTORY
    QAction* viewLogsAction = new QAction(tr("&View History…"), this);
    connect(viewLogsAction, SIGNAL(triggered()), SLOT(handleViewLogsAction()));
    actionsMenu->addAction(viewLogsAction);
#endif
    openBlockingListEditor_ = new QAction(tr("Edit &Blocking List…"), this);
    connect(openBlockingListEditor_, SIGNAL(triggered()), SLOT(handleEditBlockingList()));
    actionsMenu->addAction(openBlockingListEditor_);
    onlineOnlyActions_ << openBlockingListEditor_;
    openBlockingListEditor_->setVisible(false);
    addUserAction_ = new QAction(tr("&Add Contact…"), this);
    addUserAction_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
    addUserAction_->setShortcutContext(Qt::ApplicationShortcut);
    connect(addUserAction_, SIGNAL(triggered(bool)), this, SLOT(handleAddUserActionTriggered(bool)));
    actionsMenu->addAction(addUserAction_);
    onlineOnlyActions_ << addUserAction_;
    editUserAction_ = new QAction(tr("&Edit Selected Contact…"), this);
    connect(editUserAction_, SIGNAL(triggered(bool)), treeWidget_, SLOT(handleEditUserActionTriggered(bool)));
    actionsMenu->addAction(editUserAction_);
    onlineOnlyActions_ << editUserAction_;
    editUserAction_->setEnabled(false);
    chatUserAction_ = new QAction(tr("Start &Chat…"), this);
    chatUserAction_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    chatUserAction_->setShortcutContext(Qt::ApplicationShortcut);
    connect(chatUserAction_, SIGNAL(triggered(bool)), this, SLOT(handleChatUserActionTriggered(bool)));
    actionsMenu->addAction(chatUserAction_);
    onlineOnlyActions_ << chatUserAction_;
    if (enableAdHocCommandOnJID) {
        otherAdHocAction_ = new QAction(tr("Run Other Command"), this);
        connect(otherAdHocAction_, SIGNAL(triggered()), this, SLOT(handleOtherAdHocActionTriggered()));
        actionsMenu->addAction(otherAdHocAction_);
        onlineOnlyActions_ << otherAdHocAction_;
    }
    serverAdHocMenu_ = new QMenu(tr("Run Server Command"), this);
    actionsMenu->addMenu(serverAdHocMenu_);
    actionsMenu->addSeparator();
    QAction* signOutAction = new QAction(tr("&Sign Out"), this);
    connect(signOutAction, SIGNAL(triggered()), SLOT(handleSignOutAction()));
    actionsMenu->addAction(signOutAction);

    toggleRequestDeliveryReceipts_ = new QAction(tr("&Request Delivery Receipts"), this);
    toggleRequestDeliveryReceipts_->setCheckable(true);
    toggleRequestDeliveryReceipts_->setChecked(settings_->getSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS));
    connect(toggleRequestDeliveryReceipts_, SIGNAL(toggled(bool)), SLOT(handleToggleRequestDeliveryReceipts(bool)));

    QList< QAction* > generalMenuActions = loginMenus_.generalMenu->actions();
    loginMenus_.generalMenu->insertAction(generalMenuActions.at(generalMenuActions.count()-2),toggleRequestDeliveryReceipts_);

    treeWidget_->onSomethingSelectedChanged.connect(boost::bind(&QtMainWindow::handleSomethingSelectedChanged, this, _1));

    setAvailableAdHocCommands(std::vector<DiscoItems::Item>());
    QAction* adHocAction = new QAction(tr("Collecting commands..."), this);
    adHocAction->setEnabled(false);
    serverAdHocMenu_->addAction(adHocAction);
    serverAdHocCommandActions_.append(adHocAction);

    settings_->onSettingChanged.connect(boost::bind(&QtMainWindow::handleSettingChanged, this, _1));
}

QtMainWindow::~QtMainWindow() {
    settings_->onSettingChanged.disconnect(boost::bind(&QtMainWindow::handleSettingChanged, this, _1));
}

void QtMainWindow::handleTabChanged(int index) {
    settings_->storeSetting(QtUISettingConstants::CURRENT_ROSTER_TAB, index);
}

void QtMainWindow::handleToggleRequestDeliveryReceipts(bool enabled) {
    settings_->storeSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS, enabled);
}

void QtMainWindow::handleShowCertificateInfo() {
    onShowCertificateRequest();
}

void QtMainWindow::handleEditBlockingList() {
    uiEventStream_->send(std::make_shared<RequestBlockListDialogUIEvent>());
}

void QtMainWindow::handleSomethingSelectedChanged(bool itemSelected) {
    bool isOnline = addUserAction_->isEnabled();
    editUserAction_->setEnabled(isOnline && itemSelected);
}

QtEventWindow* QtMainWindow::getEventWindow() {
    return eventWindow_;
}

QtChatListWindow* QtMainWindow::getChatListWindow() {
    return chatListWindow_;
}

void QtMainWindow::setRosterModel(Roster* roster) {
    treeWidget_->setRosterModel(roster);
}

void QtMainWindow::handleEditProfileRequest() {
    uiEventStream_->send(std::make_shared<RequestProfileEditorUIEvent>());
}

void QtMainWindow::handleEventCountUpdated(int count) {
    QColor eventTabColor = (count == 0) ? QColor() : QColor(255, 0, 0); // invalid resets to default
    int eventIndex = 2;
    tabs_->tabBar()->setTabTextColor(eventIndex, eventTabColor);
    QString text = tr("&Notices");
    if (count > 0) {
        text += QString(" (%1)").arg(count);
    }
    tabs_->setTabText(eventIndex, text);
}

void QtMainWindow::handleChatCountUpdated(int count) {
    QColor chatTabColor = (count == 0) ? QColor() : QColor(255, 0, 0); // invalid resets to default
    int chatIndex = 1;
    tabs_->tabBar()->setTabTextColor(chatIndex, chatTabColor);
    QString text = tr("C&hats");
    if (count > 0) {
        text += QString(" (%1)").arg(count);
    }
    tabs_->setTabText(chatIndex, text);
}

void QtMainWindow::handleAddUserActionTriggered(bool /*checked*/) {
    std::shared_ptr<UIEvent> event(new RequestAddUserDialogUIEvent());
    uiEventStream_->send(event);
}

void QtMainWindow::handleChatUserActionTriggered(bool /*checked*/) {
    std::shared_ptr<UIEvent> event(new RequestChatWithUserDialogUIEvent());
    uiEventStream_->send(event);
}

void QtMainWindow::handleOtherAdHocActionTriggered() {
    new QtAdHocCommandWithJIDWindow(uiEventStream_);
}

void QtMainWindow::handleSignOutAction() {
    loginMenus_.generalMenu->removeAction(toggleRequestDeliveryReceipts_);
    onSignOutRequest();
}

void QtMainWindow::handleEditProfileAction() {
    uiEventStream_->send(std::make_shared<RequestProfileEditorUIEvent>());
}

void QtMainWindow::handleJoinMUCAction() {
    uiEventStream_->send(std::make_shared<RequestJoinMUCUIEvent>());
}

void QtMainWindow::handleViewLogsAction() {
    uiEventStream_->send(std::make_shared<RequestHistoryUIEvent>());
}

void QtMainWindow::handleStatusChanged(StatusShow::Type showType, const QString &statusMessage) {
    onChangeStatusRequest(showType, Q2PSTRING(statusMessage));
}

void QtMainWindow::handleSettingChanged(const std::string& settingPath) {
    if (settingPath == SettingConstants::SHOW_OFFLINE.getKey()) {
        handleShowOfflineToggled(settings_->getSetting(SettingConstants::SHOW_OFFLINE));
    }
    if (settingPath == QtUISettingConstants::SHOW_EMOTICONS.getKey()) {
        handleShowEmoticonsToggled(settings_->getSetting(QtUISettingConstants::SHOW_EMOTICONS));
    }
    if (settingPath == SettingConstants::REQUEST_DELIVERYRECEIPTS.getKey()) {
        toggleRequestDeliveryReceipts_->setChecked(settings_->getSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS));
    }
    if (settingPath == QtUISettingConstants::COMPACT_ROSTER.getKey()) {
        handleCompactRosterToggled(settings_->getSetting(QtUISettingConstants::COMPACT_ROSTER));
    }
}

void QtMainWindow::handleCompactRosterToggled(bool state) {
    settings_->storeSetting(QtUISettingConstants::COMPACT_ROSTER, state);
    compactRosterAction_->setChecked(settings_->getSetting(QtUISettingConstants::COMPACT_ROSTER));
}

void QtMainWindow::handleShowOfflineToggled(bool state) {
    settings_->storeSetting(SettingConstants::SHOW_OFFLINE, state);
    showOfflineAction_->setChecked(settings_->getSetting(SettingConstants::SHOW_OFFLINE));
}

void QtMainWindow::handleShowEmoticonsToggled(bool state) {
    settings_->storeSetting(QtUISettingConstants::SHOW_EMOTICONS, state);
    showEmoticonsAction_->setChecked(settings_->getSetting(QtUISettingConstants::SHOW_EMOTICONS));
}

void QtMainWindow::setMyNick(const std::string& nick) {
    meView_->setNick(P2QSTRING(nick));
}

void QtMainWindow::setMyJID(const JID& jid) {
    meView_->setJID(P2QSTRING(jid.toBare().toString()));
}

void QtMainWindow::setMyAvatarPath(const std::string& path) {
    meView_->setAvatar(P2QSTRING(path));
}

void QtMainWindow::setMyStatusText(const std::string& status) {
    meView_->setStatusText(P2QSTRING(status));
}

void QtMainWindow::setMyStatusType(StatusShow::Type type) {
    meView_->setStatusType(type);
    const bool online = (type != StatusShow::None);
    treeWidget_->setOnline(online);
    chatListWindow_->setOnline(online);
    for (auto action : onlineOnlyActions_) {
        action->setEnabled(online);
    }
    serverAdHocMenu_->setEnabled(online);
}

void QtMainWindow::setMyContactRosterItem(std::shared_ptr<ContactRosterItem> contact) {
    meView_->setContactRosterItem(contact);
}

void QtMainWindow::setConnecting() {
    meView_->setConnecting();
}

void QtMainWindow::setStreamEncryptionStatus(bool tlsInPlaceAndValid) {
    meView_->setStreamEncryptionStatus(tlsInPlaceAndValid);
}

void QtMainWindow::openCertificateDialog(const std::vector<Certificate::ref>& chain) {
    openCertificateDialog(chain, this);
}

void QtMainWindow::openCertificateDialog(const std::vector<Certificate::ref>& chain, QWidget* parent) {
#if defined(SWIFTEN_PLATFORM_MACOSX)
    CocoaUIHelpers::displayCertificateChainAsSheet(parent, chain);
#elif defined(SWIFTEN_PLATFORM_WINDOWS)
    WinUIHelpers::displayCertificateChainAsSheet(parent, chain);
#else
    QtCertificateViewerDialog::displayCertificateChainAsSheet(parent, chain);
#endif
}

void QtMainWindow::handleAdHocActionTriggered(bool /*checked*/) {
    QAction* action = qobject_cast<QAction*>(sender());
    assert(action);
    assert(serverAdHocCommandActions_.indexOf(action) >= 0);
    DiscoItems::Item command = serverAdHocCommands_[serverAdHocCommandActions_.indexOf(action)];
    uiEventStream_->send(std::make_shared<RequestAdHocUIEvent>(command));
}

void QtMainWindow::setAvailableAdHocCommands(const std::vector<DiscoItems::Item>& commands) {
    serverAdHocCommands_ = commands;
    for (auto action : serverAdHocCommandActions_) {
        delete action;
    }
    serverAdHocMenu_->clear();
    serverAdHocCommandActions_.clear();
    for (const auto& command : commands) {
        QAction* action = new QAction(P2QSTRING(command.getName()), this);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(handleAdHocActionTriggered(bool)));
        serverAdHocMenu_->addAction(action);
        serverAdHocCommandActions_.append(action);
    }
    if (serverAdHocCommandActions_.isEmpty()) {
        QAction* action = new QAction(tr("No Available Commands"), this);
        action->setEnabled(false);
        serverAdHocMenu_->addAction(action);
        serverAdHocCommandActions_.append(action);
    }
}

void QtMainWindow::setBlockingCommandAvailable(bool isAvailable) {
    openBlockingListEditor_->setVisible(isAvailable);
}

}


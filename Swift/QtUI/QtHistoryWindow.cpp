/*
 * Copyright (c) 2012 Catalin Badea
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <QtHistoryWindow.h>
#include <QtTabbable.h>

#include <QtSwiftUtil.h>
#include <MessageSnippet.h>
#include <Swiften/History/HistoryMessage.h>
#include <string>

#include <boost/shared_ptr.hpp>

#include <QTime>
#include <QUrl>
#include <QMenu>
#include <QTextDocument>
#include <QDateTime>
#include <Swift/QtUI/QtScaledAvatarCache.h>
#include <QLineEdit>
#include "QtUtilities.h"

#include <boost/smart_ptr/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace Swift {

QtHistoryWindow::QtHistoryWindow(SettingsProvider* settings, UIEventStream* eventStream) :
		previousTopMessageWasSelf_(false),
		previousBottomMessageWasSelf_(false) {
	ui_.setupUi(this);

	theme_ = new QtChatTheme("");
	idCounter_ = 0;

	delete ui_.conversation_;
	conversation_ = new QtChatView(theme_, this, true);
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	sizePolicy.setHorizontalStretch(80);
	sizePolicy.setVerticalStretch(0);
	conversation_->setSizePolicy(sizePolicy);

	ui_.conversation_ = conversation_;
	ui_.bottomLayout_->addWidget(conversation_);

	delete ui_.conversationRoster_;
	conversationRoster_ = new QtTreeWidget(eventStream, settings, this);
	QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Expanding);
	sizePolicy2.setVerticalStretch(80);
	conversationRoster_->setSizePolicy(sizePolicy2);
	ui_.conversationRoster_ = conversationRoster_;
	ui_.bottomLeftLayout_->setDirection(QBoxLayout::BottomToTop);
	ui_.bottomLeftLayout_->addWidget(conversationRoster_);

	setWindowTitle(tr("History"));

	conversationRoster_->onSomethingSelectedChanged.connect(boost::bind(&QtHistoryWindow::handleSomethingSelectedChanged, this, _1));
	connect(conversation_, SIGNAL(scrollRequested(int)), this, SLOT(handleScrollRequested(int)));
	connect(conversation_, SIGNAL(scrollReachedTop()), this, SLOT(handleScrollReachedTop()));
	connect(conversation_, SIGNAL(scrollReachedBottom()), this, SLOT(handleScrollReachedBottom()));
	connect(conversation_, SIGNAL(fontResized(int)), this, SLOT(handleFontResized(int)));
	connect(ui_.searchBox_->lineEdit(), SIGNAL(returnPressed()), this, SLOT(handleReturnPressed()));
	connect(ui_.calendarWidget_, SIGNAL(clicked(const QDate&)), this, SLOT(handleCalendarClicked(const QDate&)));
	connect(ui_.calendarWidget_, SIGNAL(activated(const QDate&)), this, SLOT(handleCalendarClicked(const QDate&)));
	connect(ui_.previousButton_, SIGNAL(clicked(bool)), this, SLOT(handlePreviousButtonClicked()));
	connect(ui_.nextButton_, SIGNAL(clicked(bool)), this, SLOT(handleNextButtonClicked()));
}

QtHistoryWindow::~QtHistoryWindow() {
	disconnect(conversation_, SIGNAL(scrollRequested(int)), this, SLOT(handleScrollRequested(int)));
	disconnect(conversation_, SIGNAL(scrollReachedTop()), this, SLOT(handleScrollReachedTop()));
	disconnect(conversation_, SIGNAL(scrollReachedBottom()), this, SLOT(handleScrollReachedBottom()));
	disconnect(conversation_, SIGNAL(fontResized(int)), this, SLOT(handleFontResized(int)));
	disconnect(ui_.searchBox_->lineEdit(), SIGNAL(returnPressed()), this, SLOT(handleReturnPressed()));
	disconnect(ui_.calendarWidget_, SIGNAL(clicked(const QDate&)), this, SLOT(handleCalendarClicked(const QDate&)));
	disconnect(ui_.calendarWidget_, SIGNAL(activated(const QDate&)), this, SLOT(handleCalendarClicked(const QDate&)));
	disconnect(ui_.previousButton_, SIGNAL(clicked(bool)), this, SLOT(handlePreviousButtonClicked()));
	disconnect(ui_.nextButton_, SIGNAL(clicked(bool)), this, SLOT(handleNextButtonClicked()));

	delete theme_;
	delete conversation_;
	// TODO: delete ui_
}

void QtHistoryWindow::activate() {
	emit wantsToActivate();
}

void QtHistoryWindow::showEvent(QShowEvent* event) {
	emit windowOpening();
	emit titleUpdated();
	QWidget::showEvent(event);
}

void QtHistoryWindow::closeEvent(QCloseEvent* event) {
	emit windowClosing();
	event->accept();
}

void QtHistoryWindow::setRosterModel(Roster* model) {
	conversationRoster_->setRosterModel(model);
}

void QtHistoryWindow::addMessage(const std::string &message, const std::string &senderName, bool senderIsSelf, const std::string& avatarPath, const boost::posix_time::ptime& time, bool addAtTheTop) {
	QString scaledAvatarPath = QtScaledAvatarCache(32).getScaledAvatarPath(avatarPath.c_str());

	QString messageHTML(P2QSTRING(message));
	messageHTML = QtUtilities::htmlEscape(messageHTML);
	QString searchTerm = ui_.searchBox_->lineEdit()->text();
	if (searchTerm.length()) {
		messageHTML.replace(searchTerm, "<span style='background-color: yellow'>" + searchTerm + "</span>");
	}

	// note: time uses localtime
	QDate date = QDate(time.date().year(), time.date().month(), time.date().day());
	QTime dayTime = QTime(time.time_of_day().hours(), time.time_of_day().minutes(), time.time_of_day().seconds());
	QDateTime qTime = QDateTime(date, dayTime);

	std::string id = "id" + boost::lexical_cast<std::string>(idCounter_++);

	QString qAvatarPath =  scaledAvatarPath.isEmpty() ? "qrc:/icons/avatar.png" : QUrl::fromLocalFile(scaledAvatarPath).toEncoded();

	if (addAtTheTop) {
		bool appendToPrevious = ((senderIsSelf && previousTopMessageWasSelf_) || (!senderIsSelf && !previousTopMessageWasSelf_&& previousTopSenderName_ == P2QSTRING(senderName)));
		conversation_->addMessageTop(boost::shared_ptr<ChatSnippet>(new MessageSnippet(messageHTML, QtUtilities::htmlEscape(P2QSTRING(senderName)), qTime, qAvatarPath, senderIsSelf, appendToPrevious, theme_, P2QSTRING(id))));

		previousTopMessageWasSelf_ = senderIsSelf;
		previousTopSenderName_ = P2QSTRING(senderName);
	}
	else {
		bool appendToPrevious = ((senderIsSelf && previousBottomMessageWasSelf_) || (!senderIsSelf && !previousBottomMessageWasSelf_&& previousBottomSenderName_ == P2QSTRING(senderName)));
		conversation_->addMessageBottom(boost::shared_ptr<ChatSnippet>(new MessageSnippet(messageHTML, QtUtilities::htmlEscape(P2QSTRING(senderName)), qTime, qAvatarPath, senderIsSelf, appendToPrevious, theme_, P2QSTRING(id))));
		previousBottomMessageWasSelf_ = senderIsSelf;
		previousBottomSenderName_ = P2QSTRING(senderName);
	}

	// keep track of the days viewable in the chatView
	if (!dates_.count(date)) {
		dates_.insert(date);
	}
}

void QtHistoryWindow::handleSomethingSelectedChanged(RosterItem* item) {
	onSelectedContactChanged(item);
}

void QtHistoryWindow::resetConversationView() {
	previousTopMessageWasSelf_ = false;
	previousBottomMessageWasSelf_ = false;
	previousTopSenderName_.clear();
	previousBottomSenderName_.clear();

	dates_.clear();
	conversation_->resetView();
}

void QtHistoryWindow::handleScrollRequested(int pos) {
	// first message starts with offset 5
	if (pos < 5) {
		pos = 5;
	}

	QDate currentDate;
	foreach (const QDate& date, dates_) {
		int snippetPosition = conversation_->getSnippetPositionByDate(date);
		if (snippetPosition <= pos) {
			currentDate = date;
		}
	}

	if (ui_.calendarWidget_->selectedDate() != currentDate) {
		ui_.calendarWidget_->setSelectedDate(currentDate);
	}
}

void QtHistoryWindow::handleScrollReachedTop() {
	if (dates_.empty()) {
		return;
	}

	int year, month, day;
	QDate firstDate = *dates_.begin();
	firstDate.getDate(&year, &month, &day);
	onScrollReachedTop(boost::gregorian::date(boost::numeric_cast<unsigned short>(year), boost::numeric_cast<unsigned short>(month), boost::numeric_cast<unsigned short>(day)));
}

void QtHistoryWindow::handleScrollReachedBottom() {
	if (dates_.empty()) {
		return;
	}

	int year, month, day;
	QDate lastDate = *dates_.rbegin();
	lastDate.getDate(&year, &month, &day);
	onScrollReachedBottom(boost::gregorian::date(boost::numeric_cast<unsigned short>(year), boost::numeric_cast<unsigned short>(month), boost::numeric_cast<unsigned short>(day)));
}

void QtHistoryWindow::handleReturnPressed() {
	onReturnPressed(ui_.searchBox_->lineEdit()->text().toStdString());
}

void QtHistoryWindow::handleCalendarClicked(const QDate& date) {
	int year, month, day;
	QDate tempDate = date; // getDate discards const qualifier
	tempDate.getDate(&year, &month, &day);
	onCalendarClicked(boost::gregorian::date(boost::numeric_cast<unsigned short>(year), boost::numeric_cast<unsigned short>(month), boost::numeric_cast<unsigned short>(day)));
}

void QtHistoryWindow::setDate(const boost::gregorian::date& date) {
	ui_.calendarWidget_->setSelectedDate(QDate::fromJulianDay(date.julian_day()));
}

void QtHistoryWindow::handleNextButtonClicked() {
	onNextButtonClicked();
}

void QtHistoryWindow::handlePreviousButtonClicked() {
	onPreviousButtonClicked();
}

void QtHistoryWindow::handleFontResized(int fontSizeSteps) {
	conversation_->resizeFont(fontSizeSteps);

	emit fontResized(fontSizeSteps);
}

void QtHistoryWindow::resetConversationViewTopInsertPoint() {
	previousTopMessageWasSelf_ = false;
	previousTopSenderName_ = QString();
	conversation_->resetTopInsertPoint();
}

std::string QtHistoryWindow::getSearchBoxText() {
	return ui_.searchBox_->lineEdit()->text().toStdString();
}

boost::gregorian::date QtHistoryWindow::getLastVisibleDate() {
	if (!dates_.empty()) {
		QDate lastDate = *dates_.rbegin();
		int year, month, day;
		lastDate.getDate(&year, &month, &day);

		return boost::gregorian::date(boost::numeric_cast<unsigned short>(year), boost::numeric_cast<unsigned short>(month), boost::numeric_cast<unsigned short>(day));
	}
	return boost::gregorian::date(boost::gregorian::not_a_date_time);
}

}
